//
// Created by screemer on 2018-12-15.
//

#include "scanner.h"
#include "ecmhelper.h"


bool wayToSort(Game i, Game j) {
    string name1=i.title;
    string name2=j.title;
    lcase(name1);
    lcase(name2);
    return name1 < name2;
}

bool Scanner::isFirstRun(string path, Database *db) {

    bool listFile = !Util::exists(Util::getWorkingPath() + Util::separator() + "autobleem.list");
    if (listFile) {
        return listFile;
    }


    bool prevFileExists = Util::exists(Util::getWorkingPath() + Util::separator() + "autobleem.prev");
    if (!prevFileExists) {
        return true;
    }
    ifstream prev;
    string prevName = Util::getWorkingPath() + Util::separator() + "autobleem.prev";
    prev.open(prevName.c_str(), ios::binary);
    vector<DirEntry> entries = Util::diru(path);
    for (DirEntry entry:entries) {
        if (entry.name == "!SaveStates") continue;
        if (entry.name == "!MemCards") continue;
        string nameInFile;
        getline(prev, nameInFile);
        if (nameInFile != entry.name) {
            return true;
        }
    }
    prev.close();

    return false;

}

void Scanner::unecm(string path) {
    for (DirEntry entry: Util::dir(path)) {
        if (entry.name[0] == '.') continue;
        if (Util::matchExtension(entry.name, EXT_ECM)) {
            Ecmhelper ecm;
            shared_ptr<Gui> splash(Gui::getInstance());
            splash->logText("Decompressing ecm:");
            if (ecm.unecm(path + entry.name, path + entry.name.substr(0, entry.name.length() - 4))) {

                remove((path + entry.name).c_str());

            }
        }

    }
}

