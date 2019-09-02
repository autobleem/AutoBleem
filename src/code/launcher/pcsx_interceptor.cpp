//
// Created by screemer on 2/13/19.
//

#include <sys/wait.h>
#include "pcsx_interceptor.h"
#include "../util.h"
#include "../gui/gui.h"
#include "../lang.h"
#include "../engine/memcard.h"
#include "../engine/cfgprocessor.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include "../Environment.h"

using namespace std;

void PcsxInterceptor::cleanupConfig(PsGamePtr &game)
{
    // copy back config to its place
    auto processor = new CfgProcessor();
    string newConfig = game->ssFolder + sep + "autobleem.cfg";
    if (DirEntry::exists(newConfig)) {
        // fix bios
        processor->replaceRaConf(newConfig,"Bios","Bios = SET_BY_PCSX");



        if (!game->internal) {
            DirEntry::copy(newConfig, game->ssFolder + sep + PCSX_CFG);
            DirEntry::copy(newConfig, game->folder + sep + PCSX_CFG);
        } else {
            DirEntry::copy(newConfig, game->ssFolder + sep + PCSX_CFG);
        }
        remove(newConfig.c_str());
    }
    delete processor;
}
//*******************************
// PcsxInterceptor::execute
//*******************************
bool PcsxInterceptor::execute(PsGamePtr & game, int resumepoint) {
    shared_ptr<Gui> gui(Gui::getInstance());

    string padMapping = gui->padMapping;

    string lastCDpoint = game->ssFolder + sep + "lastcdimg.txt";
    string lastCDpointX = game->ssFolder + sep + "lastcdimg." + to_string(resumepoint)+".txt";
    gui->saveSelection();
    std::vector<const char *> argvNew;
    string gameFile = "";

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
    game->ssFolder = DirEntry::removeSeparatorFromEndOfPath(game->ssFolder);

    argvNew.push_back(link.c_str());
    argvNew.push_back(game->ssFolder.c_str());

    remove (lastCDpoint.c_str());

    if (DirEntry::exists(lastCDpointX))
    {
        DirEntry::copy(lastCDpointX,lastCDpoint);
        ifstream is(lastCDpointX.c_str());
        if (is.is_open()) {
            std::string line;
            std::getline(is, line);

            // last line is our filename
            gameFile = line;
            is.close();
        }
    } else {
        gameFile += (game->folder + sep + game->base);
        if (!DirEntry::matchExtension(game->base, ".pbp")) {
            gameFile += ".cue";
        }
    }

    gameFile += "";

    argvNew.push_back(gameFile.c_str());
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

    if (padMapping.empty())
    {
        argvNew.push_back("NA");
    } else
    {
        argvNew.push_back(padMapping.c_str());
    }

    argvNew.push_back(nullptr);

    for (const char *s:argvNew) {
        if (s != nullptr) {
            cout << "'" << s << "' ";
        }
    }
    cout << endl;

    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }

    waitpid(pid, NULL, 0);
    cleanupConfig(game);

    usleep(3 * 1000);
    return true;
}

//*******************************
// PcsxInterceptor::memcardIn
//*******************************
void PcsxInterceptor::memcardIn(PsGamePtr & game) {
    string memcard = "SONY";
    if (!game->internal) {
        Inifile gameini;
        gameini.load(game->folder + sep + "Game.ini");
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
}

//*******************************
// PcsxInterceptor::memcardOut
//*******************************
void PcsxInterceptor::memcardOut(PsGamePtr & game) {
    string memcard = "SONY";
    if (!game->internal) {
        Inifile gameini;
        gameini.load(game->folder + sep + "Game.ini");
        memcard = gameini.values["memcard"];
    }
    if (memcard != "SONY") {
        Memcard *card = new Memcard(Env::getPathToGamesDir() + sep);
        card->swapOut(game->ssFolder, game->memcard);
        delete card;
    }
}

//*******************************
// PcsxInterceptor::saveResumePoint
//*******************************
void PcsxInterceptor::saveResumePoint(PsGamePtr & game, int pointId) {
    string filenamefile = game->ssFolder + sep + "filename.txt";
    string filenamefileX = game->ssFolder + sep + "filename.txt.res";
    string filenamepoint = game->ssFolder + sep + "filename."+to_string(pointId)+".txt.res";
    string lastCDpoint = game->ssFolder + sep + "lastcdimg.txt";
    string lastCDpointX = game->ssFolder + sep + "lastcdimg."+to_string(pointId)+".txt";
    if (DirEntry::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            // last line is our filename
            string ssfile = game->ssFolder + sep + "sstates/" + line + ".00" + to_string(pointId) + ".res";
            string newName = game->ssFolder + sep + "sstates/" + line + ".000";

            remove(ssfile.c_str());
            DirEntry::copy(newName.c_str(), ssfile.c_str());
            remove(newName.c_str());

            // update image

            is.close();
        }
        remove(filenamefileX.c_str());
        remove(filenamepoint.c_str());
        rename(filenamefile.c_str(), filenamefileX.c_str());
        DirEntry::copy(filenamefileX,filenamepoint);
        if (DirEntry::exists(lastCDpoint))
        {
            remove(lastCDpointX.c_str());
            DirEntry::copy(lastCDpoint, lastCDpointX);
            remove(lastCDpoint.c_str());
        }
    }
}

//*******************************
// PcsxInterceptor::prepareResumePoint
//*******************************
void PcsxInterceptor::prepareResumePoint(PsGamePtr & game, int pointId) {

    // cleanup after previous crash as pcsx doest not want to save
    string filenameTrash = game->ssFolder + sep + "filename.txt";
    if (DirEntry::exists(filenameTrash)) {
        remove(filenameTrash.c_str());
    }

    string ssfile = game->ssFolder + "sstates";
    for (const DirEntry & sstate:DirEntry::diru(ssfile)) {
        if (DirEntry::getFileExtension(sstate.name) == "000") {
            string toDelete = ssfile + sep + sstate.name;
            remove(toDelete.c_str());
        }
    }

    ssfile = game->ssFolder + "screenshots";
    for (const DirEntry & sstate:DirEntry::diru(ssfile)) {
        if (DirEntry::getFileExtension(sstate.name) == "png") {
            string toDelete = ssfile + sep + sstate.name;
            remove(toDelete.c_str());
        }
    }

    if (pointId == -1)
        return;
    string filenamefile = game->ssFolder + sep + "filename.txt.res";
    string filenamefileX = game->ssFolder + sep + "filename.txt";
    string filenamepoint = game->ssFolder + sep + "filename."+to_string(pointId)+".txt.res";
    remove(filenamefileX.c_str());
    if (DirEntry::exists(filenamepoint))
    {
        filenamefile = filenamepoint;
    }
    if (DirEntry::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            string lastImageInfo = line;

            // fix lastcdpoint
            string lastCDpointX = game->ssFolder + sep + "lastcdimg."+to_string(pointId)+".txt";
            remove(lastCDpointX.c_str());
            string file = DirEntry::getFileNameFromPath(lastImageInfo);
            string imageToLoad = game->folder + sep + file;

            ofstream os;
            os.open(lastCDpointX);
            os << imageToLoad << endl;
            os.close();

            std::getline(is, line);

            // last line is our filename
            string ssfile = game->ssFolder + sep + "sstates/" + line + ".00" + to_string(pointId) + ".res";
            string newName = game->ssFolder + sep + "sstates/" + line + ".000";
            if (DirEntry::exists(ssfile)) {
                remove(newName.c_str());
                DirEntry::copy(ssfile.c_str(), newName.c_str());
            }
            is.close();
        }
    }
}
