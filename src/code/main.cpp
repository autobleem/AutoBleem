/* 
 * File:   main.cpp
 * Author: screemer
 *
 * Created on 11 Dec 2018, 20:37
 */

#include <iostream>
#include "database.h"
#include "scanner.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "USAGE: bleemsync /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }


    Database *db = new Database();
    if (!db->connect(argv[1]))
    {
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

