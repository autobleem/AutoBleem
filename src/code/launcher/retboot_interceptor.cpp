//
// Created by screemer on 2019-07-18.
//

#include "retboot_interceptor.h"
#include <sys/wait.h>
#include "../util.h"
#include "../gui/gui.h"
#include "../lang.h"
#include "../engine/memcard.h"
#include "../engine/cfgprocessor.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include "../environment.h"

#define RA_MEMCARDLOC "/media/retroarch/saves/"
#define RA_CORE_CONFIG "/media/retroarch/config/retroarch-core-options.cfg"
#define RA_CONFIG "/media/retroarch/config/retroarch.cfg"
#define RA_NEON "NEON"
#define RA_PEOPS "PEOPS"
#define PCSX_NEON "builtin_gpu"

bool RetroArchInterceptor::execute(PsGamePtr &game, int resumepoint) {
    shared_ptr<Gui> gui(Gui::getInstance());

    string padMapping = gui->padMapping;

    gui->saveSelection();
    std::vector<const char *> argvNew;
    string gameFile = "";

    cout << "Starting RetroArch Emu" << endl;

    if (game->foreign) {
        cout << "RA FOREIGN MODE" << endl;
    }
    string link = "/media/Autobleem/rc/launch_rb.sh";
    argvNew.push_back(link.c_str());


    if (!game->foreign) {
        gameFile += (game->folder + sep + game->base);
        if (!DirEntry::matchExtension(game->base, ".pbp")) {
            gameFile += ".cue";
        }
        gameFile += "";
        string base;
        if (DirEntry::isPBPFile(game->base)) {
            base = game->base.substr(0, game->base.length() - 4);
        } else {
            base = game->base;
        }
        if (DirEntry::exists(game->folder + sep + base + ".m3u")) {
            gameFile = game->folder + sep + base + ".m3u";
        }
    } else {
        gameFile = game->image_path + "";
    }
    // figure out which plugin is selected
    string gpu;
    if (!game->foreign) {
        CfgProcessor *processor = new CfgProcessor();
        string path = game->folder;
        if (game->internal) {
            path = game->ssFolder;
        }

        gpu = processor->getValue(path, "gpu3");
        gpu = Util::trim(gpu);
        if (gpu.empty()) {
            gpu = PCSX_NEON;
        }
        delete (processor);
    } else {
        gpu = "NONE";
    }
    cout << "Using GPU plugin: " << gpu << endl;

    string RACore = RA_NEON;
    if (gpu != PCSX_NEON) {
        RACore = RA_PEOPS;
    }

    if (game->foreign) {
        RACore = game->core_path;
    }
    argvNew.push_back(gameFile.c_str());
    argvNew.push_back(RACore.c_str());

    argvNew.push_back(nullptr);

    for (const char *s:argvNew) {
        if (s != nullptr) {
            cout << s << " ";
        }
    }
    cout << endl;

    // core config here - to be optional
    if (gui->cfg.inifile.values["raconfig"]=="true") {
        backupCoreConfig();
        transferConfig(game);
    }

    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }
    waitpid(pid, NULL, 0);
    usleep(3 * 1000);

    // core config here - to be optional
    if (gui->cfg.inifile.values["raconfig"]=="true") {
        restoreCoreConfig();
    }
    return true;
}

//*******************************
// RetroArchInterceptor::memcardIn
//*******************************
void RetroArchInterceptor::memcardIn(PsGamePtr &game) {
    if (!game->foreign) {
        string memcard = "SONY";
        if (!game->internal) {
            Inifile gameini;
            gameini.load(game->folder + sep + GAME_INI);
            memcard = gameini.values["memcard"];

        }
        if (memcard != "SONY") {
            if (DirEntry::exists(Env::getPathToMemCardsDir() + sep + game->memcard)) {
                Memcard *card = new Memcard(Env::getPathToGamesDir() + sep);
                if (!card->swapIn(game->ssFolder, game->memcard)) {
                    game->setMemCard("SONY");
                };
                delete card;
            }
        }

        // Copy the card moved to RA
        string base;
        if (DirEntry::isPBPFile(game->base)) {
            base = game->base.substr(0, game->base.length() - 4);
        } else {
            base = game->base;
        }

        string inpath = game->ssFolder + sep + "memcards" + sep + "card1.mcd";
        string outpath = string("") + RA_MEMCARDLOC + sep + base + ".srm";
        string backup = outpath + ".bak";
        if (!DirEntry::exists(backup)) {
            if (DirEntry::exists(outpath)) {
                DirEntry::copy(outpath, backup);
            }
        }
        if (DirEntry::exists(inpath)) {
            DirEntry::copy(inpath, outpath);
        }
    }
}

