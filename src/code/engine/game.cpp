//
// Created by screemer on 2018-12-15.
//
#include "game.h"
#include "metadata.h"
#include "isodir.h"
#include "inifile.h"
#include "cfgprocessor.h"
#include "../gui/gui.h"
#include "serialscanner.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include "../engine/scanner.h"

using namespace std;

//*******************************
// Game::validateCue
//*******************************
bool Game::validateCue(string cuePath, string path) {
    vector<string> binFiles;
    string line;
    ifstream cueStream;
    bool result = true;

    cueStream.open(cuePath);
    while (getline(cueStream, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line.substr(0, 4) == "FILE") {
            line = line.substr(6, string::npos);
            line = line.substr(0, line.find('"'));
            binFiles.push_back(line);
        }
    }
    for (int i = 0; i < binFiles.size(); i++) {
        string binPath = path + binFiles[i];
        if (!Util::exists(binPath)) {
            result = false;
        } else {
            if (i == 0) {
                if (firstBinPath.empty()) {
                    firstBinPath = binPath;
                }
            }
        }
    }
    cueStream.close();
    return result;
}

//*******************************
// Game::valueOrDefault
//*******************************
string Game::valueOrDefault(string name, string def) {
    string value;
    if (iniValues.find(name) != iniValues.end()) {
        value = trim(iniValues.find(name)->second);
        if (value.length() == 0) {
            automationUsed = true;
            return def;
        }
    } else {
        automationUsed = true;
        value = def;
    }
    return value;
}

//*******************************
// Game::verify
//*******************************
bool Game::verify() {
    bool result = true;

    if (discs.size() == 0) result = false;

    for (int i = 0; i < discs.size(); i++) {
        if (discs[i].diskName.length() == 0) result = false;
        if (!discs[i].cueFound) result = false;
        if (!discs[i].binVerified) result = false;
    }

    if (!gameDataFound) result = false;
    if (!gameIniFound) result = false;
    if (!gameIniValid) result = false;
    if (!imageFound) result = false;
    if (!licFound) result = false;
    if (!pcsxCfgFound) result = false;

    if (!result) {
        cerr << "Game: " << title << " Validation Failed" << endl;
    }

    return result;
}

//*******************************
// Game::print
//*******************************
bool Game::print() {
    cout << "-------------------" << endl;
    cout << "Printing game data:" << endl;
    cout << "-----------------" << endl;
    cout << "AUTOMATION: " << automationUsed << endl;
    cout << "Game folder id: " << folder_id << endl;
    cout << "Game: " << title << endl;
    cout << "Players: " << players << endl;
    cout << "Publisher: " << publisher << endl;
    cout << "Year: " << year << endl;
    cout << "Serial: " << serial << endl;
    cout << "Region: " << region << endl;
    cout << "GameData found: " << gameDataFound << endl;
    cout << "Game.ini found: " << gameIniFound << endl;
    cout << "Game.ini valid: " << gameIniValid << endl;
    cout << "PNG found:" << imageFound << endl;
    cout << "LIC found:" << licFound << endl;
    cout << "pcsx.cfg found: " << pcsxCfgFound << endl;
    cout << "TotalDiscs: " << discs.size() << endl;
    cout << "Favorite: " << favorite << endl;

    for (int i = 0; i < discs.size(); i++) {
        cout << "  Disc:" << i + 1 << "  " << discs[i].diskName << endl;
        cout << "  CUE found: " << discs[i].cueFound << endl;
        cout << "  BIN correct: " << discs[i].binVerified << endl;
    }

    bool result = verify();
    if (result) {
        cout << "-------OK-------" << endl;
    } else {
        cout << "------FAIL------" << endl;
    }

    return result;
}

