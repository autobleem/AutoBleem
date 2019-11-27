#include "GetGameDirHierarchy.h"
#include "../util.h"
#include "../DirEntry.h"
#include <iostream>
#include "scanner.h"
#include "serialscanner.h"
#include <fstream>
#include "../environment.h"

//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;

using namespace std;

                                //*******************************
                                // GameSubDir
                                //*******************************

//*******************************
// GameSubDir::GameSubDir
//*******************************
GameSubDir::GameSubDir(const std::string & _fullPath, int _displayIndentLevel, GameSubDirRows *_displayRows) {
    fullPath = DirEntry::removeSeparatorFromEndOfPath(_fullPath);
    subDirName = DirEntry::getFileNameFromPath(fullPath);
    displayIndentLevel = _displayIndentLevel;
    displayRows = _displayRows;
}

//*******************************
// GameSubDir::scanAll
// recursive scan of the sub directories
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
            GameSubDirPtr subdir(new GameSubDir(path, displayIndentLevel + 1, displayRows));
            displayRows->emplace_back(subdir);
            subdir->scanAll();

            if (subdir->gamesInThisDir.size() > 0 || subdir->gamesInChildrenDirs.size() > 0) {
                // we need these so we will add subdirs that have no games but subdirs that do
                // if scanner calls makeGamesToDisplayWhileRemovingChildDuplicates it will be rebuilt without duplicates
                gamesInChildrenDirs += subdir->gamesToDisplay;
                childrenDirs.emplace_back(subdir);
            }
            else {
                cout << subdir->subDirName << " FAILED TO ADD" << endl;
            }
        }
    }
    gamesToDisplay = gamesInThisDir;
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
// remove any games in the second vector that are duplicates of games in the first vector.
//*******************************
void GameSubDir::removeGamesInSecondListThatMatchAGameInFirstList(USBGames &parentGames, USBGames &childGames, std::ofstream &dupFile) {
    for (auto &parentGame : parentGames) {
        auto it = remove_if(begin(childGames), end(childGames), [&parentGame, &dupFile] (USBGamePtr &childGame) {
            cout << "compare " << parentGame->title << " with " << childGame->title << endl;
            if (sameGame(parentGame, childGame)) {
                dupFile << "removed duplicate child game: " << childGame->fullPath << endl;
                dupFile << endl;

                return true;
            } else
                return false;
        });
        if (it != childGames.end())
            childGames.erase(it, childGames.end());   // actually erase the ones to be removed
    }
}

//*******************************
// GameSubDir::removeDuplicateGamesLeavingOne
// remove duplicate games in the vector leaving a single game of each title.
//*******************************
void GameSubDir::removeDuplicateGamesLeavingOne(USBGames &games, std::ofstream &dupFile) {
    // do a reverse sort of the games by the full path.
    // we want to keep the highest path alphabetically.  when adjacent_find finds a matching title after the second sort
    // it will return an iter to the first adjacent pair.  that is the one we will delete.  so the game being deleted
    // will be the lower title alphabetically.
    sort(begin(games), end(games),  [] (const USBGamePtr &g1, const USBGamePtr &g2)
                                    { return SortByCaseInsensitive(g2->fullPath, g1->fullPath); });
    USBGame::sortByTitle(games);
    auto it = begin(games); // simply to get the correct type.  for some reason auto gave an error.
    while ((it = adjacent_find(begin(games), end(games), sameGame)) != end(games)) {
        dupFile << "removed duplicate: " << (*it)->fullPath <<
        ", that had more than one copy in the children of it's parent dir" << endl;
        dupFile << endl;

        it = games.erase(it); // erase the first of the two games
    }
}

//*******************************
// GameSubDir::makeGamesToDisplayWhileRemovingChildDuplicates
// we delay running this until after the serial is in the USBGame
//*******************************
void GameSubDir::makeGamesToDisplayWhileRemovingChildDuplicates(ofstream &dupFile) {
    gamesToDisplay.clear();
    gamesInChildrenDirs.clear();
    for (auto &subdir : childrenDirs) {
        subdir->makeGamesToDisplayWhileRemovingChildDuplicates(dupFile);
        gamesInChildrenDirs += subdir->gamesToDisplay;
    }

    // remove duplicates so they don't show up in the carousel
    removeGamesInSecondListThatMatchAGameInFirstList(gamesInThisDir, gamesInChildrenDirs, dupFile);
    removeDuplicateGamesLeavingOne(gamesInChildrenDirs, dupFile);

    // what games to display for this game dir row after duplicates have been removed
    gamesToDisplay += gamesInThisDir;
    gamesToDisplay += gamesInChildrenDirs;
    USBGame::sortByTitle(gamesToDisplay);
}

