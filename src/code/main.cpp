/* 
 * File:   main.cpp
 * Author: screemer
 *
 * Created on 11 Dec 2018, 20:37
 */

#include <iostream>
#include "database.h"
#include "scanner.h"
#include "gui.h"
#include "main.h"
#include "ver_migration.h"
#include "coverdb.h"


using namespace std;

#include "memcard.h"
#include "lang.h"


int scanGames(string path, string dbpath) {
    shared_ptr <Scanner> scanner(Scanner::getInstance());
    Database *db = new Database();
    if (!db->connect(dbpath)) {
        delete db;
        return EXIT_FAILURE;
    }
    if (!db->createInitialDatabase()) {
        cout << "Error creating db structure" << endl;
        db->disconnect();
        delete db;
        return EXIT_FAILURE;
    };


    scanner->scanDirectory(path);
    scanner->updateDB(db);


    db->disconnect();
    shared_ptr <Gui> gui(Gui::getInstance());
    gui->drawText(_("Total:")+" " + to_string(scanner->games.size()) + " "+_("games scanned")+".");
    sleep(1);
    delete db;
    return (EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "USAGE: bleemsync /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }
    shared_ptr <Gui> gui(Gui::getInstance());
    shared_ptr <Scanner> scanner(Scanner::getInstance());

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




    gui->display(scanner->forceScan, path, db);
    db->disconnect();
    delete db;
    while (gui->menuOption == MENU_OPTION_SCAN) {

        gui->menuSelection();
        gui->saveSelection();
        if (gui->menuOption == MENU_OPTION_SCAN) {
            scanGames(path, dbpath);
            if (gui->forceScan) {
                gui->forceScan = false;
            } else {
                break;
            }

        }
    }


    gui->logText(_("Loading ... Please Wait ..."));
    gui->finish();
    sync();
    SDL_Quit();
    delete coverdb;

    shared_ptr <Lang> lang(Lang::getInstance());
    lang->dump("output.txt");
    return 0;
}


