//
// Created by screemer on 2018-12-15.
//

#include "scanner.h"
#include "ecmhelper.h"
#include "cfgprocessor.h"
#include "serialscanner.h"
#include "../lang.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include "GetGameDirHierarchy.h"

using namespace std;

//*******************************
// Scanner::unecm
//*******************************
//
// this routine removes Error Correction files from the bin file to save space
// https://www.lifewire.com/ecm-file-2620956
// https://en.wikipedia.org/wiki/Error_correction_mode
//
void Scanner::unecm(const string & path) {
    for (const DirEntry & entry: DirEntry::dir(path)) {
        if (entry.name[0] == '.') continue;
        if (DirEntry::matchExtension(entry.name, EXT_ECM)) {
            Ecmhelper ecm;
            shared_ptr<Gui> splash(Gui::getInstance());
            splash->logText(_("Decompressing ecm:"));
            if (ecm.unecm(path + sep + entry.name, path + sep + entry.name.substr(0, entry.name.length() - 4))) {
                remove((path + sep + entry.name).c_str());
            }
        }
    }
}

//*******************************
// Scanner::updateRegionalDB
//*******************************
void Scanner::updateRegionalDB(GamesHierarchy &gamesHierarchy, Database *db) {
    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText(_("Updating regional.db..."));
    string path = DirEntry::getWorkingPath() + sep + "autobleem.list";
    ofstream outfile;
    outfile.open(path);
    if (complete) {
        db->beginTransaction();
        for (int i = 0; i < gamesToAddToDB.size(); i++) {
            USBGamePtr data = gamesToAddToDB[i];
            cout << "Inserting game ID: " << i + 1 << " - " << data->title << endl;
            data->gameId = i + 1;
            db->insertGame(data->gameId, data->title, data->publisher, data->players, data->year, data->fullPath + sep,
                           data->saveStatePath + sep, data->memcard);
            if (data->discs.size() == 0)
                cout << "No discs in game: " << data->title << endl;
            for (int j = 0; j < data->discs.size(); j++) {
                db->insertDisc(i + 1, j + 1, data->discs[j].diskName);
            }
            outfile << i + 1 << "," << Util::escape(data->fullPath.substr(0, data->fullPath.size() - 1)) << ","
                    << Util::escape(data->saveStatePath.substr(0, data->saveStatePath.size() - 1)) << '\n';
        }
        db->commit();
    }
    outfile.flush();
    outfile.close();

    //cout << "about to write hierarchy to DB" << endl;
    gamesHierarchy.printRowDisplayGameInfo(false);

    db->beginTransaction();
    for (auto &row : gamesHierarchy.gameSubDirRows) {
        //cout << " write row: " << row->displayRowIndex << ", " << row->subDirName << ", " << row->displayIndentLevel << ", " << row->gamesToDisplay.size() << endl;
        db->insertSubDirRow(row->displayRowIndex, row->subDirName, row->displayIndentLevel, row->gamesToDisplay.size());

        for (auto &game : row->gamesToDisplay) {
            //cout << " write game: " << game->gameDirName << ", " << row->displayRowIndex << ", " << game->gameId << endl;
            db->insertSubDirGames(row->displayRowIndex, game->gameId);
        }
    }
    db->commit();
}


static const char cue1[] = "FILE \"{binName}\" BINARY\n"
                           "  TRACK 01 MODE2/2352\n"
                           "    INDEX 01 00:00:00\n";
static const char cue2[] = "FILE \"{binName}\" BINARY\n"
                           "  TRACK {track} AUDIO\n"
                           "    INDEX 00 00:00:00\n"
                           "    INDEX 01 00:02:00\n";

                                //*******************************
                                // local routines
                                // *******************************

//*******************************
// repairBinCommaNames
//*******************************
void repairBinCommaNames(const string & path) {
    // TODO: Add support for German diactrics for nex here
    for (DirEntry entry : DirEntry::diru_FilesOnly(path)) {
        if (DirEntry::fixCommaInDirOrFileName(path, &entry)) {
            if (DirEntry::matchExtension(entry.name, EXT_CUE)) {
                // process cue inside
                ifstream is(path + sep + entry.name);
                ofstream os(path + sep + entry.name + ".new");
                string line;
                while (getline(is, line)) {
                    trim(line);
                    if (!line.empty()) {
                        if ((line.rfind("FILE", 0) == 0) || (line.rfind("file", 0) == 0)) {
                            Util::replaceAll(line, ",", "-");
                        }
                    }
                    os << line << endl;
                }

                os.flush();
                os.close();
                is.close();

                remove((path + sep + entry.name).c_str());
                rename((path + sep + entry.name + ".new").c_str(),
                       (path + sep + entry.name).c_str());
            }
        }
    }
}

