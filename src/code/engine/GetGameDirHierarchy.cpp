#include "GetGameDirHierarchy.h"
#include "../util.h"
#include "../DirEntry.h"
#include <iostream>
#include "scanner.h"
#include "serialscanner.h"

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
    DirEntries dirs = DirEntry::diru_DirsOnly(fullPath);
    for (auto & dirEntry : dirs) {
        if (dirEntry.name == "!SaveStates")
            continue;
        if (dirEntry.name == "!MemCards")
            continue;

        DirEntry::fixCommaInDirOrFileName(fullPath, &dirEntry);

        string path = fullPath + sep + dirEntry.name;
        //cout << "path: " << path << endl;
        if (DirEntry::thereIsAGameFile(path)) {
            USBGamePtr game{new USBGame};
            game->fullPath = path;
            game->gameDirName = dirEntry.name;
            game->displayRowIndex = displayRowIndex;
#if 0
            ImageType imageType;
            string gameFile;
            tie(imageType, gameFile) = DirEntry::getGameFile(DirEntry::diru_FilesOnly(path));
            game->imageType = imageType;
            game->firstBinPath = path + sep + gameFile;
            game->serial = SerialScanner::scanSerial(game->imageType, game->fullPath + sep, game->firstBinPath);
            game->region = SerialScanner::serialToRegion(game->serial);
#endif
            gamesInThisDir.emplace_back(game);
            //cout << "added game: " << game->pathName << endl;
        } else {
            //cout << "subdir: " << path << endl;
            GameSubDirPtr subdir(new GameSubDir(path,
                                 displayRowIndex + childrenDirs.size() + 1, displayIndentLevel + 1, displayRows));
            if (subdir->gamesInThisDir.size() > 0 || subdir->gamesInChildrenDirs.size() > 0) {
                childrenDirs.emplace_back(subdir);
                displayRows->emplace_back(subdir);
            }
        }
    }
}

//*******************************
// GameSubDir::makeGamesToDisplayWhileRemovingChildDuplicates
// we delay running this until after the serial is in the USBGame
//*******************************
void GameSubDir::makeGamesToDisplayWhileRemovingChildDuplicates() {
    // gather the children
    for (auto & childDir : childrenDirs) {
        gamesInChildrenDirs += childDir->gamesInThisDir;
        gamesInChildrenDirs += childDir->gamesInChildrenDirs;
    }

    // remove the games in our copy of the games in the child dir that are duplicates of games in this row
    // so he game won't show up twice when viewing this row's carousel
    removeChildGamesThatAreDuplicatesOfGamesInThisRow();

    // allGames is all the games in this dir and the child dirs
    gamesToDisplay += gamesInThisDir;
    gamesToDisplay += gamesInChildrenDirs;
}

//*******************************
// GameSubDir::removeChildGamesThatAreDuplicatesOfGamesInThisRow
// remove the games in our copy of the games in the child dir that are duplicates of games in this row
// so he game won't show up twice when viewing this row's carousel
//*******************************
void GameSubDir::removeChildGamesThatAreDuplicatesOfGamesInThisRow() {
    for (auto &game : gamesInThisDir) {
        auto it = remove_if(begin(gamesInChildrenDirs), end(gamesInChildrenDirs), [&game, this] (USBGamePtr &childGame) {
            bool removeMe = false;
            if (game->serial != "" && childGame->serial != "") {
                removeMe = (game->serial == childGame->serial);
            }
            else if (game->serial == "" && childGame->serial == "") {
                removeMe = (game->gameDirName == childGame->gameDirName);
            }

            if (removeMe)
                cout << "removed duplicate: serial: " << childGame->serial << ", " << childGame->fullPath << ", from: " << this->subDirName << endl;

            return removeMe;
        });
        gamesInChildrenDirs.erase(it, gamesInChildrenDirs.end());   // actually erase the ones to be removed
    }
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
// GameSubDir::USBGames += USBGames
//*******************************
void operator += (USBGames &dest, const USBGames &src) {
    copy(begin(src), end(src), back_inserter(dest));
}

//*******************************
// GamesHierarchy::GamesHierarchy(path)
//*******************************
GamesHierarchy::GamesHierarchy(const std::string & path) {
    GameSubDirPtr top(new GameSubDir(path, 0, 0, &gameSubDirRows));
    if (top->gamesInThisDir.size() > 0 || top->gamesInChildrenDirs.size() > 0)
        gameSubDirRows.emplace_back(top);

    sort(begin(gameSubDirRows), end(gameSubDirRows), [] (const GameSubDirPtr &gameSubDir1, const GameSubDirPtr &gameSubDir2)
        { return gameSubDir2->displayRowIndex > gameSubDir1->displayRowIndex; });

    for (auto & row : gameSubDirRows) {
        USBGame::sortByTitle(row->gamesInThisDir);
        USBGame::sortByTitle(row->gamesInChildrenDirs);
        row->makeGamesToDisplayWhileRemovingChildDuplicates();
        USBGame::sortByTitle(row->gamesToDisplay);
    }

#if 1
    for (auto & row : gameSubDirRows)
        cout << row->displayRowIndex << ": " << string(row->displayIndentLevel, ' ') << row->subDirName <<
             " (" << row->gamesToDisplay.size() << " games)" << endl;
#endif
}

//*******************************
// GamesHierarchy::getAllGames
//*******************************
USBGames GamesHierarchy::getAllGames() {
    USBGames allGames;
    for (auto & row : gameSubDirRows)
        allGames += row->gamesInThisDir;

    return allGames;
}
