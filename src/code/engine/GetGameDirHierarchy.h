#pragma once

#include <string>
#include <vector>
#include <memory>
#include "game.h"

using GameSubDirPtr = std::shared_ptr<class GameSubDir>;
using GameSubDirRows = std::vector<GameSubDirPtr>;

struct GameSubDir {
    std::string fullPath = "";
    std::string subDirName = "";    // last part from the path
    GameSubDirRows *displayRows;
    unsigned int displayRowIndex = 0;
    unsigned int displayIndentLevel = 0;

    std::vector<GameSubDirPtr> childrenDirs;
    USBGames gamesInThisDir;
    USBGames allGames;

    GameSubDir(const std::string & _path, GameSubDirRows *displayRows);

    void scanAll();
    void appendGames(const USBGames &src, USBGames *dest);
    void print(bool plusGames);

    static GameSubDirRows scanGamesHierarchy(const std::string & _path);
};