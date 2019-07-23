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
    string gameIso = "";

    cout << "Starting RetroArch Emu" << endl;

    string link = "/media/Autobleem/rc/launch_rb.sh";
    argvNew.push_back(link.c_str());


    gameIso += (game->folder + game->base);
    if (!Util::matchExtension(game->base, ".pbp")) {
        gameIso += ".cue";
    }
    gameIso += "";

    // figure out which plugin is selected
    CfgProcessor *processor = new CfgProcessor();
    string path = game->folder;
    if (game->internal) {
        path = game->ssFolder;
    }

    string gpu = processor->getValue(game->base, path, "gpu3", internal);
    gpu = trim(gpu);
    if (gpu.empty()) {
        gpu = PCSX_NEON;
    }
    delete (processor);

    cout << "Using GPU plugin:" << gpu << endl;

    string RACore = RA_NEON;
    if (gpu != PCSX_NEON) {
        RACore = RA_PEOPS;
    }
    argvNew.push_back(gameIso.c_str());
    argvNew.push_back(RACore.c_str());
    argvNew.push_back(nullptr);

    for (const char *s:argvNew) {
        if (s != nullptr) {
            cout << s << " ";
        }
    }
    cout << endl;

    backupCoreConfig();
    transferConfig(game);
    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }
    waitpid(pid, NULL, 0);
    usleep(3 * 1000);
    restoreCoreConfig();
    return true;
}

//*******************************
// RetroArchInterceptor::memcardIn
//*******************************
void RetroArchInterceptor::memcardIn(PsGamePtr &game) {
    string memcard = "SONY";
    if (!game->internal) {
        Inifile gameini;
        gameini.load(game->folder + "/Game.ini");
        memcard = gameini.values["memcard"];

    }
    if (memcard != "SONY") {
        if (Util::exists("/media/Games/!MemCards/" + game->memcard)) {
            Memcard *card = new Memcard("/media/Games/");
            if (!card->swapIn(game->ssFolder, game->memcard)) {
                game->setMemCard("SONY");
            };
            delete card;
        }
    }

    // Copy the card moved to RA

    string inpath = game->ssFolder + Util::separator() + "memcards" + Util::separator() + "card1.mcd";
    string outpath = string("") + RA_MEMCARDLOC + Util::separator() + game->base + ".srm";
    string backup = outpath + ".bak";
    if (!Util::exists(backup)) {
        Util::copy(outpath, backup);
    }
    Util::copy(inpath, outpath);
}

//*******************************
// RetroArchInterceptor::memcardOut
//*******************************
void RetroArchInterceptor::memcardOut(PsGamePtr &game) {
    string memcard = "SONY";
    if (!game->internal) {
        Inifile gameini;
        gameini.load(game->folder + "/Game.ini");
        memcard = gameini.values["memcard"];
    }
    if (memcard != "SONY") {
        Memcard *card = new Memcard("/media/Games/");
        card->swapOut(game->ssFolder, game->memcard);
        delete card;
    }

    string outpath = game->ssFolder + Util::separator() + "memcards" + Util::separator() + "card1.mcd";
    string inpath = string("") + RA_MEMCARDLOC + Util::separator() + game->base + ".srm";
    string backup = inpath + ".bak";
    Util::copy(inpath, outpath);
    remove(inpath.c_str());
    if (Util::exists(backup)) {
        rename(backup.c_str(), inpath.c_str());
    }
}


void RetroArchInterceptor::backupCoreConfig() {
    Util::copy(RA_CORE_CONFIG, string(RA_CORE_CONFIG) + ".bak");
    Util::copy(RA_CONFIG, string(RA_CONFIG) + ".bak");
}

void RetroArchInterceptor::restoreCoreConfig() {
    if (Util::exists(string(RA_CORE_CONFIG) + ".bak"))
        Util::copy(string(RA_CORE_CONFIG) + ".bak", RA_CORE_CONFIG);
    if (Util::exists(string(RA_CONFIG) + ".bak"))
        Util::copy(string(RA_CONFIG) + ".bak", RA_CONFIG);
}

void RetroArchInterceptor::transferConfig(PsGamePtr &game) {


    shared_ptr<Gui> gui(Gui::getInstance());

    string path = game->folder;
    if (game->internal) {
        path = game->ssFolder;
    }
    CfgProcessor *processor = new CfgProcessor();


    int highres = atoi(processor->getValue(game->base, path, "gpu_neon.enhancement_enable", internal).c_str());
    int speedhack = atoi(processor->getValue(game->base, path, "gpu_neon.enhancement_no_main", internal).c_str());
    int clock = strtol(processor->getValue(game->base, path, "psx_clock", internal).c_str(), NULL, 16);
    int dither = atoi(processor->getValue(game->base, path, "gpu_peops.iUseDither", internal).c_str());
    int interpolation = strtol(processor->getValue(game->base, path, "spu_config.iUseInterpolation", internal).c_str(),
                               NULL, 16);
    string aspect = gui->cfg.inifile.values["aspect"]; // true - 1280x720 - false 960x720
    string filter = gui->cfg.inifile.values["mip"]; // true - billiner
    int scanlines = atoi(processor->getValue(game->base, path, "scanlines", internal).c_str());
    int scanline_level = strtol(processor->getValue(game->base, path, "scanline_level", internal).c_str(),
                               NULL, 16);





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
                             "pcsx_rearmed_nocdaudio  = \"disabled\" ");

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

    // RA_CONFIG
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

    if (scanlines == 1)
    {

        float opacity = scanline_level/100.0f;
        processor->replaceRaConf(RA_CONFIG, "input_overlay",
                                 "input_overlay  = \":/overlay/scanlines.cfg\" ");
        processor->replaceRaConf(RA_CONFIG, "input_overlay_enable",
                                 "input_overlay_enable  = \"true\" ");
        processor->replaceRaConf(RA_CONFIG, "input_overlay_opacity",
                                 "input_overlay_opacity  = \""+to_string(opacity)+"\" ");
    }

    if (filter == "true")
    {
        processor->replaceRaConf(RA_CONFIG, "video_smooth",
                                 "video_smooth  = \"true\" ");
    } else
    {
        processor->replaceRaConf(RA_CONFIG, "video_smooth",
                                 "video_smooth  = \"false\" ");
    }




    delete processor;
}
