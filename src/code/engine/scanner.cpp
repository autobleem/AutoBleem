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
// Scanner::gamesDoNotMatchAutobleemPrev
//*******************************
bool Scanner::gamesDoNotMatchAutobleemPrev(const USBGames &_allGames, const std::string & autobleemPrevPath) {
    auto allGames = _allGames;
    sortByFullPath(allGames);
cout << "gamesDoNotMatchAutobleemPrev" << endl;
for (const auto &g : allGames) cout << g->fullPath << endl;

    ifstream prev;
    prev.open(autobleemPrevPath.c_str(), ios::binary);
    for (const auto game : allGames) {
        string pathInFile;
        getline(prev, pathInFile);
cout << "compare " << pathInFile << " ======== " << game->fullPath << endl;
        if (pathInFile != game->fullPath) {
cout << "compare failed" << endl;
            return true;    // the autobleem.prev file does not match
        }
    }
    prev.close();

    return false;
}

//*******************************
// Scanner::writeAutobleemPrev
//*******************************
void Scanner::writeAutobleemPrev(const USBGames &_allGames, const std::string & autobleemPrevPath) {
    auto allGames = _allGames;
    sortByFullPath(allGames);
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
            if (ecm.unecm(path + entry.name, path + entry.name.substr(0, entry.name.length() - 4))) {
                remove((path + entry.name).c_str());
            }
        }
    }
}

