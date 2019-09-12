#pragma once

#include <string>
#include <vector>
#include <memory>
#include "game.h"
#include <fstream>

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

    GameSubDir(const std::string & _path, int _displayIndentLevel, GameSubDirRows *displayRows);
    void scanAll();   // recursive scan of the sub directories

    static bool sameGame(const USBGamePtr &game1, const USBGamePtr &game2);
    void makeGamesToDisplayWhileRemovingChildDuplicates(std::ofstream &dupFile);    // recursive

    void print(bool plusGames);

private:

    static void removeGamesInSecondListThatMatchAGameInFirstList(USBGames &games1, USBGames &games2, std::ofstream &dupFile);
    static void removeDuplicateGamesLeavingOne(USBGames &games, std::ofstream &dupFile);
};

//******************
// GamesHierarchy
//******************
struct GamesHierarchy {
    GameSubDirRows gameSubDirRows;  // these rows are displayed in the select game dir menu
    std::ofstream dupFile;

    GamesHierarchy() { }
    void getHierarchy(const std::string & _path);
    USBGames getAllGames();

    bool gamesDoNotMatchAutobleemPrev(const std::string & autobleemPrevPath);
    void writeAutobleemPrev(const std::string & autobleemPrevPath);

    // run this after Scanner has filled in the serial so we can correctly match duplicate games
    void makeGamesToDisplayWhileRemovingChildDuplicates();

    // if a game failed to verify in Scanner it needs to be removed
    void removeGameFromEntireHierarchy(USBGamePtr &game);

    void dumpRowGameInfo(std::ostream &o, bool alsoPrintGames);
    void dumpRowDisplayGameInfo(std::ostream &o, bool alsoPrintGames);
    void printRowGameInfo(bool alsoPrintGames);
    void printRowDisplayGameInfo(bool alsoPrintGames);
};