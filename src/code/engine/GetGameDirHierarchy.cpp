#include "GetGameDirHierarchy.h"
#include "../util.h"
#include "../DirEntry.h"
#include <iostream>
#include "scanner.h"
#include "serialscanner.h"
#include <fstream>

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
            gamesInThisDir.emplace_back(game);
            //cout << "added game: " << game->pathName << endl;
        } else {
            //cout << "subdir: " << path << endl;
            GameSubDirPtr subdir(new GameSubDir(path,
                                 displayRowIndex + childrenDirs.size() + 1, displayIndentLevel + 1, displayRows));
            if (subdir->gamesInThisDir.size() > 0 || subdir->gamesInChildrenDirs.size() > 0) {
                gamesInChildrenDirs += subdir->gamesInThisDir;
                gamesInChildrenDirs += subdir->gamesInChildrenDirs;
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
    // remove the games in our copy of the games in the child dir that are duplicates of games in this row
    // so he game won't show up twice when viewing this row's carousel
    removeChildGamesThatAreDuplicatesOfGamesInThisRow();

    // allGames to display for this row are all the games in this dir and the child dirs
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
    }
    printGamesInEachRow();
}

//*******************************
// GamesHierarchy::makeGamesToDisplayWhileRemovingChildDuplicates
// run this after Scanner has filled in the serial so we can correctly match duplicate games
//*******************************
void GamesHierarchy::makeGamesToDisplayWhileRemovingChildDuplicates() {
    for (auto & row : gameSubDirRows) {
        USBGame::sortByTitle(row->gamesInThisDir);
        USBGame::sortByTitle(row->gamesInChildrenDirs);
        row->makeGamesToDisplayWhileRemovingChildDuplicates();
        USBGame::sortByTitle(row->gamesToDisplay);
    }
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

//*******************************
// GamesHierarchy::gamesDoNotMatchAutobleemPrev
//*******************************
bool GamesHierarchy::gamesDoNotMatchAutobleemPrev(const std::string & autobleemPrevPath) {
    auto allGames = getAllGames();
    USBGame::sortByFullPath(allGames);
//cout << "gamesDoNotMatchAutobleemPrev" << endl;
    for (const auto &g : allGames) cout << g->fullPath << endl;

    ifstream prev;
    prev.open(autobleemPrevPath.c_str(), ios::binary);
    for (const auto game : allGames) {
        string pathInFile;
        getline(prev, pathInFile);
//cout << "compare " << pathInFile << " ======== " << game->fullPath << endl;
        if (pathInFile != game->fullPath) {
//cout << "compare failed" << endl;
            return true;    // the autobleem.prev file does not match
        }
    }
    prev.close();

    return false;
}

//*******************************
// GamesHierarchy::writeAutobleemPrev
//*******************************
void GamesHierarchy::writeAutobleemPrev(const std::string & autobleemPrevPath) {
    auto allGames = getAllGames();

    USBGame::sortByFullPath(allGames);
    cout << "writeAutobleemPrev" << endl;
    for (const auto &g : allGames) cout << g->fullPath << endl;

    ofstream prev;
    prev.open(autobleemPrevPath.c_str(), ios::binary);
    for (const auto game : allGames) {
        prev << game->fullPath << endl;
    }
    prev.close();
}

//*******************************
// GamesHierarchy::removeGameFromEntireHierarchy
// if a game failed to verify in Scanner it needs to be removed
//*******************************
void GamesHierarchy::removeGameFromEntireHierarchy(USBGamePtr &game) {
    // the game did not pass the verify step and was not added to the DB.
    // remove the game everywhere in the gamesHierarchy
    cout << "game: " << game->title << " did not pass verify() test" << endl;
    // remove the game everywhere in the gamesHierarchy
    for (auto &row : gameSubDirRows) {
        auto it = remove_if(begin(row->gamesInThisDir), end(row->gamesInThisDir),
                            [&](USBGamePtr &g) { return g->fullPath == game->fullPath; });
        row->gamesInThisDir.erase(it, end(row->gamesInThisDir));

        it = remove_if(begin(row->gamesInChildrenDirs), end(row->gamesInChildrenDirs),
                       [&](USBGamePtr &g) { return g->fullPath == game->fullPath; });
        row->gamesInChildrenDirs.erase(it, end(row->gamesInChildrenDirs));

        it = remove_if(begin(row->gamesToDisplay), end(row->gamesToDisplay),
                       [&](USBGamePtr &g) { return g->fullPath == game->fullPath; });
        row->gamesToDisplay.erase(it, end(row->gamesToDisplay));
    }
}

//*******************************
// GamesHierarchy::printGamesInEachRow
//*******************************
void GamesHierarchy::printGamesInEachRow() {
    cout << "Games in each row" << endl;
    // display the row name
    for (auto & row : gameSubDirRows) {
        cout << row->displayRowIndex << ": " << string(row->displayIndentLevel, ' ') << row->subDirName <<
             " (" << row->gamesInThisDir.size() << " games)" << endl;
        // display the game name
        for (auto & game : row->gamesInThisDir) {
            int indexStringSize = string(to_string(row->displayRowIndex)).size();
            int numSpaces = indexStringSize + sizeof(": ") + row->displayIndentLevel + 1;
            cout << string(numSpaces, ' ') + game->gameDirName << endl;
        }
    }
}

//*******************************
// GamesHierarchy::printGamesToDisplayInEachRow
//*******************************
void GamesHierarchy::printGamesToDisplayInEachRow() {
    cout << "Games to display in each row" << endl;
    // display the row name
    for (auto & row : gameSubDirRows) {
        cout << row->displayRowIndex << ": " << string(row->displayIndentLevel, ' ') << row->subDirName <<
             " (" << row->gamesToDisplay.size() << " games)" << endl;
        // display the game name
        for (auto & game : row->gamesToDisplay) {
            int indexStringSize = string(to_string(row->displayRowIndex)).size();
            int numSpaces = indexStringSize + sizeof(": ") + row->displayIndentLevel + 1;
            cout << string(numSpaces, ' ') + game->gameDirName << endl;
        }
    }
}
