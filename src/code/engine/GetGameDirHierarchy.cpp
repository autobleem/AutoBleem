#include "GetGameDirHierarchy.h"
#include "../util.h"
#include "../DirEntry.h"
#include <iostream>
#include "scanner.h"

//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;

using namespace std;

//*******************************
// GameSubDir::GameSubDir
//*******************************
GameSubDir::GameSubDir(const std::string & _fullPath, int _displayRowIndex, int _displayIndentLevel,
                       GameSubDirRows *_displayRows) {
    fullPath = DirEntry::removeSeparatorFromEndOfPath(_fullPath);
    subDirName = DirEntry::getFileNameFromPath(fullPath);

    displayRowIndex = _displayRowIndex;
    displayIndentLevel = _displayIndentLevel;

    displayRows = _displayRows;
    scanAll();
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
            game->gameDirName = dir.name;
            gamesInThisDir.emplace_back(game);
            //cout << "added game: " << game->pathName << endl;
        } else {
            //cout << "subdir: " << path << endl;
            GameSubDirPtr subdir(new GameSubDir(path,
                                 displayRowIndex + childrenDirs.size() + 1, displayIndentLevel + 1,
                    displayRows));

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
    cout << displayRowIndex << ": " << indent << fullPath << ", " << subDirName << " (" << gamesInThisDir.size() << " games)" << endl;
    if (plusGames) {
        for (auto & game : gamesInThisDir)
            cout << indent << " " << game->gameDirName << endl;
    }
    for (auto & child : childrenDirs)
        child->print(plusGames);
}

//*******************************
// GameSubDir::scanGamesHierarchy
//*******************************
GameSubDirRows GameSubDir::scanGamesHierarchy(const std::string & path) {
    GameSubDirRows displayRows;
    GameSubDirPtr gamesHierarchy(new GameSubDir(path, 0, 0, &displayRows));
    if (gamesHierarchy->allGames.size() > 0)
        displayRows.emplace_back(gamesHierarchy);

    sort(begin(displayRows), end(displayRows), [] (const GameSubDirPtr &gameSubDir1, const GameSubDirPtr &gameSubDir2)
        { return gameSubDir2->displayRowIndex > gameSubDir1->displayRowIndex; });

    for (auto & row : displayRows) {
        Scanner::sortByTitle(row->allGames);
    }
#if 1
    for (auto & row : displayRows)
        cout << row->displayRowIndex << ": " << string(row->displayIndentLevel, ' ') << row->subDirName <<
             " (" << row->allGames.size() << " games)" << endl;
#endif

    return displayRows;
}
