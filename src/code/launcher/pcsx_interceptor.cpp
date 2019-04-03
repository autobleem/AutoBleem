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
    string lastCDpointX = game->ssFolder + "lastcdimg."+to_string(resumepoint)+".txt";
    gui->saveSelection();
    std::vector<const char *> argvNew;
    string gameIso = "";

    string region = "2"; // need to find out if console is jap to switch to 2 - later on
    string link = "/media/Autobleem/rc/launch.sh";
    string aspect = "0";
    if (gui->cfg.inifile.values["aspect"] == "true") {
        aspect = "1";
    }

    string filter = "0";
    if (gui->cfg.inifile.values["mip"] == "true") {
        filter = "1";
    } else {
        filter = "0";
    }

    trim(game->ssFolder);
    if (game->ssFolder.back() == Util::separator()[0]) {
        game->ssFolder = game->ssFolder.substr(0, game->ssFolder.size() - 1);
    }

    argvNew.push_back(link.c_str());
    argvNew.push_back(game->ssFolder.c_str());



    if (Util::exists(lastCDpointX))
    {
        ifstream is(lastCDpointX.c_str());
        if (is.is_open()) {
            std::string line;
            std::getline(is, line);

            // last line is our filename
            gameIso = line;
            is.close();
        }
    } else {
        gameIso += (game->folder + game->base);
        if (!Util::matchExtension(game->base, ".pbp")) {
            gameIso += ".cue";
        }
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
    if (resumepoint != -1) {
        argvNew.push_back("1");
    } else {
        argvNew.push_back("0");
    }
    argvNew.push_back(aspect.c_str());
    argvNew.push_back(filter.c_str());
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

void PcsxInterceptor::memcardIn(PsGame *game) {
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
}

void PcsxInterceptor::memcardOut(PsGame *game) {
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
}

void PcsxInterceptor::saveResumePoint(PsGame *game, int pointId) {
    string filenamefile = game->ssFolder + "filename.txt";
    string filenamefileX = game->ssFolder + "filename.txt.res";
    string filenamepoint = game->ssFolder + "filename."+to_string(pointId)+".txt.res";
    string lastCDpoint = game->ssFolder + "lastcdimg.txt";
    string lastCDpointX = game->ssFolder + "lastcdimg."+to_string(pointId)+".txt";
    if (Util::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            // last line is our filename
            string ssfile = game->ssFolder + "sstates/" + line + ".00" + to_string(pointId) + ".res";
            string newName = game->ssFolder + "sstates/" + line + ".000";

            remove(ssfile.c_str());
            Util::copy(newName.c_str(), ssfile.c_str());
            remove(newName.c_str());

            // update image

            is.close();
        }
        remove(filenamefileX.c_str());
        remove(filenamepoint.c_str());
        rename(filenamefile.c_str(), filenamefileX.c_str());
        Util::copy(filenamefileX,filenamepoint);
        if (Util::exists(lastCDpoint))
        {
            remove(lastCDpointX.c_str());
            Util::copy(lastCDpoint, lastCDpointX);
            remove(lastCDpoint.c_str());
        }


    }
}

void PcsxInterceptor::prepareResumePoint(PsGame *game, int pointId) {

    // cleanup after previous crash as pcsx doest not want to save
    string filenameTrash = game->ssFolder + "filename.txt";
    if (Util::exists(filenameTrash)) {
        remove(filenameTrash.c_str());
    }

    string ssfile = game->ssFolder + "sstates";
    for (DirEntry sstate:Util::diru(ssfile)) {
        if (Util::getFileExtension(sstate.name) == "000") {
            string toDelete = ssfile + "/" + sstate.name;
            remove(toDelete.c_str());
        }
    }

    ssfile = game->ssFolder + "screenshots";
    for (DirEntry sstate:Util::diru(ssfile)) {
        if (Util::getFileExtension(sstate.name) == "png") {
            string toDelete = ssfile + "/" + sstate.name;
            remove(toDelete.c_str());
        }
    }

    if (pointId == -1)
        return;
    string filenamefile = game->ssFolder + "filename.txt.res";
    string filenamefileX = game->ssFolder + "filename.txt";
    string filenamepoint = game->ssFolder + "filename."+to_string(pointId)+".txt.res";
    remove(filenamefileX.c_str());
    if (Util::exists(filenamepoint))
    {
        filenamefileX = filenamepoint;
    }
    if (Util::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            // last line is our filename
            string ssfile = game->ssFolder + "sstates/" + line + ".00" + to_string(pointId) + ".res";
            string newName = game->ssFolder + "sstates/" + line + ".000";
            if (Util::exists(ssfile)) {
                remove(newName.c_str());
                Util::copy(ssfile.c_str(), newName.c_str());
            }
            is.close();
        }
    }
}