//*******************************
// repairMissingCue
//*******************************
void repairMissingCue(const string & path, const string & folderName) {
    vector<string> binFiles;
    bool hasCue = false;
    DirEntries rootDir = DirEntry::diru_FilesOnly(path);
    for (const DirEntry & entry : rootDir) {
        if (DirEntry::matchExtension(entry.name, EXT_CUE)) {
            hasCue = true;
        }

        if (DirEntry::matchExtension(entry.name, EXT_BIN)) {
            binFiles.push_back(entry.name);
        }
        if (DirEntry::matchExtension(entry.name, EXT_IMG)) {
            binFiles.push_back(entry.name);
        }
    }
    if (!hasCue) {
        string newCueName = path + sep + folderName + EXT_CUE;
        ofstream os;
        os.open(newCueName);
        // let's create new one
        bool first = true;
        int track = 1;
        for (const string & bin : binFiles) {
            string cueElement;
            if (first) {
                cueElement = cue1;
            } else {
                cueElement = cue2;
            }

            Util::replaceAll(cueElement, "{binName}", bin);
            if (track < 10) {
                Util::replaceAll(cueElement, "{track}", "0" + to_string(track));
            } else {
                Util::replaceAll(cueElement, "{track}", to_string(track));
            }
            track++;
            first = false;
            os << cueElement;
        }
        os.flush();
        os.close();
    }
}

                                //*******************************
                                // Scanner
                                // *******************************

//*******************************
// Scanner::moveFolderIfNeeded
//*******************************
void Scanner::moveFolderIfNeeded(const std::string &gameDirName, string gameDataPath, string path) {
    bool gameDataExists = DirEntry::exists(gameDataPath);

    if (gameDataExists) {
        cerr << "Game: " << gameDirName << " - Moving GameData to 0.5" << endl;
        for (const DirEntry & entryGame : DirEntry::diru(gameDataPath)) {
            string newName = path + sep + gameDirName + sep + entryGame.name;
            string oldName = gameDataPath + sep + entryGame.name;
            cerr << "Moving: " << oldName << "  to: " << newName << endl;
            rename(oldName.c_str(), newName.c_str());
        }
    }

    DirEntry::rmDir(gameDataPath);
}

//*******************************
// Scanner::repairBrokenCueFiles
//*******************************
void Scanner::repairBrokenCueFiles(const string & path) {
    vector<string> allBinFiles;
    vector<string> allCues;
    vector<bool> validCue;
    vector<int> cueTracks;

    allBinFiles.clear();
    allCues.clear();
    validCue.clear();
    cueTracks.clear();

    for (const DirEntry & entryGame:DirEntry::diru(path)) {
        if (DirEntry::matchExtension(entryGame.name, EXT_CUE)) {
            allCues.push_back(entryGame.name);
        }

        if (DirEntry::matchExtension(entryGame.name, EXT_BIN)) {
            allBinFiles.push_back(entryGame.name);
        }

        if (DirEntry::matchExtension(entryGame.name, EXT_IMG)) {
            allBinFiles.push_back(entryGame.name);
        }
    }

    for (const string & cue:allCues) {
        ifstream cueStream;

        cueStream.open(path + sep + cue);
        string line;
        bool cueOk = true;
        int bins = 0;
        while (getline(cueStream, line)) {
            line = trim(line);
            if (line.empty()) continue;
            if (line.substr(0, 4) == "FILE") {
                line = line.substr(6, string::npos);
                line = line.substr(0, line.find('"'));
                bins++;
                if (std::find(allBinFiles.begin(), allBinFiles.end(), line) == allBinFiles.end()) {
                    cueOk = false;
                }
            }
        }
        validCue.push_back(cueOk);
        cueTracks.push_back(bins);
        cueStream.close();
    }

    // now we know cues that are corrupted - regenerate them

    int startPos = 0;
    for (int i = 0; i < allCues.size(); i++) {
        bool cueOk = validCue[i];
        string cuePath = path + sep + allCues[i];
        if (!cueOk) {
            remove(cuePath.c_str());

            ofstream os;
            os.open(cuePath);
            // let's create new one
            bool first = true;
            int track = 1;
            for (int binId = 0; binId != cueTracks[i]; binId++) {
                string cueElement;
                if (first) {
                    cueElement = cue1;
                } else {
                    cueElement = cue2;
                }

                string newBinName = "BinDoesNotExists.bin";
                if ((startPos + binId) < allBinFiles.size()) {
                    newBinName = allBinFiles[startPos + binId];
                }

                Util::replaceAll(cueElement, "{binName}", newBinName);
                if (track < 10) {
                    Util::replaceAll(cueElement, "{track}", "0" + to_string(track));
                } else {
                    Util::replaceAll(cueElement, "{track}", to_string(track));
                }
                track++;
                first = false;
                os << cueElement;
            }
            os.flush();
            os.close();
        }
        startPos += cueTracks[i];
    }
}

