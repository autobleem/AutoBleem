//
// Created by screemer on 2019-01-27.
//

#pragma once

#include <string>
#include "engine/database.h"

//******************
// VerMigration
//******************
class VerMigration {
public:
    std::string getLastRunVersion();
    void migrate04_05(Database * db);
    void migrate(Database * db);
};
