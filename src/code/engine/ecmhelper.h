//
// Created by screemer on 2018-12-16.
//
#pragma once

#include "../main.h"

// ECM files are Error Code Modeler files used in disc images.
// https://www.lifewire.com/ecm-file-2620956
// this class removes them from the bin files to save space.

//******************
// Ecmhelper
//******************
class Ecmhelper {
public:
    bool unecm(std::string input, std::string output);
};