//*******************************
// Game::recoverMissingFiles
//*******************************
void Game::recoverMissingFiles() {
    string path = Util::getWorkingPath();

    Metadata md;
    bool metadataLoaded = false;

    if (this->imageType == IMAGE_PBP) {
        // disc link
        string destinationDir = fullPath ;
        string pbpFileName = Util::findFirstFile(EXT_PBP, destinationDir);
        if (pbpFileName != "") {
            if (discs.size() == 0) {
                automationUsed = false;
                Disc disc;
                disc.diskName = pbpFileName;
                disc.cueFound = true;
                disc.cueName = pbpFileName;
                disc.binVerified = true;
                discs.push_back(disc);
            }
        } else
        {
            automationUsed = true;
            cout << "Switching automation in PBP" << endl;
        }
    }
    if (this->imageType == IMAGE_CUE_BIN) {
        if (discs.size() == 0) {
            automationUsed = true;
            cout << "Switching automation no discs" << endl;
            // find cue files
            string destination = fullPath ;
            for (const DirEntry & entry: Util::diru(destination)) {
                if (Util::matchExtension(entry.name, EXT_CUE)) {
                    string discEntry = entry.name.substr(0, entry.name.size() - 4);
                    Disc disc;
                    disc.diskName = discEntry;
                    disc.cueFound = true;
                    disc.cueName = discEntry;
                    disc.binVerified = validateCue(destination + entry.name, fullPath );
                    discs.push_back(disc);
                }
            }
        }
    }

    if (discs.size() > 0) {
        if (!licFound) {
            automationUsed = true;
            cout << "Switching automation no lic" << endl;
            string source = path + Util::separator() + "default.lic";
            string destination = fullPath  + discs[0].diskName + ".lic";
            cerr << "SRC:" << source << " DST:" << destination << endl;
            Util::copy(source, destination);
            licFound = true;
        }
        if (!imageFound) {
            automationUsed = true;
            cout << "Switching automation no image" << endl;
            string source = path + Util::separator() + "default.png";
            string destination = fullPath  + discs[0].diskName + ".png";
            cerr << "SRC:" << source << " DST:" << destination << endl;
            Util::copy(source, destination);
            // maybe we can do better ?
            string serial = SerialScanner::scanSerial(imageType,fullPath,firstBinPath);
            if (serial != "") {

                if (md.lookupBySerial(serial)) {
                    metadataLoaded = true;
                    //cout << "Updating cover" << destination << endl;
                    ofstream pngFile;
                    pngFile.open(destination);
                    pngFile.write(md.bytes, md.dataSize);
                    pngFile.flush();
                    pngFile.close();
                    automationUsed = false;
                    imageFound = true;
                };
                md.clean();

            }
            imageFound = true;

        }
    }

    if (!pcsxCfgFound) {
        automationUsed = true;
        cout << "Switching automation no pcsx" << endl;
        string source = path + Util::separator() + "pcsx.cfg";
        string destination = fullPath + "pcsx.cfg";
        cerr << "SRC:" << source << " DST:" << destination << endl;

        int region = 0;
        bool japan = false;

        if (!metadataLoaded) {
            string serial = SerialScanner::scanSerial(imageType,fullPath,firstBinPath);
            if (serial != "") {
                metadataLoaded = md.lookupBySerial(serial);
            }
        }

        if (metadataLoaded) {
            if (md.lastRegion == "U") {
                japan = false;
                region = 1;
            }
            if (md.lastRegion == "J") {
                japan = true;
                region = 1;
            }
            if (md.lastRegion == "P") {
                japan = false;
                region = 2;
            }
        }
        md.clean();
        shared_ptr<Gui> gui(Gui::getInstance());
        Util::copy(source, destination);

        CfgProcessor * processor=new CfgProcessor();
        processor->replace(pathName, gui->path, "region", "region = " + to_string(region),false);
        delete(processor);
        pcsxCfgFound = true;
    }
}

//*******************************
// Game::updateObj
//*******************************
void Game::updateObj() {
    string tmp;
    discs.clear();
    title = valueOrDefault("title", pathName);
    memcard = valueOrDefault("memcard", "");

    publisher = valueOrDefault("publisher", "Other");
    string automation = valueOrDefault("automation", "0");
    automationUsed = atoi(automation.c_str());
    tmp = valueOrDefault("players", "1");
    if (Util::isInteger(tmp.c_str())) players = atoi(tmp.c_str()); else players = 1;
    tmp = valueOrDefault("year", "2018");

    if (Util::isInteger(tmp.c_str())) year = atoi(tmp.c_str()); else year = 2018;
    tmp = valueOrDefault("highres","0");
    if (Util::isInteger(tmp.c_str())) highRes = atoi(tmp.c_str()); else highRes = 0;
    tmp = valueOrDefault("discs", "");
    favorite = valueOrDefault("favorite", "0");

    if (!tmp.empty()) {
        vector<string> strings;
        istringstream f(tmp);
        string s;
        while (getline(f, s, ',')) {
            s = Util::decode(s);
            strings.push_back(s);
        }
        for (int i = 0; i < strings.size(); i++) {
            Disc disc;
            disc.diskName = strings[i];
            if (imageType == IMAGE_CUE_BIN) {
                string cueFile = fullPath  + disc.diskName + EXT_CUE;
                bool discCueExists = Util::exists(cueFile);
                if (discCueExists) {
                    disc.binVerified = validateCue(cueFile, fullPath );
                    disc.cueFound = true;
                    disc.cueName = disc.diskName;
                }
                discs.push_back(disc);
            }
            if (imageType == IMAGE_PBP) {
                string pbpName = Util::findFirstFile(EXT_PBP, fullPath );
                if (pbpName == disc.diskName) {
                    disc.cueFound = true;
                } else {
                    disc.cueFound = false;
                }

                disc.binVerified = true;
                disc.cueName = disc.diskName;
                discs.push_back(disc);
            }
        }
    }
    gameIniValid = true;
}

//*******************************
// Game::saveIni
//*******************************
void Game::saveIni(string path) {
    //cout << "Overwritting ini file" << path << endl;
    Inifile *ini = new Inifile();
    ini->section = "Game";
    ini->values["title"] = title;
    ini->values["publisher"] = publisher;
    ini->values["year"] = to_string(year);
    ini->values["serial"] = serial;
    ini->values["region"] = region;
    ini->values["players"] = to_string(players);
    ini->values["automation"] = to_string(automationUsed);
    ini->values["imagetype"] = to_string(imageType);
    ini->values["highres"] = to_string(highRes);
    if (memcard.empty())
    {
        ini->values["memcard"] = "SONY";
    } else
    {
        ini->values["memcard"] = memcard;
    }
    ini->values["Favorite"] = favorite;

    stringstream ss;
    for (int i = 0; i < discs.size(); i++) {
        ss << Util::escape(discs[i].diskName);
        if (i != discs.size() - 1) {
            ss << ",";
        }
    }
    ini->values["discs"] = ss.str();
    ini->save(path);
    delete ini;
    gameIniFound = true;
}

//*******************************
// Game::parseIni
//*******************************
void Game::parseIni(string path) {
    iniValues.clear();
    Inifile *ini = new Inifile();
    ini->load(path);
    if (ini->values.empty()) {
        gameIniFound = false;
        delete ini;
        return;
    }
    gameIniFound = true;
    iniValues = ini->values;
    delete ini;
}

//*******************************
// Game::readIni
//*******************************
void Game::readIni(string path) {
    parseIni(path);
    updateObj();
}