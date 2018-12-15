//
// Created by screemer on 2018-12-15.
//

#ifndef CBLEEMSYNC_SCANNER_H
#define CBLEEMSYNC_SCANNER_H

#include <string>
#include <vector>
#include "game.h"
#include "database.h"

#include "util.h"

using namespace std;

class Scanner {
public:
    vector<Game> games;

    void scanDirectory(string path);

    void updateDB(Database *db);

private:
    bool complete;
};


#endif //CBLEEMSYNC_SCANNER_H
