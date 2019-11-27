//
// Created by screemer on 2019-01-15.
//

#pragma once

#include "config.h"

//******************
// CfgProcessor
//******************
class CfgProcessor {
    void replaceProperty(std::string fullCfgFilePath, std::string property, std::string newline);
    std::string getValueFromCfgFile(std::string fullCfgFilePath, std::string property);

    // example pathToCfgDir = "/media/Games/!SaveStates/12/cfg"
    // example pathToCfgDir = "/media/Games/!SaveStates/Driver 2/cfg"
    void replacePropertyInAllCfgsInDir(std::string pathToCfgDir, std::string property, std::string newline);

public:
    // example gamePath = "/media/Games/!SaveStates/7"
    // example gamePath = "/media/Games/!SaveStates/Driver 2" or
    // example gamePath = "/media/Games/Racing/Driver 2"
    std::string getValue(std::string gamePath, std::string property);

    // example gamePathInSaveStates = "/media/Games/!SaveStates/12"
    void replaceInternal(std::string gamePathInSaveStates, std::string property, std::string newline);

    // example entry = "Driver 2"
    // example gamePath = "/media/Games/Racing"
    void replaceUSB(std::string entry, std::string gamePath, std::string property, std::string newline);

    void replace(std::string entry, std::string gamePath, std::string property, std::string newline, bool internal);

    void replaceRaConf(std::string fullCfgFilePath, std::string property, std::string newline);
};
