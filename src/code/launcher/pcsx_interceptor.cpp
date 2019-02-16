//
// Created by screemer on 2/13/19.
//

#include <wait.h>
#include "pcsx_interceptor.h"
#include "../util.h"
#include "../gui/gui.h"
#include "../lang.h"

bool PcsxInterceptor::execute(PsGame *game) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->saveSelection();
    std::vector<const char *> argvNew;
    string gameIso = "";

    string region = "3"; // need to find out if console is jap to switch to 2 - later on
    string link = "/media/AutoBleem/rc/launch.sh";

    trim(game->ssFolder);
    if (game->ssFolder.back() == Util::separator()[0]) {
        game->ssFolder = game->ssFolder.substr(0, game->ssFolder.size() - 1);
    }

    argvNew.push_back(link.c_str());
    argvNew.push_back(game->ssFolder.c_str());

    gameIso += (game->folder + game->base);
    if (!Util::matchExtension(game->base, ".pbp")) {
        gameIso += ".cue";
    }
    gameIso += "";

    argvNew.push_back(gameIso.c_str());
    // hack to get language from lang file
    string langStr = _("|@lang|");
    if (langStr == "|@lang|") {
        langStr = 1;
    }
    argvNew.push_back(langStr.c_str()); // lang by language file hack
    argvNew.push_back(region.c_str());
    argvNew.push_back(game->folder.c_str());
    argvNew.push_back(nullptr);


    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }

    waitpid(pid, NULL, 0);

    sleep(2);

}