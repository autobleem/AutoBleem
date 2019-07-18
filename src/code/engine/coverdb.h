//
// Created by screemer on 2/2/19.
//
#pragma once

#include "database.h"

//******************
// Coverdb
//******************
class Coverdb {
public:
    Database *covers[3];
    std::string regionStr[3];

    bool isValid();
    Coverdb();
    ~Coverdb();

};

