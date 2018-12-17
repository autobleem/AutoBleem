//
// Created by screemer on 2018-12-15.
//

#include "scanner.h"
#include "ecmhelper.h"


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
    // This is really slow - do not want to run it until some GUI shows progress

    for (DirEntry entry: Util::dir(path)) {
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

static const char cue1[] = "FILE \"{binName}\" BINARY\n"
                           "  TRACK 01 MODE2/2352\n"
                           "    INDEX 01 00:00:00\n";
static const char cue2[] = "FILE \"{binName}\" BINARY\n"
                           "  TRACK 02 AUDIO\n"
                           "    INDEX 00 00:00:00\n"
                           "    INDEX 01 00:02:00\n";

void replaceAll(std::string &str, const std::string &from, const std::string &to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void repairMissingCue(string path, string folderName) {
    vector<string> binFiles;
    bool hasCue = false;
    vector<DirEntry> rootDir = Util::dir(path);
    for (DirEntry entry:rootDir) {
        if (entry.name[0] == '.') continue;
        if (entry.name.length() > 4)
            if (Util::strcicmp(entry.name.substr(entry.name.length() - 4).c_str(), ".cue") == 0) {
                hasCue = true;
            }
        if (entry.name.length() > 4)
            if (Util::strcicmp(entry.name.substr(entry.name.length() - 4).c_str(), ".bin") == 0) {
                binFiles.push_back(entry.name);
            }
    }
    if (!hasCue) {
        string newCueName = path + folderName + ".cue";
        ofstream os;
        os.open(newCueName);
        // let's create new one
        bool first = true;
        for (string bin:binFiles) {
            string cueElement;
            if (first) {
                cueElement = cue1;
            } else {
                cueElement = cue2;
            }

            replaceAll(cueElement, "{binName}", bin);
            first = false;
            os << cueElement;
        }
        os.flush();
        os.close();
    }
}

void Scanner::moveFolderIfNeeded(DirEntry entry, string gameDataPath, string path) {
    bool gameDataExists = Util::exists(gameDataPath);
    if (!gameDataExists) {
        cerr << "Game: " << entry.name << " - GameData Not found" << endl;
        Util::createDir(gameDataPath);

        for (DirEntry entryGame:  Util::dir(path + entry.name + Util::separator())) {

            if (entryGame.name[0] == '.') continue;
            if (entryGame.name == GAME_DATA) continue;

            string oldName = path + entry.name + Util::separator() + entryGame.name;
            string newName = gameDataPath + entryGame.name;
            cerr << "Moving: " << oldName << "  to: " << newName << endl;
            rename(oldName.c_str(), newName.c_str());
        }
    }
}

void Scanner::scanDirectory(string path) {
    // clear games list
    games.clear();
    complete = false;


    for (DirEntry entry: Util::dir(path)) {
        if (entry.name[0] == '.') continue;
        if (!entry.dir) continue;

        Game game;
        game.folder_id = 0; // this will not be in use;
        game.fullPath = path + entry.name + Util::separator();
        game.pathName = entry.name;

        string gameDataPath = path + entry.name + Util::separator() + GAME_DATA + Util::separator();

        moveFolderIfNeeded(entry, gameDataPath, path);
        game.gameDataFound = true;

        repairMissingCue(gameDataPath, entry.name);


        /*
         *
         *  Uncomment this to UNECM data
         *  COMMENTED AS IT IS SLOW AND SCREEN SHOWS JUST BLACK
         */
        //unecm(gameDataPath);

        if (!Util::exists(gameDataPath + GAME_INI)) {
            game.readIni(gameDataPath + GAME_INI);
            game.gameIniFound = false;
        } else {
            game.gameIniFound = true;
        }

        for (DirEntry entryGame:Util::dir(gameDataPath)) {
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

        game.recoverMissingFiles();

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