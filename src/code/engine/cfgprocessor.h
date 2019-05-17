//
// Created by screemer on 2019-01-15.
//

#pragma once

#include "config.h"

//******************
// CfgProcessor
//******************
class CfgProcessor {
public:
    Config config;
    void replace(std::string entry, std::string gamePath,  std::string property, std::string newline, bool internal);
    void patchHLEbios(std::string entry, std::string gamePath);
    std::string getValue(std::string entry, std::string gamePath, std::string property,bool internal);

private:
    void replaceInternal(std::string filePath, std::string property, std::string newline);
};
