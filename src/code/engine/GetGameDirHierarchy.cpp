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
void GameSubDir::makeGamesToDisplayWhileRemovingChildDuplicates(ofstream &dupFile) {
    gamesToDisplay.clear();
    // remove the games in our copy of the games in the child dir that are duplicates of games in this row
    // so he game won't show up twice when viewing this row's carousel
    removeChildGamesThatAreDuplicatesOfGamesInThisRow(dupFile);

    // there could be duplicate games among the children dirs where the same game isn't also in the parent dir.
    // since all the children games are included in the parent's gamesToDisplay list we need the remove duplicates among
    // the child dirs.
    removeDuplicateGamesAmongTheChildren(dupFile);

    // allGames to display for this row are all the games in this dir and the child dirs
    gamesToDisplay += gamesInThisDir;
    gamesToDisplay += gamesInChildrenDirs;
}

//*******************************
// GameSubDir::sameGame
// if both ames have a serial comare the serials
// else compare the gameDirName
//*******************************
bool GameSubDir::sameGame(const USBGamePtr &game1, const USBGamePtr &game2) {
    if (game1->serial != "" && game2->serial != "")
        return (game1->serial == game2->serial);
    else
        return Util::compareCaseInsensitive(game1->title, game2->title);

}

//*******************************
// GameSubDir::removeChildGamesThatAreDuplicatesOfGamesInThisRow
// remove the games in the child dir that are duplicates of games in this row.
// so the game won't show up twice when viewing this row's carousel.
// the game in the parent dir has precedence.
//*******************************
void GameSubDir::removeChildGamesThatAreDuplicatesOfGamesInThisRow(ofstream &dupFile) {
    for (auto &game : gamesInThisDir) {
        auto it = remove_if(begin(gamesInChildrenDirs), end(gamesInChildrenDirs), [&game, &dupFile, this] (USBGamePtr &childGame) {
            if (sameGame(game, childGame)) {
                dupFile << "removed duplicate: serial: " << childGame->serial << ", " << childGame->fullPath << ", from: "
                     << this->subDirName << " gamesToDisplay list" << endl;
                dupFile << endl;

                return true;
            } else
                return false;
        });
        gamesInChildrenDirs.erase(it, gamesInChildrenDirs.end());   // actually erase the ones to be removed
    }
}

//*******************************
// GameSubDir::removeDUplicateGamesAmongTheChildren
// remove duplicate games amongh the children dirs.
// so the game won't show up twice when viewing this row's carousel of all games here and below.
//*******************************
void GameSubDir::removeDuplicateGamesAmongTheChildren(std::ofstream &dupFile) {
    // do a reverse sort of the child games by the full path.
    // we want to keep the highest path alphabetically.  when adjacent_find finds a matching title after the second sort
    // if will return an iter to the first adjacent pair.  that is the one we will delete.  so the game being deleted
    // will be the lower title alphabetically.
    sort(begin(gamesInChildrenDirs), end(gamesInChildrenDirs),  [] (const USBGamePtr &g1, const USBGamePtr &g2) { return SortByCaseInsensitive(g2->fullPath, g1->fullPath); });
    USBGame::sortByTitle(gamesInChildrenDirs);
    auto it = begin(gamesInChildrenDirs);
    while ((it = adjacent_find(begin(gamesInChildrenDirs), end(gamesInChildrenDirs), sameGame)) != end(gamesInChildrenDirs)) {
        dupFile << "removed duplicate: serial: " << (*it)->serial << ", " << (*it)->fullPath << ", that was located in " <<
            "multiple children of " << this->subDirName << endl;
        dupFile << endl;

        it = gamesInChildrenDirs.erase(it); // erase the first of the two games
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
    printRowGameInfo(false);
}

//*******************************
// GamesHierarchy::makeGamesToDisplayWhileRemovingChildDuplicates
// run this after Scanner has filled in the serial so we can correctly match duplicate games
//*******************************
void GamesHierarchy::makeGamesToDisplayWhileRemovingChildDuplicates() {
    string dupFilePath = DirEntry::getWorkingPath() + sep + "duplicateGames.txt";
    ofstream dupFile;
    dupFile.open(dupFilePath.c_str(), ios::binary);

    for (auto & row : gameSubDirRows) {
        USBGame::sortByTitle(row->gamesInThisDir);
        USBGame::sortByTitle(row->gamesInChildrenDirs);
        row->makeGamesToDisplayWhileRemovingChildDuplicates(dupFile);
        USBGame::sortByTitle(row->gamesToDisplay);
    }
    dupFile.close();
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
// GamesHierarchy::printRowGameInfo
//*******************************
void GamesHierarchy::printRowGameInfo(bool alsoPrintGames) {
    cout << "Games in each row" << endl;
    // display the row name
    for (auto & row : gameSubDirRows) {
        cout << string(row->displayIndentLevel, ' ') << row->subDirName <<
                " (" << row->gamesInThisDir.size() << " games)" << endl;
        if (alsoPrintGames) {
            // display the game name
            for (auto & game : row->gamesInThisDir) {
                int indexStringSize = string(to_string(row->displayRowIndex)).size();
                int numSpaces = indexStringSize + sizeof(": ") + row->displayIndentLevel + 1;
                cout << string(numSpaces, ' ') + game->gameDirName << endl;
            }
        }
    }
}

//*******************************
// GamesHierarchy::printRowDisplayGameInfo
//*******************************
void GamesHierarchy::printRowDisplayGameInfo(bool alsoPrintGames) {
    cout << "Games to display in each row" << endl;
    // display the row name
    for (auto & row : gameSubDirRows) {
        cout << string(row->displayIndentLevel, ' ') << row->subDirName <<
             " (" << row->gamesToDisplay.size() << " games)" << endl;
        if (alsoPrintGames) {
            // display the game name
            for (auto &game : row->gamesToDisplay) {
                int indexStringSize = string(to_string(row->displayRowIndex)).size();
                int numSpaces = indexStringSize + sizeof(": ") + row->displayIndentLevel + 1;
                cout << string(numSpaces, ' ') + game->gameDirName << endl;
            }
        }
    }
}
