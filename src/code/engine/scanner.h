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

//******************
// Scanner
//******************
class Scanner {
public:
    Scanner() {}
    USBGames games;

    void scanUSBGamesDirectory(const std::string & path);
    void repairBrokenCueFiles(const std::string & path);
    bool gamesDoNotMatchAutobleemprev(const USBGames &allGames, const std::string & autobleemPrevPath);
    void unecm(const std::string & path); // this routine removes Error Correction files from the bin file to save space
    void updateDB(Database *db);
    bool forceScan=false;
    bool noGamesFound=false;
    static bool areThereGameFilesInDir(const std::string & path);
    static bool copyGameFilesInGamesDirToSubDirs(const std::string & path);    // returns true is any files moved into sub-dirs

    Scanner(Scanner const &) = delete;
    Scanner &operator=(Scanner const &) = delete;

    static std::shared_ptr<Scanner> getInstance() {
        static std::shared_ptr<Scanner> s{new Scanner};
        return s;
    }

    static void sortByTitle(USBGames &games) { std::sort(games.begin(), games.end(),
            [] (const USBGamePtr &g1, const USBGamePtr &g2) { return SortByCaseInsensitive(g1->title, g2->title); }); }
    static void sortByFullPath(USBGames &games) { std::sort(begin(games), end(games),
            [] (const USBGamePtr &g1, const USBGamePtr &g2) { return SortByCaseInsensitive(g1->fullPath, g2->fullPath); }); }
    static void sortBySerial(USBGames &games) { std::sort(begin(games), end(games),
            [] (const USBGamePtr &g1, const USBGamePtr &g2) { return SortByCaseInsensitive(g1->serial, g2->serial); }); }

private:
    bool complete;
    void moveFolderIfNeeded(const std::string &gameDirName, std::string gameDataPath, std::string path);
};
