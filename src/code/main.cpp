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

using namespace std;

#include "engine/memcard.h"
#include "lang.h"
#include "launcher/pcsx_interceptor.h"


int scanGames(string path, string dbpath) {
    shared_ptr<Gui> gui(Gui::getInstance());
    shared_ptr<Scanner> scanner(Scanner::getInstance());

    if (!gui->db->createInitialDatabase()) {
        cout << "Error creating db structure" << endl;

        return EXIT_FAILURE;
    };

    scanner->detectAndSortGamefiles(path);
    scanner->scanDirectory(path);
    scanner->updateDB(gui->db);


    gui->drawText(_("Total:") + " " + to_string(scanner->games.size()) + " " + _("games scanned") + ".");
    sleep(1);
    for (Game *game:scanner->games) {
        delete (game);

    }
    scanner->games.clear();
    return (EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    shared_ptr<Lang> lang(Lang::getInstance());
    if (argc < 3) {
        cout << "USAGE: bleemsync /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }
    shared_ptr<Gui> gui(Gui::getInstance());
    shared_ptr<Scanner> scanner(Scanner::getInstance());

    lang->load(gui->cfg.inifile.values["language"]);

    Coverdb *coverdb = new Coverdb();
    gui->coverdb = coverdb;


    Database *db = new Database();
    if (!db->connect(argv[1])) {
        delete db;
        return EXIT_FAILURE;
    }

    string dbpath = argv[1];
    string path = argv[2];

    Memcard *memcardOperation = new Memcard(path);
    memcardOperation->restoreAll(path + Util::separator() + "!SaveStates");
    delete memcardOperation;

    if (scanner->isFirstRun(path, db)) {
        scanner->forceScan = true;

    }


    gui->display(scanner->forceScan, path, db, false);

    while (gui->menuOption == MENU_OPTION_SCAN || gui->menuOption == MENU_OPTION_START) {

        gui->menuSelection();
        gui->saveSelection();
        if (gui->menuOption == MENU_OPTION_SCAN) {
            scanGames(path, dbpath);
            if (gui->forceScan) {
                gui->forceScan = false;
            } else {
                //break;
            }

        }

        if (gui->menuOption == MENU_OPTION_START) {
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
            cout << numtimesopened << endl;

            for (SDL_Joystick* joy:gui->joysticks) {
                if (SDL_JoystickGetAttached(joy)) {
                    SDL_JoystickClose(joy);
                }
            }
            SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
            sleep(1);

            gui->saveSelection();
            PcsxInterceptor *interceptor = new PcsxInterceptor();
            interceptor->memcardIn(gui->runningGame);
            interceptor->prepareResumePoint(gui->runningGame, gui->resumepoint);
            interceptor->execute(gui->runningGame, gui->resumepoint );
            interceptor->memcardOut(gui->runningGame);
            delete (interceptor);

            SDL_InitSubSystem(SDL_INIT_JOYSTICK);

            delete gui->runningGame;
            sync();
            gui->runningGame = nullptr;
            gui->startingGame = false;
            gui->display(false, path, db, true);
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


