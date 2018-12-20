//
// Created by screemer on 2018-12-15.
//

#ifndef CBLEEMSYNC_SCANNER_H
#define CBLEEMSYNC_SCANNER_H

#include "main.h"
#include "game.h"
#include "database.h"

using namespace std;

class Scanner {
public:
    vector<Game> games;

    void scanDirectory(string path);

    void unecm(string path);
    void updateDB(Database *db);

private:
    bool complete;
    void moveFolderIfNeeded(DirEntry entry, string gameDataPath, string path);
};


#endif //CBLEEMSYNC_SCANNER_H
