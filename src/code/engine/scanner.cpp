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

using namespace std;

//*******************************
// Scanner::isFirstRun
//*******************************
bool Scanner::isFirstRun(const string & path, Database *db) {

    bool listFileExists = Util::exists(Util::getWorkingPath() + Util::separator() + "autobleem.list");
    if (!listFileExists) {
        return true;
    }

    bool prevFileExists = Util::exists(Util::getWorkingPath() + Util::separator() + "autobleem.prev");
    if (!prevFileExists) {
        return true;
    }
    ifstream prev;
    string prevName = Util::getWorkingPath() + Util::separator() + "autobleem.prev";
    prev.open(prevName.c_str(), ios::binary);
    vector<DirEntry> entries = Util::diru_DirsOnly(path);
    noGamesFound = true;
    for (const DirEntry & entry:entries) {
        if (entry.name == "!SaveStates") continue;
        if (entry.name == "!MemCards") continue;
        noGamesFound = false;
        string nameInFile;
        getline(prev, nameInFile);
        if (nameInFile != entry.name) {
            return true;
        }
    }
    prev.close();

    return false;
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
    for (const DirEntry & entry: Util::dir(path)) {
        if (entry.name[0] == '.') continue;
        if (Util::matchExtension(entry.name, EXT_ECM)) {
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
    string path = Util::getWorkingPath() + Util::separator() + "autobleem.list";
    ofstream outfile;
    outfile.open(path);
    if (complete)
        for (int i = 0; i < games.size(); i++) {
            shared_ptr<Game> data = games[i];
            cout << "Inserting game ID: " << i + 1 << " - " << data->title << endl;
            db->insertGame(i + 1, data->title, data->publisher, data->players, data->year, data->fullPath,
                           data->saveStatePath, data->memcard);
            if (data->discs.size() == 0)
                cout << "No discs in game: " << data->title << endl;
            for (int j = 0; j < data->discs.size(); j++) {
                db->insertDisc(i + 1, j + 1, data->discs[j].diskName);
            }
            outfile << i + 1 << "," << Util::escape(data->fullPath.substr(0, data->fullPath.size() - 1)) << ","
                    << Util::escape(data->saveStatePath.substr(0, data->saveStatePath.size() - 1)) << endl;
        }
    outfile.flush();
    outfile.close();
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
    for (DirEntry entry : Util::diru(path)) {
        if (entry.name.find(",") != string::npos) {
            string newName = entry.name;
            Util::replaceAll(newName, ",", "-");
            rename((path + entry.name).c_str(), (path + newName).c_str());
            entry.name = newName;
            if (Util::matchExtension(entry.name, EXT_CUE)) {
                // process cue inside
                ifstream is(path + Util::separator() + entry.name);
                ofstream os(path + Util::separator() + entry.name + ".new");
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

                remove((path + Util::separator() + entry.name).c_str());
                rename((path + Util::separator() + entry.name + ".new").c_str(),
                       (path + Util::separator() + entry.name).c_str());
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
    vector<DirEntry> rootDir = Util::dir(path);
    for (const DirEntry & entry : rootDir) {
        if (entry.name[0] == '.') continue;

        if (Util::matchExtension(entry.name, EXT_CUE)) {
            hasCue = true;
        }

        if (Util::matchExtension(entry.name, EXT_BIN)) {
            binFiles.push_back(entry.name);
        }
    }
    if (!hasCue) {
        string newCueName = path + folderName + EXT_CUE;
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
void Scanner::moveFolderIfNeeded(const DirEntry & entry, string gameDataPath, string path) {
    bool gameDataExists = Util::exists(gameDataPath);

    if (gameDataExists) {
        cerr << "Game: " << entry.name << " - Moving GameData to 0.5" << endl;
        for (const DirEntry & entryGame : Util::diru(gameDataPath)) {
            string newName = path + entry.name + Util::separator() + entryGame.name;
            string oldName = gameDataPath + entryGame.name;
            cerr << "Moving: " << oldName << "  to: " << newName << endl;
            rename(oldName.c_str(), newName.c_str());
        }
    }

    Util::rmDir(gameDataPath);
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

    for (const DirEntry & entryGame:Util::diru(path)) {
        if (Util::matchExtension(entryGame.name, EXT_CUE)) {
            allCues.push_back(entryGame.name);
        }

        if (Util::matchExtension(entryGame.name, EXT_BIN)) {
            allBinFiles.push_back(entryGame.name);
        }

        if (Util::matchExtension(entryGame.name, EXT_IMG)) {
            allBinFiles.push_back(entryGame.name);
        }
    }

    for (const string & cue:allCues) {
        ifstream cueStream;

        cueStream.open(path + Util::separator() + cue);
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
        string cuePath = path + Util::separator() + allCues[i];
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
// Scanner::getImageType
//*******************************
int Scanner::getImageType(string path) {
    bool hasASubDir {false};
    for (const DirEntry & entry: Util::diru(path)) {
        if (entry.isDir)
            hasASubDir = true;
        else { // it's a file
            if (Util::matchExtension(entry.name, EXT_BIN)) {
                return IMAGE_CUE_BIN;
            }
            if (Util::matchExtension(entry.name, EXT_PBP)) {
                return IMAGE_PBP;
            }
        }
    }
    if (hasASubDir)
        return IMAGE_NO_GAME_BUT_HAS_SUBDIR;
    else
        return IMAGE_NO_GAME_FOUND;
}

//*******************************
// Scanner::scanDirectory
//*******************************
void Scanner::scanDirectory(const string & _path) {
    string path = Util::pathWithSeparatorAtEnd(_path); // it looks like the Games path must have a / at the end for changing the game conig to work

    games.clear();  // clear games list
    complete = false;

    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText(_("Scanning..."));

    ofstream prev;
    string prevFileName = Util::getWorkingPath() + Util::separator() + "autobleem.prev";
    prev.open(prevFileName.c_str(), ios::binary);

    if (!Util::exists(path + "!SaveStates")) {
        Util::createDir(path + "!SaveStates");
    }

    if (!Util::exists(path + "!MemCards")) {
        Util::createDir(path + "!MemCards");
    }

    // for each game dir
    for (DirEntry entry: Util::diru_DirsOnly(path)) {
        if (entry.name[0] == '.') continue;
        if (entry.name == "!SaveStates") continue;
        if (entry.name == "!MemCards") continue;

        // fix for comma in dirname
        if (entry.name.find(",") != string::npos) {
            string newName = entry.name;
            Util::replaceAll(newName, ",", "-");
            rename((path + entry.name).c_str(), (path + newName).c_str());
            entry.name = newName;
        }

        repairBinCommaNames(path + entry.name + Util::separator());
        prev << entry.name << endl;

        string saveStateDir = path + "!SaveStates" + Util::separator() + entry.name;
        Util::createDir(saveStateDir);

        shared_ptr<Game> game{new Game};

        game->folder_id = 0; // this will not be in use;
        game->fullPath = path + entry.name + Util::separator();
        game->saveStatePath = path + "!SaveStates" + Util::separator() + entry.name + Util::separator();

        game->pathName = entry.name;
        splash->logText(_("Game:") + " " + entry.name);

        string gameDataPath = path + entry.name + Util::separator() + GAME_DATA + Util::separator();

        moveFolderIfNeeded(entry, gameDataPath, path);
        gameDataPath = path + entry.name + Util::separator();

        string gameIniPath = gameDataPath + GAME_INI;

        int gameType = getImageType(gameDataPath);
        if (gameType == IMAGE_CUE_BIN || gameType == IMAGE_PBP) {
			game->imageType = gameType;
			game->gameDataFound = true;

			if (game->imageType == IMAGE_CUE_BIN) // only cue/bin
			{
				repairMissingCue(gameDataPath, entry.name);
				repairBrokenCueFiles(gameDataPath);
				unecm(gameDataPath);
			}

            // for each file in the game dir
			for (const DirEntry & file : Util::diru(gameDataPath)) {
				if (Util::matchesLowercase(file.name, GAME_INI)) {
                    game->gameIniFound = true;
				}

				if (Util::matchesLowercase(file.name, PCSX_CFG)) {
					game->pcsxCfgFound = true;
				}

				if (Util::matchExtension(file.name, EXT_PNG)) {
					game->imageFound = true;
				}

				if (Util::matchExtension(file.name, EXT_LIC)) {
					game->licFound = true;
				}
			}

			cout << "before calling recoverMissingFiles() automationUsed = " << game->automationUsed << endl;
			game->recoverMissingFiles();
            cout << "after calling recoverMissingFiles() automationUsed = " << game->automationUsed << endl;

            if (game->gameIniFound)
                game->readIni(gameIniPath); // read it in now in case we need to create or update the serial/region

            game->serial = SerialScanner::scanSerial(game->imageType, game->fullPath, game->firstBinPath);
            game->region = SerialScanner::serialToRegion(game->serial);
            //cout << "serial: " << game->serial << ", region: " << game->region << ", " << game->title <<endl;

            // if there was no ini file before, get the values for the ini, create the cover file if needed, and create/update the game.ini file
            if ( !game->gameIniFound || game->automationUsed) {

				if (!game->serial.empty()) {
					//cout << "Accessing metadata for serial: " << game->serial << endl;
					Metadata md;
					if (md.lookupBySerial(game->serial)) {
						// at this stage we have more data;
						game->title = md.title;
						game->publisher = md.publisher;
						game->players = md.players;
						game->year = md.year;

						if (game->discs.size() > 0) {
							// all recovered :)

							string newFilename = gameDataPath + game->discs[0].cueName + EXT_PNG;
							//cout << "Updating cover" << newFilename << endl;
							ofstream pngFile;
							pngFile.open(newFilename);
							pngFile.write(md.bytes, md.dataSize);
							pngFile.flush();
							pngFile.close();
							game->automationUsed = false;
							game->imageFound = true;
						}

						md.clean();
					}
					else {
						game->title = game->pathName;
					}
				}
			}
            game->saveIni(gameIniPath);
            game->readIni(gameIniPath); // the updated iniValues are needed for updateObj
			//game->print();

			if (game->verify())
				games.push_back(game);
            else
                cout << "game: " << game->title << " did not pass verify() test" << endl;
		}
	} // end for each game dir

    prev.flush();
    prev.close();
    sort(games.begin(), games.end(), sortByTitle);

    complete = true;
}

//*******************************
// Scanner::detectAndSortGamefiles
//*******************************
/*
 * Searching for games with supported extension and create associated folders
 */
void Scanner::detectAndSortGamefiles(const string & path){
    string fileExt;
    string filenameWE;
    vector<string> extensions;
    vector<string> binList;
    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText(_("Sorting..."));
    extensions.push_back("iso");
    extensions.push_back("pbp");
    extensions.push_back("cue");

    //Getting all files in Games Dir
    vector<DirEntry> globalFileList = Util::diru(path);
    vector<DirEntry> fileList = Util::getFilesWithExtension(path, globalFileList, extensions);

    //On first run, we won't process bin/img files, as cue file may handle a part of them
    for (const auto &entry : fileList){
        splash->logText(_("Sorting : ") + entry.name);
        fileExt = Util::getFileExtension(entry.name);
        filenameWE = Util::getFileNameWithoutExtension(entry.name);
        //Checking if file exists
        if(access((path + "/" + entry.name).c_str(),F_OK) != -1){
            if(fileExt == "cue"){
                binList = Util::cueToBinList(path + "/" + entry.name);
                if(!binList.empty()){
                    //Create directory for game
                    Util::createDir(path + "/" + filenameWE);
                    //Move cue file
                    rename((path + "/" + entry.name).c_str(), (path + "/" + filenameWE + "/" + entry.name).c_str());
                    //Move bin files
                    for (const auto &bin : binList){
                        splash->logText(_("Sorting : ")+bin);
                        rename((path + "/" + bin).c_str(), (path + "/" + filenameWE + "/" + bin).c_str());
                    }
                }
            }else{
                Util::createDir(path + "/" + filenameWE);

                rename((path + "/" + entry.name).c_str(),(path + "/" + filenameWE + "/" + entry.name).c_str());
            }
        }
    }

    //Next we will read only bin and img files
    extensions.clear();
    extensions.push_back("img");
    extensions.push_back("bin");
    fileList = Util::getFilesWithExtension(path, globalFileList, extensions);
    for (const auto &entry : fileList){
        splash->logText(_("Sorting : ") + entry.name);
        fileExt = Util::getFileExtension(entry.name);
        filenameWE = Util::getFileNameWithoutExtension(entry.name);
        //Checking if file exists
        if(access((path + "/" + entry.name).c_str(),F_OK) != -1){
            Util::createDir(path + "/" + filenameWE);
            rename((path + "/" + entry.name).c_str(), (path + "/" + filenameWE + "/" + entry.name).c_str());
        }
    }
}