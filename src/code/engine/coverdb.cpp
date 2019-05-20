//
// Created by screemer on 2/2/19.
//

#include "coverdb.h"

#include "../util.h"

static const char *jDatabases[] = {"../db/coversU.db", "../db/coversP.db", "../db/coversJ.db"};

//*******************************
// Coverdb::Coverdb()
//*******************************
Coverdb::Coverdb()
{
    for (int i=0;i<3;i++)
    {
        covers[i]= nullptr;
        if (Util::exists(jDatabases[i]))
        {
                covers[i] = new Database();
                covers[i]->connect(jDatabases[i]);

        }
    }
    regionStr[0] = "U";
    regionStr[1] = "P";
    regionStr[2] = "J";
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