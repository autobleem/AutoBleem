/* 
 * File:   main.cpp
 * Author: screemer
 *
 * Created on 11 Dec 2018, 20:37
 */

#include <iostream>
#include "engine/database.h"
#include "engine/scanner.h"
#include "gui/gui.h"
#include "main.h"
#include "ver_migration.h"
#include "engine/coverdb.h"
#include "util.h"
#include <unistd.h>
#include "engine/GetGameDirHierarchy.h"

using namespace std;

#include "engine/memcard.h"
#include "lang.h"
#include "launcher/emu_interceptor.h"
#include "launcher/pcsx_interceptor.h"
#include "launcher/retboot_interceptor.h"
#include "engine/GetGameDirHierarchy.h"

Database * db;

//*******************************
// copyGameFilesInGamesDirToSubDirs
//*******************************
// Search for games with supported extension and move to sub-dir
// returns true is any files moved into sub-dirs
bool copyGameFilesInGamesDirToSubDirs(const string & path){
    bool ret = false;
    string fileExt;
    string filenameWE;
    vector<string> extensions;
    vector<string> binList;
    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText(_("Moving..."));
//    extensions.push_back("iso");
    extensions.push_back("pbp");
    extensions.push_back("cue");

    //Getting all files in USBGames Dir
    DirEntries globalFileList = DirEntry::diru(path);
    DirEntries fileList = DirEntry::getFilesWithExtension(path, globalFileList, extensions);

    //On first run, we won't process bin/img files, as cue file may handle a part of them
    for (const auto &entry : fileList){
        splash->logText(_("Moving :") + " " + entry.name);
        fileExt = DirEntry::getFileExtension(entry.name);
        filenameWE = DirEntry::getFileNameWithoutExtension(entry.name);
        //Checking if file exists
        if(access((path + sep + entry.name).c_str(),F_OK) != -1){
            if(fileExt == "cue"){
                binList = DirEntry::cueToBinList(path + sep + entry.name);
                if(!binList.empty()){
                    //Create directory for game
                    DirEntry::createDir(path + sep + filenameWE);
                    //Move cue file
                    rename((path + "/" + entry.name).c_str(), (path + sep + filenameWE + "/" + entry.name).c_str());
                    //Move bin files
                    for (const auto &bin : binList){
                        splash->logText(_("Moving :") + " " + bin);
                        rename((path + sep + bin).c_str(), (path + sep + filenameWE + sep + bin).c_str());
                    }
                    ret = true;
                }
            }else{
                DirEntry::createDir(path + sep + filenameWE);

                rename((path + sep + entry.name).c_str(),(path + sep + filenameWE + sep + entry.name).c_str());
                ret = true;
            }
        }
    }

    //Next we will read only bin and img files
    extensions.clear();
    extensions.push_back("img");
    extensions.push_back("bin");
    fileList = DirEntry::getFilesWithExtension(path, globalFileList, extensions);
    for (const auto &entry : fileList){
        splash->logText(_("Moving :") + " " + entry.name);
        fileExt = DirEntry::getFileExtension(entry.name);
        filenameWE = DirEntry::getFileNameWithoutExtension(entry.name);
        //Checking if file exists
        if(access((path + sep + entry.name).c_str(),F_OK) != -1){
            DirEntry::createDir(path + sep + filenameWE);
            rename((path + sep + entry.name).c_str(), (path + sep + filenameWE + sep + entry.name).c_str());
            ret = true;
        }
    }
    return ret; // true if any game files moved into a sub-dir
}

//*******************************
// scanGames
//*******************************
int scanGames(string rootPath, GamesHierarchy &gamesHierarchy, string dbpath) {
    shared_ptr<Gui> gui(Gui::getInstance());
    shared_ptr<Scanner> scanner(Scanner::getInstance());

    if (!db->createInitialDatabase()) {
        cout << "Error creating db structure" << endl;

        return EXIT_FAILURE;
    };

    if (!db->truncate())
    {
        gui->drawText("ERROR IN DB");
        sleep(1);
        return EXIT_FAILURE;
    }

    scanner->scanUSBGamesDirectory(rootPath, gamesHierarchy);
    scanner->updateDB(gui->db);

    gui->drawText(_("Total:") + " " + to_string(scanner->gamesToAddToDB.size()) + " " + _("games scanned") + ".");
    sleep(1);
    scanner->gamesToAddToDB.clear();
    return (EXIT_SUCCESS);
}

