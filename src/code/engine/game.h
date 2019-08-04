//
// Created by screemer on 2018-12-15.
//
#pragma once

#include "../main.h"
#include <vector>
#include <memory>
#include <vector>
#include <map>

//******************
// Disc
//******************
class Disc {
public:
    std::string diskName;
    std::string cueName;
    // verifications

    bool cueFound = false;
    bool binVerified = false;
};

//******************
// USBGame
//******************
class USBGame {
public:
    int folder_id = 0;
    std::string fullPath;       // "/Games/Sports/Football/NFL Blitz"
    std::string gameDirName;    // "NFL Blitz"
    std::string saveStatePath;

    std::string title;
    std::string publisher;
    int year = 0;
    std::string serial;
    std::string region;
    int players = 0;
    std::vector<Disc> discs;
    std::string favorite;

    std::string memcard;

    bool gameDataFound = false;
    bool pcsxCfgFound = false;
    bool gameIniFound = false;
    bool gameIniValid = false;
    bool coverImageFound = false;
    bool licFound = false;
    bool automationUsed = false;
    ImageType imageType = IMAGE_BIN;
    bool highRes = false;
    std::string firstBinPath;

    void readIni(std::string path);
    void saveIni(std::string path);
    void recoverMissingFiles();
    bool verify();
    bool print();
    void updateObj();
    bool validateCue(std::string cuePath, std::string path);

    std::map<std::string, std::string> iniValues;

private:
    void parseIni(std::string path);
    std::string valueOrDefault(std::string name, std::string def, bool setAutomationIfDefaultUsed = true);
};

using USBGamePtr = std::shared_ptr<USBGame>;
using USBGames = std::vector<USBGamePtr>;
