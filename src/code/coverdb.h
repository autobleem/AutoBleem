//
// Created by screemer on 2/2/19.
//

#ifndef AUTOBLEEM_GUI_COVERDB_H
#define AUTOBLEEM_GUI_COVERDB_H

#include "database.h"

class Coverdb {
public:
    Database *covers[3];
    string regionStr[3];

    Coverdb();
    ~Coverdb();

};


#endif //AUTOBLEEM_GUI_COVERDB_H
