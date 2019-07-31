#include "GetGameDirHierarchy.h"
#include "../util.h"
#include "../DirEntry.h"
#include <iostream>

//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;

using namespace std;

//*******************************
// GameSubDir::GameSubDir
//*******************************
GameSubDir::GameSubDir(const std::string & _fullPath, GameSubDirRows *_displayRows) {
    fullPath = DirEntry::pathWithOutSeparatorAtEnd(_fullPath);
    dirName = DirEntry::getFileNameFromPath(fullPath);
    displayRows = _displayRows;
}

//*******************************
// GameSubDir::scanAll
//*******************************
void GameSubDir::scanAll() {
    DirEntries dirs = DirEntry::diru_DirsOnly_WithFixedCommas(fullPath);
    for (auto & dir : dirs) {
        if (dir.name == "!SaveStates")
            continue;
        if (dir.name == "!MemCards")
            continue;
        string path = fullPath + DirEntry::separator() + dir.name;
        //cout << "path: " << path << endl;
        if (DirEntry::thereIsAGameFile(path)) {
            USBGamePtr game{new USBGame};
            game->fullPath = path;
            game->pathName = dir.name;
            gamesInThisDir.emplace_back(game);
            //cout << "added game: " << game->pathName << endl;
        } else {
            //cout << "subdir: " << path << endl;
            GameSubDirPtr subdir(new GameSubDir(path, displayRows));
            int numChildren = childrenDirs.size();
            subdir->displayRowIndex = displayRowIndex + numChildren + 1;
            subdir->displayIndentLevel = displayIndentLevel + 1;
            subdir->scanAll();
            if (subdir->allGames.size() > 0) {
                childrenDirs.emplace_back(subdir);
                displayRows->emplace_back(subdir);
            }
        }
    }

    appendGames(gamesInThisDir, &allGames);
    for (auto & dir : childrenDirs)
        appendGames(dir->allGames, &allGames);
}

//*******************************
// GameSubDir::appendGames
//*******************************
void GameSubDir::appendGames(const USBGames &src, USBGames *dest) {
    copy(begin(src), end(src), back_inserter(*dest));
}

//*******************************
// GameSubDir::print
//*******************************
void GameSubDir::print(bool plusGames) {
    string indent(displayIndentLevel, ' ');
    cout << displayRowIndex << ": " << indent << fullPath << ", " << dirName << " (" << gamesInThisDir.size() << " games)" << endl;
    if (plusGames) {
        for (auto & game : gamesInThisDir)
            cout << indent << " " << game->pathName << endl;
    }
    for (auto & child : childrenDirs)
        child->print(plusGames);
}

//*******************************
// GameSubDir::scanGamesHierarchy
//*******************************
GameSubDirRows GameSubDir::scanGamesHierarchy(const std::string & path) {
    GameSubDirRows displayRows;
    GameSubDirPtr gamesHierarchy(new GameSubDir(path, &displayRows));
    gamesHierarchy->scanAll();
    if (gamesHierarchy->allGames.size() > 0)
        displayRows.emplace_back(gamesHierarchy);

    sort(begin(displayRows), end(displayRows), [] (const GameSubDirPtr &gameSubDir1, const GameSubDirPtr &gameSubDir2)
        { return gameSubDir2->displayRowIndex > gameSubDir1->displayRowIndex; });

#if 1
    for (auto & row : displayRows)
        cout << row->displayRowIndex << ": " << string(row->displayIndentLevel, ' ') << row->dirName <<
             " (" << row->allGames.size() << " games)" << endl;
#endif

    return displayRows;
}