void Scanner::updateDB(Database *db) {
    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText("Updating regional.db...");
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
            outfile << i + 1 << "," << Util::escape(data.fullPath.substr(0, data.fullPath.size() - 1)) << ","
                    << Util::escape(data.saveStatePath.substr(0, data.saveStatePath.size() - 1)) << endl;

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

void repairBinCommaNames(string path) {
    // TODO: Add support for German diactrics for nex here
    for (DirEntry entry:Util::diru(path)) {
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

void repairMissingCue(string path, string folderName) {
    vector<string> binFiles;
    bool hasCue = false;
    vector<DirEntry> rootDir = Util::dir(path);
    for (DirEntry entry:rootDir) {
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
        for (string bin:binFiles) {
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

void Scanner::moveFolderIfNeeded(DirEntry entry, string gameDataPath, string path) {
    bool gameDataExists = Util::exists(gameDataPath);

    if (gameDataExists) {
        cerr << "Game: " << entry.name << " - Moving GameData to 0.5" << endl;
        for (DirEntry entryGame:  Util::diru(gameDataPath)) {
            string newName = path + entry.name + Util::separator() + entryGame.name;
            string oldName = gameDataPath + entryGame.name;
            cerr << "Moving: " << oldName << "  to: " << newName << endl;
            rename(oldName.c_str(), newName.c_str());
        }
    }

    Util::rmDir(gameDataPath);


}

void Scanner::repairBrokenCueFiles(string path) {
    vector<string> allBinFiles;
    vector<string> allCues;
    vector<bool> validCue;
    vector<int> cueTracks;

    allBinFiles.clear();
    allCues.clear();
    validCue.clear();
    cueTracks.clear();

    for (DirEntry entryGame:Util::diru(path)) {


        if (Util::matchExtension(entryGame.name, EXT_CUE)) {
            allCues.push_back(entryGame.name);
        }

        if (Util::matchExtension(entryGame.name, EXT_BIN)) {
            allBinFiles.push_back(entryGame.name);
        }
    }

        for (string cue:allCues) {
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
        for (int i=0;i<allCues.size();i++) {
            bool cueOk = validCue[i];
            string cuePath = path+Util::separator()+allCues[i];
            if (!cueOk)
            {
                remove(cuePath.c_str());

                ofstream os;
                os.open(cuePath);
                // let's create new one
                bool first = true;
                int track = 1;
                for (int binId=0;binId!=cueTracks[i];binId++) {
                    string cueElement;
                    if (first) {
                        cueElement = cue1;
                    } else {
                        cueElement = cue2;
                    }

                    string newBinName = "BinDoesNotExists.bin";
                    if ((startPos+binId)<allBinFiles.size())
                    {
                        newBinName =  allBinFiles[startPos+binId];
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
            startPos+=cueTracks[i];



    }


}

int Scanner::getImageType(string path) {
    for (DirEntry entry: Util::diru(path)) {
        if (Util::matchExtension(entry.name, EXT_BIN)) {
            return 0;
        }
        if (Util::matchExtension(entry.name, EXT_PBP)) {
            return 1;
        }

    }
    return 0;
}

void Scanner::scanDirectory(string path) {
    // clear games list

    games.clear();
    complete = false;
    shared_ptr<Gui> splash(Gui::getInstance());
    splash->logText("Scanning...");

    ofstream prev;
    string prevFileName = Util::getWorkingPath() + Util::separator() + "autobleem.prev";
    prev.open(prevFileName.c_str(), ios::binary);

    if (!Util::exists(path + Util::separator() + "!SaveStates")) {
        Util::createDir(path + Util::separator() + "!SaveStates");
    }

    if (!Util::exists(path + Util::separator() + "!MemCards")) {
        Util::createDir(path + Util::separator() + "!MemCards");
    }

    for (DirEntry entry: Util::diru(path)) {
        if (entry.name[0] == '.') continue;
        if (!Util::isDirectory(path+Util::separator()+entry.name)) continue;
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

        string saveStateDir = path + Util::separator() + "!SaveStates" + Util::separator() + entry.name;
        Util::createDir(saveStateDir);

        Game game;

        game.folder_id = 0; // this will not be in use;
        game.fullPath = path + entry.name + Util::separator();
        game.saveStatePath = path + "!SaveStates" + Util::separator() + entry.name + Util::separator();

        game.pathName = entry.name;
        splash->logText("Game: " + entry.name);

        string gameDataPath = path + entry.name + Util::separator() + GAME_DATA + Util::separator();

        moveFolderIfNeeded(entry, gameDataPath, path);
        gameDataPath = path + entry.name + Util::separator();

        game.imageType = this->getImageType(gameDataPath);
        game.gameDataFound = true;

        if (game.imageType == 0) // only cue/bin
        {

            repairMissingCue(gameDataPath, entry.name);
            repairBrokenCueFiles(gameDataPath);
            unecm(gameDataPath);
        }


        if (!Util::exists(gameDataPath + GAME_INI)) {
            game.readIni(gameDataPath + GAME_INI);
            game.gameIniFound = false;
        } else {
            game.gameIniFound = true;
        }

        for (DirEntry entryGame:Util::diru(gameDataPath)) {

            if (Util::matchesLowercase(entryGame.name, GAME_INI)) {
                string gameIniPath = gameDataPath + GAME_INI;
                game.readIni(gameIniPath);
            }

            if (Util::matchesLowercase(entryGame.name, PCSX_CFG)) {
                game.pcsxCfgFound = true;
            }

            if (Util::matchExtension(entryGame.name, EXT_PNG)) {
                game.imageFound = true;
            }

            if (Util::matchExtension(entryGame.name, EXT_LIC)) {
                game.licFound = true;
            }


        }

        game.recoverMissingFiles();

        if (!game.gameIniFound || game.automationUsed) {
            string serial = game.scanSerial();
            if (!serial.empty()) {
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
                        string newFilename = gameDataPath + game.discs[0].cueName + EXT_PNG;
                        cout << "Updating cover" << newFilename << endl;
                        ofstream pngFile;
                        pngFile.open(newFilename);
                        pngFile.write(md.bytes, md.dataSize);
                        pngFile.flush();
                        pngFile.close();
                        game.automationUsed = false;
                        game.imageFound = true;

                    }

                    md.clean();
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

    prev.flush();
    prev.close();
    sort(games.begin(), games.end(), wayToSort);

    complete = true;
}