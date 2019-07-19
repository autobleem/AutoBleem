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

//******************
// Scanner
//******************
class Scanner {
public:
    Scanner() {}
    USBGames games;

    void scanDirectory(const std::string & path);
    void repairBrokenCueFiles(const std::string & path);
    bool isFirstRun(const std::string & path, Database * db);
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
    static bool sortByTitle(const USBGamePtr i, const USBGamePtr j) { return SortByCaseInsensitive(i->title, j->title); }

private:
    ImageType getImageType(std::string path);
    bool complete;
    void moveFolderIfNeeded(const DirEntry & entry, std::string gameDataPath, std::string path);
};