//*******************************
// Scanner::updateDB
//*******************************
void Scanner::updateDB(Database *db) {
    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText(_("Updating regional.db..."));
//    string path = Util::getWorkingPath() + DirEntry::separator() + "autobleem.list";
//    ofstream outfile;
//    outfile.open(path);
    if (complete)
        for (int i = 0; i < gamesToAddToDB.size(); i++) {
            USBGamePtr data = gamesToAddToDB[i];
            cout << "Inserting game ID: " << i + 1 << " - " << data->title << endl;
            db->insertGame(i + 1, data->title, data->publisher, data->players, data->year, data->fullPath,
                           data->saveStatePath, data->memcard);
            if (data->discs.size() == 0)
                cout << "No discs in game: " << data->title << endl;
            for (int j = 0; j < data->discs.size(); j++) {
                db->insertDisc(i + 1, j + 1, data->discs[j].diskName);
            }
//            outfile << i + 1 << "," << Util::escape(data->fullPath.substr(0, data->fullPath.size() - 1)) << ","
//                    << Util::escape(data->saveStatePath.substr(0, data->saveStatePath.size() - 1)) << '\n';
        }
//    outfile.flush();
//    outfile.close();
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
    for (DirEntry entry : DirEntry::diru(path)) {
        if (entry.name.find(",") != string::npos) {
            string newName = entry.name;
            Util::replaceAll(newName, ",", "-");
            rename((path + entry.name).c_str(), (path + newName).c_str());
            entry.name = newName;
            if (DirEntry::matchExtension(entry.name, EXT_CUE)) {
                // process cue inside
                ifstream is(path + DirEntry::separator() + entry.name);
                ofstream os(path + DirEntry::separator() + entry.name + ".new");
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

                remove((path + DirEntry::separator() + entry.name).c_str());
                rename((path + DirEntry::separator() + entry.name + ".new").c_str(),
                       (path + DirEntry::separator() + entry.name).c_str());
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
    DirEntries rootDir = DirEntry::dir(path);
    for (const DirEntry & entry : rootDir) {
        if (entry.name[0] == '.') continue;

        if (DirEntry::matchExtension(entry.name, EXT_CUE)) {
            hasCue = true;
        }

        if (DirEntry::matchExtension(entry.name, EXT_BIN)) {
            binFiles.push_back(entry.name);
        }
    }
    if (!hasCue) {
        string newCueName = DirEntry::pathWithSeparatorAtEnd(path) + folderName + EXT_CUE;
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
            string newName = path + gameDirName + DirEntry::separator() + entryGame.name;
            string oldName = gameDataPath + entryGame.name;
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

        cueStream.open(DirEntry::pathWithSeparatorAtEnd(path) + cue);
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
        string cuePath = DirEntry::pathWithSeparatorAtEnd(path) + allCues[i];
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
void Scanner::scanUSBGamesDirectory(const string & _path, const GameSubDirRows &gameSubDirRows) {
    // it looks like the USBGames path must have a / at the end for changing the game config to work
    string rootPathWithSeparator = DirEntry::pathWithSeparatorAtEnd(_path);
    string rootPathWithOutSeparator = DirEntry::pathWithOutSeparatorAtEnd(_path);

    gamesToAddToDB.clear();  // clear games list
    complete = false;

    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText(_("Scanning..."));

    if (!DirEntry::exists(rootPathWithSeparator + "!SaveStates")) {
        DirEntry::createDir(rootPathWithSeparator + "!SaveStates");
    }

    if (!DirEntry::exists(rootPathWithSeparator + "!MemCards")) {
        DirEntry::createDir(rootPathWithSeparator + "!MemCards");
    }

    auto gamesScanned = gameSubDirRows[0]->allGames;

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

    for (USBGamePtr game : gamesScanned) {
        int i = 0;
        if (game)
            cout << i++ << ": "<< game->pathName << ", " << game->fullPath << endl;
        else
            cout << i++ << ": "<< "NULL" << endl;
        repairBinCommaNames(game->fullPath + DirEntry::separator());

        string saveStateDir = rootPathWithSeparator + "!SaveStates" + DirEntry::separator() + game->pathName;
        DirEntry::createDir(saveStateDir);

        //USBGamePtr game{new USBGame};

        game->folder_id = 0; // this will not be in use;
//        game->fullPath = DirEntry::pathWithSeparatorAtEnd(game->fullPath);
        game->saveStatePath = rootPathWithSeparator + "!SaveStates" + DirEntry::separator() + game->pathName + DirEntry::separator();

        //game->pathName = entry.name;
        splash->logText(_("Game:") + " " + game->pathName);

        string gamePathWithSeparator = DirEntry::pathWithSeparatorAtEnd(game->fullPath);
        string gamePathWithOutSeparator = DirEntry::pathWithOutSeparatorAtEnd(game->fullPath);

        string gameDataPath = gamePathWithSeparator + GAME_DATA + DirEntry::separator();

        moveFolderIfNeeded(game->pathName, gameDataPath, gamePathWithSeparator);
        gameDataPath = gamePathWithSeparator;

        string gameIniPath = gameDataPath + GAME_INI;

        DirEntries fileEntries = DirEntry::diru_FilesOnly(gameDataPath);    // get the list of files once
        if (DirEntry::thereIsAGameFile(fileEntries)) {
            ImageType imageType;
            string gameFile;
            tie(imageType, gameFile) = DirEntry::getGameFile(fileEntries);
			game->imageType = imageType;
			game->gameDataFound = true;

			if (DirEntry::imageTypeUsesACueFile(imageType))
			{
				repairMissingCue(gameDataPath, game->pathName);
				repairBrokenCueFiles(gameDataPath);
				unecm(gameDataPath);
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
					game->imageFound = true;
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

            game->serial = SerialScanner::scanSerial(game->imageType, DirEntry::pathWithSeparatorAtEnd(game->fullPath), game->firstBinPath);
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
                            if (!game->imageFound) {
                                string newFilename = gameDataPath + game->discs[0].cueName + EXT_PNG;
                                cout << "Updating cover" << newFilename << endl;
                                ofstream pngFile;
                                pngFile.open(newFilename);
                                pngFile.write(md.bytes, md.dataSize);
                                pngFile.flush();
                                pngFile.close();
                                game->automationUsed = false;
                                game->imageFound = true;
                            }
						}

						md.clean();
					}
					else {
					    if (game->title == "")
						    game->title = game->pathName;
					}
				}
			}
            game->saveIni(gameIniPath);
            game->readIni(gameIniPath); // the updated iniValues are needed for updateObj
			//game->print();

			if (game->verify())
                gamesToAddToDB.push_back(game);
            else
                cout << "game: " << game->title << " did not pass verify() test" << endl;
		}
	} // end for each game dir

    sortByTitle(gamesToAddToDB);

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

//*******************************
// Scanner::copyGameFilesInGamesDirToSubDirs
//*******************************
// Search for games with supported extension and move to sub-dir
// returns true is any files moved into sub-dirs
bool Scanner::copyGameFilesInGamesDirToSubDirs(const string & path){
    bool ret = false;
    string fileExt;
    string filenameWE;
    vector<string> extensions;
    vector<string> binList;
    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText(_("Moving..."));
//    extensions.push_back("iso");
    extensions.push_back("pbp");
    extensions.push_back("cue");

    //Getting all files in USBGames Dir
    DirEntries globalFileList = DirEntry::diru(path);
    DirEntries fileList = DirEntry::getFilesWithExtension(path, globalFileList, extensions);

    //On first run, we won't process bin/img files, as cue file may handle a part of them
    for (const auto &entry : fileList){
        splash->logText(_("Moving :") + " " + entry.name);
        fileExt = DirEntry::getFileExtension(entry.name);
        filenameWE = DirEntry::getFileNameWithoutExtension(entry.name);
        //Checking if file exists
        if(access((path + "/" + entry.name).c_str(),F_OK) != -1){
            if(fileExt == "cue"){
                binList = DirEntry::cueToBinList(path + "/" + entry.name);
                if(!binList.empty()){
                    //Create directory for game
                    DirEntry::createDir(path + "/" + filenameWE);
                    //Move cue file
                    rename((path + "/" + entry.name).c_str(), (path + "/" + filenameWE + "/" + entry.name).c_str());
                    //Move bin files
                    for (const auto &bin : binList){
                        splash->logText(_("Moving :") + " " + bin);
                        rename((path + "/" + bin).c_str(), (path + "/" + filenameWE + "/" + bin).c_str());
                    }
                    ret = true;
                }
            }else{
                DirEntry::createDir(path + "/" + filenameWE);

                rename((path + "/" + entry.name).c_str(),(path + "/" + filenameWE + "/" + entry.name).c_str());
                ret = true;
            }
        }
    }

    //Next we will read only bin and img files
    extensions.clear();
    extensions.push_back("img");
    extensions.push_back("bin");
    fileList = DirEntry::getFilesWithExtension(path, globalFileList, extensions);
    for (const auto &entry : fileList){
        splash->logText(_("Moving :") + " " + entry.name);
        fileExt = DirEntry::getFileExtension(entry.name);
        filenameWE = DirEntry::getFileNameWithoutExtension(entry.name);
        //Checking if file exists
        if(access((path + "/" + entry.name).c_str(),F_OK) != -1){
            DirEntry::createDir(path + "/" + filenameWE);
            rename((path + "/" + entry.name).c_str(), (path + "/" + filenameWE + "/" + entry.name).c_str());
            ret = true;
        }
    }
    return ret; // true if any game files moved into a sub-dir
}