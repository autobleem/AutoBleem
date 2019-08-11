//
// Created by screemer on 2018-12-15.
//
#pragma once

#include "../main.h"
#include "game.h"
#include "database.h"
#include "../gui/gui.h"
#include "../util.h"
#include <map>
#include "../DirEntry.h"
#include <algorithm>
#include "GetGameDirHierarchy.h"

//******************
// Scanner
//******************
class Scanner {
public:
    Scanner() {}
    USBGames gamesToAddToDB;
    bool forceScan=false;
    bool noGamesFound=false;

    void scanUSBGamesDirectory(const std::string & path, GamesHierarchy &gamesHierarchy);
    void repairBrokenCueFiles(const std::string & path);

    void unecm(const std::string & path); // this routine removes Error Correction files from the bin file to save space
    void updateDB(Database *db);

    static bool areThereGameFilesInDir(const std::string & path);

    Scanner(Scanner const &) = delete;
    Scanner &operator=(Scanner const &) = delete;

    static std::shared_ptr<Scanner> getInstance() {
        static std::shared_ptr<Scanner> s{new Scanner};
        return s;
    }

private:
    bool complete;
    void moveFolderIfNeeded(const std::string &gameDirName, std::string gameDataPath, std::string path);
};
