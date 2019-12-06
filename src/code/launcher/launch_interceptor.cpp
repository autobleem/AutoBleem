//
// Created by screemer on 2019-12-01.
//

#include "launch_interceptor.h"
#include "../util.h"
#include "../gui/gui.h"
#include <iostream>
#include <unistd.h>
#include "../environment.h"

#ifndef __APPLE__
#include <wait.h>
#endif
using namespace std;

bool LaunchInterceptor::execute(PsGamePtr &game, int resumepoint) {
    shared_ptr<Gui> gui(Gui::getInstance());
    cout << "Starting External App" << endl;

    if (game->foreign) {
        cout << "FOREIGN MODE" << endl;
    }

    gui->saveSelection();
    std::vector<const char *> argvNew;

    string link = game->base + sep+ game->startup;
    argvNew.push_back(link.c_str());
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

