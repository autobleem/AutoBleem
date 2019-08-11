#pragma once

#include <string>
#include <vector>
#include <memory>
#include "game.h"

using GameSubDirPtr = std::shared_ptr<class GameSubDir>;
using GameSubDirRows = std::vector<GameSubDirPtr>;

//******************
// GameSubDir
//******************
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

private:
    void scanAll();

    // remove the games in our copy of the games in the child dir that are duplicates of games in this row
    // so he game won't show up twice when viewing this row's carousel
    void removeChildGamesThatAreDuplicatesOfGamesInThisRow();
};

void operator += (USBGames &dest, const USBGames &src);

//******************
// GamesHierarchy
//******************
struct GamesHierarchy {
    GameSubDirRows gameSubDirRows;  // these rows are displayed in the select game dir menu

    GamesHierarchy(const std::string & _path);
    USBGames getAllGames();

    bool gamesDoNotMatchAutobleemPrev(const std::string & autobleemPrevPath);
    void writeAutobleemPrev(const std::string & autobleemPrevPath);

    // run this after Scanner has filled in the serial so we can correctly match duplicate games
    void makeGamesToDisplayWhileRemovingChildDuplicates();

    void printGamesInEachRow();
    void printGamesToDisplayInEachRow();
};