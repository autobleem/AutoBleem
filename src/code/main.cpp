/* 
 * File:   main.cpp
 * Author: screemer
 *
 * Created on 11 Dec 2018, 20:37
 */

#include <iostream>
#include "database.h"
#include "scanner.h"
#include "guirender.h"


using namespace std;

#include "memcard.h"


int scanGames( char *argv[], Scanner * scanner) {
    Database *db = new Database();
    if (!db->connect(argv[1])) {
        delete db;
        return EXIT_FAILURE;
    }
    if (!db->createInitialDatabase()) {
        cout << "Error creating db structure" << endl;
        db->disconnect();
        delete db;
        return EXIT_FAILURE;
    };


    scanner->scanDirectory(argv[2]);
    scanner->updateDB(db);


    db->disconnect();
    delete db;
    return (EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "USAGE: bleemsync /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }



    Scanner *scanner = new Scanner();
    Database *db = new Database();
    if (!db->connect(argv[1])) {
        delete db;
        return EXIT_FAILURE;
    }

    string path = argv[2];

    Memcard *memcardOperation = new Memcard(path);
    memcardOperation->restoreAll(path+Util::separator()+"!SaveStates");
    delete memcardOperation;

    if (scanner->isFirstRun(path,db))
    {
        scanner->forceScan = true;
    }
    db->disconnect();
    delete db;


    shared_ptr<GuiRender> gui(GuiRender::getInstance());
    gui->display(scanner->forceScan);
    gui->drawText("AutoBleem");
    gui->menuSelection();
    gui->saveSelection();
    if (gui->menuOption==MENU_OPTION_SCAN)
    {
        scanGames( argv,scanner);
    }


    gui->logText("Loading ... Please Wait ...");
    gui->finish();

    SDL_Quit();

    delete scanner;
    return 0;
}


