/* 
 * File:   main.cpp
 * Author: screemer
 *
 * Created on 11 Dec 2018, 20:37
 */

#include <iostream>
#include "database.h"
#include "scanner.h"
#include "splash.h"


using namespace std;

int scanGames(int argc, char *argv[]) {

    if (argc < 3) {
        cout << "USAGE: bleemsync /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }

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

    Scanner *scanner = new Scanner();
    scanner->scanDirectory(argv[2]);
    scanner->updateDB(db);

    delete scanner;
    db->disconnect();
    delete db;
    return (EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    shared_ptr<Splash> splash(Splash::getInstance());
    splash->display();
    splash->drawText("AutoBleem v0.2 Welcome");
    int result = scanGames(argc, argv);
    splash->logText("Loading Playstation Classic UI");
    splash->finish();
#ifndef NO_GUI
    SDL_Quit();
#endif

    return result;
}


