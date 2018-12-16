//
// Created by screemer on 2018-12-15.
//

#include "scanner.h"
#include "main.h"
#include "metadata.h"
#include "ecmhelper.h"

#include <stdio.h>


static const char GAME_DATA[] = "GameData";
static const char GAME_INI[] = "Game.ini";
static const char PCSX_CFG[] = "pcsx.cfg";
static const char LABEL[] = ".png";
static const char ECM[] = ".ecm";
static const char LICENCE[] = ".lic";


bool wayToSort(Game i, Game j) {
    return i.title < j.title;
}

void Scanner::unecm(string path) {
    vector<DirEntry> rootDir = Util::dir(path);
    for (std::vector<int>::size_type i = 0; i != rootDir.size(); i++) {
        DirEntry entry = rootDir[i];
        if (entry.name[0] == '.') continue;
        if (Util::strcicmp(entry.name.substr(entry.name.length() - 4).c_str(), ECM) == 0) {
            // oh someone forgot to unpack ECM ... let me do that for you - THIS IS EXPERIMENTAL
            Ecmhelper ecm;
            if (ecm.unecm(path + entry.name, path + entry.name.substr(0, entry.name.length() - 4))) {
                // successfuly unpacked
                remove((path + entry.name).c_str());

            }
        }

    }
}

void Scanner::updateDB(Database *db) {

    string path = Util::getWorkingPath() + Util::separator() + "autobleem.list";
    ofstream outfile;
    outfile.open(path);
    if (complete)
        for (int i = 0; i < games.size(); i++) {
            Game data = games[i];
            cout << "Inserting game ID: " << i + 1 << " - " << data.title << endl;
            db->insertGame(i + 1, data.title, data.publisher, data.players, data.year);
            for (int j = 0; j < data.discs.size(); j++) {
                db->insertDisc(i + 1, j + 1, data.discs[j].diskName);
            }
            outfile << i + 1 << "," << data.fullPath.substr(0, data.fullPath.size() - 1) << endl;

        }
    outfile.flush();
    outfile.close();
}

void Scanner::scanDirectory(string path) {
    // clear games list
    games.clear();
    complete = false;

    vector<DirEntry> rootDir = Util::dir(path);
    for (std::vector<int>::size_type i = 0; i != rootDir.size(); i++) {
        DirEntry entry = rootDir[i];
        if (entry.name[0] == '.') continue;
        if (!entry.dir) continue;

        Game game;
        game.folder_id = atoi(entry.name.c_str()); // this will not be in use;
        game.fullPath = path + entry.name + Util::separator();
        game.pathName = entry.name;

        string gameDataPath = path + entry.name + Util::separator() + GAME_DATA + Util::separator();

        bool gameDataExists = Util::exists(gameDataPath);
        if (!gameDataExists) {
            cerr << "Game: " << entry.name << " - GameData Not found" << endl;
            Util::createDir(gameDataPath);


        }
        game.gameDataFound = true;


        vector<DirEntry> gameRoot = Util::dir(path + entry.name + Util::separator());
        for (std::vector<int>::size_type j = 0; j != gameRoot.size(); j++) {
            DirEntry entryGame = gameRoot[j];
            if (entryGame.name[0] == '.') continue;
            if (entryGame.name == GAME_DATA) continue;

            string oldName = path + entry.name + Util::separator() + entryGame.name;
            string newName = gameDataPath + entryGame.name;
            cerr << "Moving: " << oldName << "  to: " << newName << endl;
            rename(oldName.c_str(), newName.c_str());
        }

        unecm(gameDataPath);

        if (!Util::exists(gameDataPath + GAME_INI)) {
            game.readIni(gameDataPath + GAME_INI);
            game.gameIniFound = false;
        } else {
            game.gameIniFound = true;
        }


        vector<DirEntry> gameDir = Util::dir(gameDataPath);
        for (std::vector<int>::size_type j = 0; j != gameDir.size(); j++) {
            DirEntry entryGame = gameDir[j];
            if (entryGame.name[0] == '.') continue;

            if (Util::strcicmp(entryGame.name.c_str(), GAME_INI) == 0) {
                string gameIniPath = gameDataPath + GAME_INI;
                game.readIni(gameIniPath);
            }

            if (Util::strcicmp(entryGame.name.c_str(), PCSX_CFG) == 0) {
                game.pcsxCfgFound = true;
            }

            if (Util::strcicmp(entryGame.name.c_str(), PCSX_CFG) == 0) {
                game.pcsxCfgFound = true;
            }

            if (entryGame.name.length() > 4)
                if (Util::strcicmp(entryGame.name.substr(entryGame.name.length() - 4).c_str(), LABEL) == 0) {
                    game.imageFound = true;
                }
            if (entryGame.name.length() > 4)

                if (entryGame.name.length() > 4)
                    if (Util::strcicmp(entryGame.name.substr(entryGame.name.length() - 4).c_str(), LICENCE) == 0) {
                        game.licFound = true;
                    }


        }
        game.recoverFiles();
        if (!game.gameIniFound || game.automationUsed) {
            string serial = game.scanSerial();
            if (serial.length() > 0) {
                cout << "Accessing metadata for serial: " << serial << endl;
                Metadata md;
                if (md.lookup(serial)) {
                    // at this stage we have more data;
                    game.title = md.title;
                    game.publisher = md.publisher;
                    game.players = md.players;
                    game.year = md.year;

                    if (game.discs.size() > 0) {
                        // all recovered :)
                        string newFilename = gameDataPath + game.discs[0].cueName + ".png";
                        cout << "Updating cover" << newFilename << endl;
                        ofstream pngFile;
                        pngFile.open(newFilename);
                        pngFile.write(md.bytes, md.dataSize);
                        pngFile.flush();
                        pngFile.close();
                        game.automationUsed = false;
                        game.imageFound = true;

                    }

                    free(md.bytes);
                } else {
                    game.title = game.pathName;
                }

            }
        }
        game.saveIni(gameDataPath + GAME_INI);

        game.print();
        if (game.verify()) {
            games.push_back(game);
        }

    }
    sort(games.begin(), games.end(), wayToSort);

    complete = true;
}