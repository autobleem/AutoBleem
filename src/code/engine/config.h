//
// Created by screemer on 23.12.18.
//
#pragma once

#include <autobleem/autobleemui.h>

//******************
// Config
//******************
class Config {
public:
    Inifile inifile;
    Config();
    void save();
};