//*******************************
// GameSubDir::print
//*******************************
void GameSubDir::print(bool plusGames) {
    string indent(displayIndentLevel * 2, ' ');
    cout << displayRowIndex << ": " << indent << fullPath << ", " << subDirName << " (" << gamesInThisDir.size() << " games)" << endl;
    if (plusGames) {
        for (auto & game : gamesInThisDir)
            cout << indent << " " << game->gameDirName << endl;
    }
    for (auto & child : childrenDirs)
        child->print(plusGames);
}

                                //*******************************
                                // GamesHierarchy
                                //*******************************

//*******************************
// GamesHierarchy::getHierarchy(path)
//*******************************
void GamesHierarchy::getHierarchy(const std::string & path) {
    gameSubDirRows.clear(); // clear any previous scan

    GameSubDirPtr top(new GameSubDir(path, 0, &gameSubDirRows));
    gameSubDirRows.emplace_back(top);
    top->scanAll();

    // remove and any thaat have nothing to display
    auto it = remove_if(begin(gameSubDirRows), end(gameSubDirRows), [&] (GameSubDirPtr &subdir) { return subdir->gamesToDisplay.size() == 0; });
    gameSubDirRows.erase(it, end(gameSubDirRows));

    int rowIndex = 0;
    for (auto & row : gameSubDirRows) {
        row->displayRowIndex = rowIndex++;  // put the row index into the row for print and debugging convenience.

        USBGame::sortByTitle(row->gamesToDisplay);
        USBGame::sortByTitle(row->gamesInThisDir);
        USBGame::sortByTitle(row->gamesInChildrenDirs);
    }
    printRowGameInfo(false);

    string opath = Env::getWorkingPath() + sep + "gameHierarchy_beforeScan.txt";
    ofstream outfile;
    outfile.open(opath);
    dumpRowGameInfo(outfile, true);
    outfile << endl << endl;
    dumpRowDisplayGameInfo(outfile, true);
    outfile.close();
}

//*******************************
// GamesHierarchy::makeGamesToDisplayWhileRemovingChildDuplicates
// run this after Scanner has filled in the serial so we can correctly match duplicate games
//*******************************
void GamesHierarchy::makeGamesToDisplayWhileRemovingChildDuplicates() {
    string dupFilePath = Env::getWorkingPath() + sep + "duplicateGames.txt";
    dupFile.open(dupFilePath.c_str(), ios::binary);

    if (gameSubDirRows.size() > 0)
        gameSubDirRows[0]->makeGamesToDisplayWhileRemovingChildDuplicates(dupFile); // recursive

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
// GamesHierarchy::dumpRowGameInfo
//*******************************
void GamesHierarchy::dumpRowGameInfo(ostream &o, bool alsoPrintGames) {
    o << "Games in each row" << endl;
    // display the row name
    for (auto & row : gameSubDirRows) {
        o << to_string(row->displayRowIndex) + ": " + string(row->displayIndentLevel * 2, ' ') << row->subDirName <<
             " (" << row->gamesInThisDir.size() << " games)" << endl;
        if (alsoPrintGames) {
            // display the game name
            for (auto & game : row->gamesInThisDir) {
                int indexStringSize = string(to_string(row->displayRowIndex)).size();
                int numSpaces = indexStringSize + sizeof(": ") + row->displayIndentLevel + 2;
                o << string(numSpaces, ' ') + game->gameDirName << endl;
            }
        }
    }
}

//*******************************
// GamesHierarchy::dumpRowDisplayGameInfo
//*******************************
void GamesHierarchy::dumpRowDisplayGameInfo(ostream &o, bool alsoPrintGames) {
    o << "Games to display in each row" << endl;
    // display the row name
    for (auto & row : gameSubDirRows) {
        o << to_string(row->displayRowIndex) + ": " + string(row->displayIndentLevel * 2, ' ') << row->subDirName <<
          " (" << row->gamesToDisplay.size() << " games)" << endl;
        if (alsoPrintGames) {
            // display the game name
            for (auto &game : row->gamesToDisplay) {
                int indexStringSize = string(to_string(row->displayRowIndex)).size();
                int numSpaces = indexStringSize + sizeof(": ") + row->displayIndentLevel + 2;
                o << string(numSpaces, ' ') + game->gameDirName << endl;
            }
        }
    }
}

//*******************************
// GamesHierarchy::printRowGameInfo
//*******************************
void GamesHierarchy::printRowGameInfo(bool alsoPrintGames) {
    dumpRowGameInfo(cout, alsoPrintGames);
}

//*******************************
// GamesHierarchy::printRowDisplayGameInfo
//*******************************
void GamesHierarchy::printRowDisplayGameInfo(bool alsoPrintGames) {
    dumpRowDisplayGameInfo(cout, alsoPrintGames);
}
