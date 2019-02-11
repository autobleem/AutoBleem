//
// Created by screemer on 2018-12-15.
//


#ifndef CBLEEMSYNC_GAME_H
#define CBLEEMSYNC_GAME_H

#include "../main.h"
using namespace std;

class Disc {
public:
    string diskName;
    string cueName = "";
    // verifications

    bool cueFound = false;
    bool binVerified = false;
};

class Game {
public:
    int folder_id;
    string fullPath;
    string saveStatePath;
    string pathName;
    string title;
    string publisher;
    string memcard;
    int players;
    int year;
    vector<Disc> discs;

    bool gameDataFound = false;
    bool pcsxCfgFound = false;
    bool gameIniFound = false;
    bool gameIniValid = false;
    bool imageFound = false;
    bool licFound = false;
    bool automationUsed = false;
    int imageType=IMAGE_CUE_BIN;
    bool highRes = false;
    string firstBinPath = "";

    void readIni(string path);
    void saveIni(string path);
    void recoverMissingFiles();
    bool verify();
    bool print();
    void updateObj();
    bool validateCue(string cuePath, string path);
private:


    map<string, string> iniValues;
    void parseIni(string path);
    string valueOrDefault(string name, string def);
};


#endif //CBLEEMSYNC_GAME_H
