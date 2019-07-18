//
// Created by screemer on 2019-07-18.
//

#include "retboot_interceptor.h"
#include <sys/wait.h>
#include "../util.h"
#include "../gui/gui.h"
#include "../lang.h"
#include "../engine/memcard.h"
#include <fstream>
#include <iostream>
#include <unistd.h>

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

    argvNew.push_back(gameIso.c_str());
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