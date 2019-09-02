//
// Created by screemer on 2/2/19.
//

#include "coverdb.h"
#include "../util.h"
#include <iostream>
#include "../DirEntry.h"
#include "../environment.h"

using namespace std;

//*******************************
// Coverdb::Coverdb()
//*******************************
Coverdb::Coverdb()
{
    regionStr[0] = "U";
    regionStr[1] = "P";
    regionStr[2] = "J";

    for (int i=0;i<3;i++)
    {
        covers[i]= nullptr;
        auto filename = Env::getPathToCoversDBDir() + sep + "covers" + regionStr[i] + ".db";
        if (DirEntry::exists(filename)) {
                covers[i] = new Database();
                bool success = covers[i]->connect(filename);
                if (!success) {
                    cout << "failed to open database " << filename << endl;
                }
        }
        else {
            cout << "database file " << filename << " not found" << endl;
        }
    }
}

//*******************************
// Coverdb::~Coverdb()
//*******************************
Coverdb::~Coverdb()
{
    for (int i=0;i<3;i++)
    {
        if (covers[i]!= nullptr)
        {
            covers[i]->disconnect();
            delete covers[i];
        }
    }
}

//*******************************
// Coverdb::isValid
//*******************************
bool Coverdb::isValid()
{
    bool valid = false;
    for (Database *db:covers)
    {
        if (db!= nullptr) valid = true;
    }
    return valid;
}