//*******************************
// RetroArchInterceptor::memcardOut
//*******************************
void RetroArchInterceptor::memcardOut(PsGamePtr &game) {
    if (!game->foreign) {
        string memcard = "SONY";
        if (!game->internal) {
            Inifile gameini;
            gameini.load(game->folder + sep + GAME_INI);
            memcard = gameini.values["memcard"];
        }
        if (memcard != "SONY") {
            Memcard *card = new Memcard(Env::getPathToGamesDir() + sep);
            card->swapOut(game->ssFolder, game->memcard);
            delete card;
        }
        string base;
        if (DirEntry::isPBPFile(game->base)) {
            base = game->base.substr(0, game->base.length() - 4);
        } else {
            base = game->base;
        }

        string outpath = game->ssFolder + sep + "memcards" + sep + "card1.mcd";
        string inpath = string("") + RA_MEMCARDLOC + sep + base + ".srm";
        string backup = inpath + ".bak";
        if (DirEntry::exists(inpath)) {
            DirEntry::copy(inpath, outpath);
        }


        if (DirEntry::exists(backup)) {
            DirEntry::removeFile(inpath);
            DirEntry::renameFile(backup, inpath);
        }
    }
}


void RetroArchInterceptor::backupCoreConfig() {

    DirEntry::copy(RA_CORE_CONFIG, string(RA_CORE_CONFIG) + ".bak");
    DirEntry::copy(RA_CONFIG, string(RA_CONFIG) + ".bak");
}

void RetroArchInterceptor::restoreCoreConfig() {
    if (DirEntry::exists(string(RA_CORE_CONFIG) + ".bak")) {
        DirEntry::copy(string(RA_CORE_CONFIG) + ".bak", RA_CORE_CONFIG);
        DirEntry::removeFile(string(RA_CORE_CONFIG) + ".bak");
    }
    if (DirEntry::exists(string(RA_CONFIG) + ".bak")) {
        DirEntry::copy(string(RA_CONFIG) + ".bak", RA_CONFIG);
        DirEntry::removeFile(string(RA_CONFIG) + ".bak");
    }
}

