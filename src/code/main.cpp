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

    scanner->detectAndSortGamefiles(path);
    scanner->scanDirectory(path);
    scanner->updateDB(db);


    db->disconnect();
    shared_ptr <Gui> gui(Gui::getInstance());
    gui->drawText(_("Total:")+" " + to_string(scanner->games.size()) + " "+_("games scanned")+".");
    sleep(1);
    delete db;
    for (Game * game:scanner->games)
    {
        delete(game);

    }
    scanner->games.clear();
    return (EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    shared_ptr <Lang> lang(Lang::getInstance());
    if (argc < 3) {
        cout << "USAGE: bleemsync /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }
    shared_ptr <Gui> gui(Gui::getInstance());
    shared_ptr <Scanner> scanner(Scanner::getInstance());

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
    SDL_Quit();
    delete coverdb;


   // lang->dump("output.txt");
    return 0;
}


