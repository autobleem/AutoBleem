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
    if (gpu.empty())
    {
        gpu = PCSX_NEON;
    }

    cout << "Using GPU plugin:" << gpu << endl;

    string RACore = RA_NEON;
    if (gpu!=PCSX_NEON)
    {
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

    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }
    waitpid(pid, NULL, 0);
    usleep(3 * 1000);
    return true;
}

//*******************************
// RetroArchInterceptor::memcardIn
//*******************************
void RetroArchInterceptor::memcardIn(PsGamePtr & game) {
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
    string outpath = string("") + RA_MEMCARDLOC + Util::separator() + game->base +".srm";
    string backup = outpath+".bak";
    if (!Util::exists(backup)) {
        Util::copy(outpath, backup);
    }
    Util::copy(inpath,outpath);
}

//*******************************
// RetroArchInterceptor::memcardOut
//*******************************
void RetroArchInterceptor::memcardOut(PsGamePtr & game) {
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
    string inpath = string("") + RA_MEMCARDLOC + Util::separator() + game->base +".srm";
    string backup = inpath+".bak";
    Util::copy(inpath,outpath);
    remove(inpath.c_str());
    if (Util::exists(backup))
    {
        rename(backup.c_str(),inpath.c_str());
    }
}