//*******************************
// main
//*******************************
int main(int argc, char *argv[]) {
    shared_ptr<Lang> lang(Lang::getInstance());
    if (argc < 3) {
        cout << "USAGE: autobleem-gui /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }
    shared_ptr<Gui> gui(Gui::getInstance());
    shared_ptr<Scanner> scanner(Scanner::getInstance());

    lang->load(gui->cfg.inifile.values["language"]);

    Coverdb *coverdb = new Coverdb();
    gui->coverdb = coverdb;

    db = new Database();
    if (!db->connect(argv[1])) {
        delete db;
        return EXIT_FAILURE;
    }
    gui->db=db;
    db->createInitialDatabase();
    db->createFavColumn();

    string dbpath = argv[1];
    string path = argv[2];

    Memcard *memcardOperation = new Memcard(path);
    memcardOperation->restoreAll(path + sep + "!SaveStates");
    delete memcardOperation;

    string prevPath = DirEntry::getWorkingPath() + sep + "autobleem.prev";
    bool prevFileExists = DirEntry::exists(prevPath);

    bool thereAreGameFilesInGamesDir = scanner->areThereGameFilesInDir(path);
    if (thereAreGameFilesInGamesDir)
        copyGameFilesInGamesDirToSubDirs(path);

    GamesHierarchy gamesHierarchy(path);
    USBGames allGames = gamesHierarchy.getAllGames();
    USBGame::sortByFullPath(allGames);

    bool autobleemPrevOutOfDate = gamesHierarchy.gamesDoNotMatchAutobleemPrev(prevPath);
    if (!prevFileExists || thereAreGameFilesInGamesDir || autobleemPrevOutOfDate) {
        scanner->forceScan = true;
    }

    gui->display(scanner->forceScan, path, db, false);

    while (gui->menuOption == MENU_OPTION_SCAN || gui->menuOption == MENU_OPTION_START) {

        gui->menuSelection();
        gui->saveSelection();
        if (gui->menuOption == MENU_OPTION_SCAN) {
            scanGames(path, gamesHierarchy, dbpath);
            gamesHierarchy.writeAutobleemPrev(prevPath);
            if (gui->forceScan) {
                gui->forceScan = false;
            } else {
                //break;
            }
        }

        if (gui->menuOption == MENU_OPTION_START) {
#if defined(__x86_64__) || defined(_M_X64)
            cout << "I'm sorry Dave I'm afraid I can't do that on this system." << endl;

            // just a temp to test exec
            EmuInterceptor *interceptor;
            if (gui->runningGame->foreign)
            {
                interceptor = new RetroArchInterceptor();
            } else {
                if (gui->emuMode == EMU_PCSX) {
                    interceptor = new PcsxInterceptor();
                } else {
                    interceptor = new RetroArchInterceptor();
                }
            }

            interceptor->memcardIn(gui->runningGame);
            interceptor->prepareResumePoint(gui->runningGame, gui->resumepoint);
            interceptor->execute(gui->runningGame, gui->resumepoint );
            interceptor->memcardOut(gui->runningGame);
            delete (interceptor);

            SDL_InitSubSystem(SDL_INIT_JOYSTICK);

            usleep(300*1000);
            gui->runningGame.reset();    // replace with shared_ptr pointing to nullptr
            gui->startingGame = false;

            gui->display(false, path, db, true);

#else
            cout << "Starting game" << endl;
            gui->finish();

            int numtimesopened, frequency, channels;
            Uint16 format;
            numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);
            for (int i=0;i<numtimesopened;i++)
            {
                Mix_CloseAudio();
            }
            numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);

            for (SDL_Joystick* joy:gui->joysticks) {
                if (SDL_JoystickGetAttached(joy)) {
                    SDL_JoystickClose(joy);
                }
            }
            SDL_QuitSubSystem(SDL_INIT_JOYSTICK);

            gui->saveSelection();
            EmuInterceptor *interceptor;
            if (gui->runningGame->foreign)
            {
                interceptor = new RetroArchInterceptor();
            } else {
                if (gui->emuMode == EMU_PCSX) {
                    interceptor = new PcsxInterceptor();
                } else {
                    interceptor = new RetroArchInterceptor();
                }
            }

            interceptor->memcardIn(gui->runningGame);
            interceptor->prepareResumePoint(gui->runningGame, gui->resumepoint);
            interceptor->execute(gui->runningGame, gui->resumepoint );
            interceptor->memcardOut(gui->runningGame);
            delete (interceptor);

            SDL_InitSubSystem(SDL_INIT_JOYSTICK);

            usleep(300*1000);
            gui->runningGame.reset();    // replace with shared_ptr pointing to nullptr
            gui->startingGame = false;

            gui->display(false, path, db, true);
#endif
        }
    }
    db->disconnect();
    delete db;

    gui->logText(_("Loading ... Please Wait ..."));
    gui->finish();
    SDL_Quit();
    delete coverdb;

    exit(0);
}
