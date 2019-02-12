//
// Created by screemer on 2019-01-27.
//

#ifndef AUTOBLEEM_GUI_VER_MIGRATION_H
#define AUTOBLEEM_GUI_VER_MIGRATION_H

#include <string>
#include "engine/database.h"


using namespace std;

class VerMigration {
public:
    string getLastRunVersion();
    void migrate04_05(Database * db);
    void migrate(Database * db);
};


#endif //AUTOBLEEM_GUI_VER_MIGRATION_H