//*******************************
// Scanner::scanUSBGamesDirectory
//*******************************
void Scanner::scanUSBGamesDirectory(const string &rootPath, GamesHierarchy &gamesHierarchy) {
    // it looks like the USBGames path must have a / at the end for changing the game config to work
    //string rootPathWithOutSeparator = DirEntry::removeSeparatorFromEndOfPath(rootPath);

    gamesToAddToDB.clear();  // clear games list
    complete = false;

    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText(_("Scanning..."));

    if (!DirEntry::exists(rootPath + sep + "!SaveStates")) {
        DirEntry::createDir(rootPath + sep + "!SaveStates");
    }

    if (!DirEntry::exists(rootPath + sep + "!MemCards")) {
        DirEntry::createDir(rootPath + sep + "!MemCards");
    }

    USBGames allGames = gamesHierarchy.getAllGames();

    string badGameFilePath = DirEntry::getWorkingPath() + sep + "gamesThatFailedVerifyCheck.txt";
    ofstream badGameFile;
    badGameFile.open(badGameFilePath.c_str(), ios::binary);

#if 0
    int i = 0;
    for (auto game : gamesScanned) {
        cout << i++ << ": ";
        if (game)
            cout << game->pathName << ", " << game->fullPath << endl;
        else
            cout << "NULL" << endl;
    }
#endif

    for (USBGamePtr game : allGames) {
        int i = 0;
        if (game)
            cout << i++ << ": "<< game->gameDirName << ", " << game->fullPath << endl;
        else
            cout << i++ << ": "<< "NULL" << endl;
        repairBinCommaNames(game->fullPath);

        string saveStateDir = rootPath + sep + "!SaveStates" + sep + game->gameDirName;
        DirEntry::createDir(saveStateDir);
        DirEntry::createDir(saveStateDir + sep + "memcards");

        game->folder_id = 0; // this will not be in use;
        game->saveStatePath = rootPath + sep + "!SaveStates" + sep + game->gameDirName + sep;

        splash->logText(_("Game:") + " " + game->gameDirName);

        string gamePathWithOutSeparator = DirEntry::removeSeparatorFromEndOfPath(game->fullPath);

        moveFolderIfNeeded(game->gameDirName, game->fullPath + sep + GAME_DATA, game->fullPath);

        string gameIniPath = game->fullPath + sep + GAME_INI;

        DirEntries fileEntries = DirEntry::diru_FilesOnly(game->fullPath);    // get the list of files once
        if (DirEntry::thereIsAGameFile(fileEntries)) {
            ImageType imageType;
            string gameFile;
            tie(imageType, gameFile) = DirEntry::getGameFile(fileEntries);
			game->imageType = imageType;
			game->gameDataFound = true;

			if (DirEntry::imageTypeUsesACueFile(imageType))
			{
				repairMissingCue(game->fullPath, game->gameDirName);
				repairBrokenCueFiles(game->fullPath);
				unecm(game->fullPath);
			}

            // for each file in the game dir
			for (const DirEntry & file : fileEntries) {
				if (Util::compareCaseInsensitive(file.name, GAME_INI)) {
                    game->gameIniFound = true;
				}

				if (Util::compareCaseInsensitive(file.name, PCSX_CFG)) {
					game->pcsxCfgFound = true;
				}

				if (DirEntry::matchExtension(file.name, EXT_PNG)) {
					game->coverImageFound = true;
				}

				if (DirEntry::matchExtension(file.name, EXT_LIC)) {
					game->licFound = true;
				}
			}

			cout << "before calling recoverMissingFiles() automationUsed = " << game->automationUsed << endl;
			game->recoverMissingFiles();
            cout << "after calling recoverMissingFiles() automationUsed = " << game->automationUsed << endl;

            if (game->gameIniFound)
                game->readIni(gameIniPath); // read it in now in case we need to create or update the serial/region

            game->serial = SerialScanner::scanSerial(game->imageType, game->fullPath + sep, game->firstBinPath);
            game->region = SerialScanner::serialToRegion(game->serial);
            //cout << "serial: " << game->serial << ", region: " << game->region << ", " << game->title <<endl;

            // if there was no ini file before, get the values for the ini, create the cover file if needed, and create/update the game.ini file
            if ( !game->gameIniFound || game->automationUsed) {

				if (!game->serial.empty()) {
					//cout << "Accessing metadata for serial: " << game->serial << endl;
					Metadata md;
					if (md.lookupBySerial(game->serial)) {
						// at this stage we have more data;
                        if (game->title == "")
						    game->title = md.title;
                        if (game->publisher == "")
                            game->publisher = md.publisher;
                        if (game->players == 0)
						    game->players = md.players;
                        if (game->year == 0)
						    game->year = md.year;

						if (game->discs.size() > 0) {
							// all recovered :)
                            if (!game->coverImageFound) {
                                string newFilename = game->fullPath + sep + game->discs[0].cueName + EXT_PNG;
                                cout << "Updating cover in scanUSBGamesDirectory()" << newFilename << endl;
                                ofstream pngFile;
                                pngFile.open(newFilename);
                                pngFile.write(md.bytes, md.dataSize);
                                pngFile.flush();
                                pngFile.close();
                                game->automationUsed = false;
                                game->coverImageFound = true;
                            }
						}

						md.clean();
					}
					else {
					    if (game->title == "")
						    game->title = game->gameDirName;
					}
				}
			}
            game->saveIni(gameIniPath);
            game->readIni(gameIniPath); // the updated iniValues are needed for updateObj
			//game->print();

			if (game->verify()) {
                gamesToAddToDB.push_back(game);

                string memcardPath = game->saveStatePath + sep + "memcards/";
                if (!DirEntry::exists(memcardPath + "card1.mcd"))
                {
                    DirEntry::copy(DirEntry::getWorkingPath() + sep + "memcard/card1.mcd", memcardPath + "card1.mcd");
                }
                if (!DirEntry::exists(memcardPath + sep + "card2.mcd"))
                {
                    DirEntry::copy(DirEntry::getWorkingPath() + sep + "memcard/card1.mcd", memcardPath + "card2.mcd");
                }
                if (!DirEntry::exists(game->saveStatePath + sep + PCSX_CFG))
                {
                    DirEntry::copy(DirEntry::getWorkingPath() + sep + PCSX_CFG, game->saveStatePath + sep + PCSX_CFG);
                }
                DirEntry::generateM3UForDirectory(game->fullPath, game->discs[0].cueName);
            }
            else {
                badGameFile << "Game failed to verify: " << game->fullPath << endl;

                // the game did not pass the verify step and was not added to the DB.
                // remove the game everywhere in the gamesHierarchy
                gamesHierarchy.removeGameFromEntireHierarchy(game);
            }
		}
	} // end for each game dir

    badGameFile.close();

    USBGame::sortByTitle(gamesToAddToDB);
    gamesHierarchy.makeGamesToDisplayWhileRemovingChildDuplicates();

    gamesHierarchy.printRowDisplayGameInfo(false);

    string path = DirEntry::getWorkingPath() + sep + "gameHierarchy_afterScanAndRemovingDuplicates.txt";
    ofstream outfile;
    outfile.open(path);
    gamesHierarchy.dumpRowGameInfo(outfile, true);
    outfile << endl << endl;
    gamesHierarchy.dumpRowDisplayGameInfo(outfile, true);
    outfile.close();

    complete = true;
}

//*******************************
// Scanner::areThereGameFilesInsDir
//*******************************
bool Scanner::areThereGameFilesInDir(const string & path) {
    vector<string> extensions;
    extensions.push_back("pbp");
    extensions.push_back("bin");
    extensions.push_back("cue");
    extensions.push_back("img");
//    extensions.push_back("iso");

    //Getting all files in USBGames Dir
    DirEntries globalFileList = DirEntry::diru(path);
    DirEntries fileList = DirEntry::getFilesWithExtension(path, globalFileList, extensions);

    return fileList.size() > 0;
}
