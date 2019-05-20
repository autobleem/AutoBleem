//
// Created by screemer on 23.12.18.
//
#pragma once

#include "../main.h"
#include "inifile.h"

//******************
// Config
//******************
class Config {
public:
    Inifile inifile;
    Config();
    void save();
};
