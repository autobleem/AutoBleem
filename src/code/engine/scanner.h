//
// Created by screemer on 2018-12-15.
//

#ifndef CBLEEMSYNC_SCANNER_H
#define CBLEEMSYNC_SCANNER_H

#include "../main.h"
#include "game.h"
#include "database.h"
#include "../gui/gui.h"

class Scanner {
public:
    Scanner() {}
    std::vector<std::shared_ptr<Game>> games;

    void scanDirectory(std::string path);
    void repairBrokenCueFiles(std::string path);
    bool isFirstRun(std::string path, Database * db);
    void unecm(const std::string & path); // this routine removes Error Correction files from the bin file to save space
    void updateDB(Database *db);
    bool forceScan=false;
    bool noGamesFound=false;
    void detectAndSortGamefiles(std::string path);
    Scanner(Scanner const &) = delete;
    Scanner &operator=(Scanner const &) = delete;
    static std::shared_ptr<Scanner> getInstance() {
        static std::shared_ptr<Scanner> s{new Scanner};
        return s;
    }

private:

    int getImageType(std::string path);
    bool complete;
    void moveFolderIfNeeded(DirEntry entry, std::string gameDataPath, std::string path);
};


#endif //CBLEEMSYNC_SCANNER_H
