//
// Created by screemer on 2/13/19.
//

#include <sys/wait.h>
#include "pcsx_interceptor.h"
#include "../util.h"
#include "../gui/gui.h"
#include "../lang.h"
#include "../engine/memcard.h"

bool PcsxInterceptor::execute(PsGame *game, int resumepoint) {

    shared_ptr<Gui> gui(Gui::getInstance());
    gui->saveSelection();
    std::vector<const char *> argvNew;
    string gameIso = "";

    string region = "2"; // need to find out if console is jap to switch to 2 - later on
    string link = "/media/Autobleem/rc/launch.sh";
    string aspect = "0";
    if (gui->cfg.inifile.values["aspect"]=="true")
    {
        aspect = "1";
    }

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
        langStr = "2";
    }
    argvNew.push_back(langStr.c_str()); // lang by language file hack
    argvNew.push_back(region.c_str());
    argvNew.push_back(game->folder.c_str());
    argvNew.push_back(to_string(resumepoint+1).c_str());
    argvNew.push_back(aspect.c_str());
    argvNew.push_back(nullptr);


    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }

    waitpid(pid, NULL, 0);

    sleep(2);
    return true;
}

void PcsxInterceptor::memcardIn(PsGame *game)
{
    if (game->memcard!="SONY")
    {
        if (Util::exists("/media/Games/!MemCards/"+game->memcard))
        {
            Memcard * card = new Memcard("/media/Games/");
            if (!card->swapIn(game->ssFolder,game->memcard))
            {
                game->setMemCard("SONY");
            };
            delete card;
        }
    }
}

void PcsxInterceptor::memcardOut(PsGame *game)
{
    if (game->memcard!="SONY")
    {
        Memcard * card = new Memcard("/media/Games/");
        card->swapOut(game->ssFolder,game->memcard);
        delete card;

    }
}

void PcsxInterceptor::prepareResumePoint(PsGame *game, int pointId)
{
    if (pointId==-1)
        return;
    string filenamefile = game->ssFolder+"filename.txt.res";
    if (Util::exists(filenamefile))
    {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            // last line is our filename
            string ssfile =  game->ssFolder+"sstates/"+line+".00"+to_string(pointId)+".res";
            string newName =  game->ssFolder+"sstates/"+line+".00"+to_string(pointId);
            if (Util::exists(ssfile))
            {
                remove(newName.c_str());
                Util::copy(ssfile.c_str(),newName.c_str());
            }
            is.close();
        }
    }
}