void RetroArchInterceptor::transferConfig(PsGamePtr &game) {
    shared_ptr<Gui> gui(Gui::getInstance());

    if (!game->foreign) {
        string path = game->folder;
        if (game->internal) {
            path = game->ssFolder;
        }
        CfgProcessor *processor = new CfgProcessor();

        int highres = atoi(processor->getValue(path, "gpu_neon.enhancement_enable").c_str());
        int speedhack = atoi(processor->getValue(path, "gpu_neon.enhancement_no_main").c_str());
        int clock = strtol(processor->getValue(path, "psx_clock").c_str(), NULL, 16);
        int dither = atoi(processor->getValue(path, "gpu_peops.iUseDither").c_str());
        int interpolation = strtol(
                processor->getValue(path, "spu_config.iUseInterpolation").c_str(), NULL, 16);

        int scanlines = atoi(processor->getValue(path, "scanlines").c_str());
        int scanline_level = strtol(processor->getValue(path, "scanline_level").c_str(), NULL, 16);
        int frameskip = atoi(processor->getValue(path, "frameskip3").c_str());


        //RA_CORE_CONFIG
        if (highres != 0)

            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_neon_enhancement_enable",
                                     "pcsx_rearmed_neon_enhancement_enable = \"enabled\" ");
        else

            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_neon_enhancement_enable",
                                     "pcsx_rearmed_neon_enhancement_enable = \"disabled\" ");

        if (dither != 0)

            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_dithering",
                                     "pcsx_rearmed_dithering = \"enabled\" ");
        else

            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_dithering",
                                     "pcsx_rearmed_dithering = \"disabled\" ");

        if (speedhack != 0)

            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_neon_enhancement_no_main",
                                     "pcsx_rearmed_neon_enhancement_no_main = \"enabled\" ");
        else

            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_neon_enhancement_no_main",
                                     "pcsx_rearmed_neon_enhancement_no_main = \"disabled\" ");

        processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_psxclock",
                                 "pcsx_rearmed_psxclock = \"" + to_string(clock) + "\" ");
        processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_show_bios_bootlogo",
                                 "pcsx_rearmed_show_bios_bootlogo  = \"enabled\" ");
        processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_nocdaudio",
                                 "pcsx_rearmed_nocdaudio  = \"enabled\" ");

        if (interpolation == 0) {
            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_spu_interpolation",
                                     "pcsx_rearmed_spu_interpolation = \"off\" ");
        }
        if (interpolation == 1) {
            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_spu_interpolation",
                                     "pcsx_rearmed_spu_interpolation = \"simple\" ");
        }
        if (interpolation == 2) {
            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_spu_interpolation",
                                     "pcsx_rearmed_spu_interpolation = \"gaussian\" ");
        }
        if (interpolation == 3) {
            processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_spu_interpolation",
                                     "pcsx_rearmed_spu_interpolation = \"cubic\" ");
        }

        processor->replaceRaConf(RA_CORE_CONFIG, "pcsx_rearmed_frameskip",
                                 "pcsx_rearmed_frameskip  = \"" + to_string(frameskip) + "\" ");
        if (scanlines == 1) {

            float opacity = scanline_level / 100.0f;
            processor->replaceRaConf(RA_CONFIG, "input_overlay",
                                     "input_overlay  = \":/overlay/scanlines.cfg\" ");
            processor->replaceRaConf(RA_CONFIG, "input_overlay_enable",
                                     "input_overlay_enable  = \"true\" ");
            processor->replaceRaConf(RA_CONFIG, "input_overlay_opacity",
                                     "input_overlay_opacity  = \"" + to_string(opacity) + "\" ");
        }
        delete processor;
    }

    // RA_CONFIG
    CfgProcessor *processor = new CfgProcessor();
    string aspect = gui->cfg.inifile.values["aspect"]; // true - 1280x720 - false 960x720
    string filter = gui->cfg.inifile.values["mip"]; // true - billiner
    if (aspect == "true") {
        // widescreen
        processor->replaceRaConf(RA_CONFIG, "custom_viewport_width",
                                 "custom_viewport_width  = \"1280\" ");
        processor->replaceRaConf(RA_CONFIG, "custom_viewport_height",
                                 "custom_viewport_height  = \"720\" ");
        processor->replaceRaConf(RA_CONFIG, "custom_viewport_x",
                                 "custom_viewport_x  = \"0\" ");
        processor->replaceRaConf(RA_CONFIG, "custom_viewport_y",
                                 "custom_viewport_y  = \"0\" ");
        processor->replaceRaConf(RA_CONFIG, "aspect_ratio_index",
                                 "aspect_ratio_index  = \"23\" ");

    } else {
        // 4:3
        processor->replaceRaConf(RA_CONFIG, "custom_viewport_width",
                                 "custom_viewport_width  = \"960\" ");
        processor->replaceRaConf(RA_CONFIG, "custom_viewport_height",
                                 "custom_viewport_height  = \"720\" ");
        processor->replaceRaConf(RA_CONFIG, "custom_viewport_x",
                                 "custom_viewport_x  = \"160\" ");
        processor->replaceRaConf(RA_CONFIG, "custom_viewport_y",
                                 "custom_viewport_y  = \"0\" ");
        processor->replaceRaConf(RA_CONFIG, "aspect_ratio_index",
                                 "aspect_ratio_index  = \"0\" ");

    }


    if (filter != "true") {
        processor->replaceRaConf(RA_CONFIG, "video_smooth",
                                 "video_smooth  = \"true\" ");
    } else {
        processor->replaceRaConf(RA_CONFIG, "video_smooth",
                                 "video_smooth  = \"false\" ");
    }
    delete processor;
}
