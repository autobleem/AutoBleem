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
    USBGames gamesInChildrenDirs;
    USBGames gamesToDisplay;

    GameSubDir(const std::string & _path, int _displayRowIndex, int _displayIndentLevel,
               GameSubDirRows *displayRows);

    void makeGamesToDisplayWhileRemovingChildDuplicates();

    void print(bool plusGames);

    static GameSubDirRows scanGamesHierarchy(const std::string & _path);
    static USBGames getAllGames(const GameSubDirRows &rows);

private:
    void scanAll();

    // remove the games in our copy of the games in the child dir that are duplicates of games in this row
    // so he game won't show up twice when viewing this row's carousel
    void removeChildGamesThatAreDuplicatesOfGamesInThisRow();
};

void operator += (USBGames &dest, const USBGames &src);
