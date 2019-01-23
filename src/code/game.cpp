//
// Created by screemer on 2018-12-15.
//
#include "game.h"
#include "metadata.h"
#include "isodir.h"
#include "inifile.h"
#include "cfgprocessor.h"

using namespace std;

string fixSerial(string serial) {
    replace(serial.begin(), serial.end(), '_', '-');
    serial.erase(std::remove(serial.begin(), serial.end(), '.'), serial.end());
    string fixed = "";
    stringstream alpha;
    stringstream digits;
    bool digitsProcessing = false;
    for (int i = 0; i < serial.size(); i++) {
        int maxchars = serial[0] == 'L' ? 3 : 4;
        if (!isdigit(serial[i])) {

            if (digitsProcessing) continue;
            if (serial[i] == '-') continue;
            if (alpha.str().length() < maxchars) {
                alpha << serial[i];
            }
        } else {
            digitsProcessing = true;
            digits << serial[i];
        }
    }
    return alpha.str() + "-" + digits.str();
}

string Game::scanSerial() {
    if (imageType == 1) {
        string destinationDir = fullPath ;
        string pbpFileName = Util::findFirstFile(EXT_PBP, destinationDir);
        if (pbpFileName != "") {
            ifstream is;
            is.open(destinationDir + pbpFileName);

            long magic = Util::readDword(&is);
            if (magic != 0x50425000) {
                return "";
            }
            long second = Util::readDword(&is);
            if (second != 0x10000) {
                return "";
            }
            long sfoStart = Util::readDword(&is);

            is.seekg(sfoStart, ios::beg);

            unsigned int signature = Util::readDword(&is);
            if (signature != 1179865088) {
                return "";
            }
            unsigned int version = Util::readDword(&is);;
            unsigned int fields_table_offs = Util::readDword(&is);
            unsigned int values_table_offs = Util::readDword(&is);
            int nitems = Util::readDword(&is);

            vector<string> fields;
            vector<string> values;
            fields.clear();
            values.clear();
            is.seekg(sfoStart, ios::beg);
            is.seekg(fields_table_offs, ios::cur);
            for (int i = 0; i < nitems; i++) {
                string fieldName = Util::readString(&is);
                Util::skipZeros(&is);
                fields.push_back(fieldName);
            }

            is.seekg(sfoStart, ios::beg);
            is.seekg(values_table_offs, ios::cur);
            for (int i = 0; i < nitems; i++) {
                string valueName = Util::readString(&is);
                Util::skipZeros(&is);
                values.push_back(valueName);
            }

            is.close();

            for (int i = 0; i < nitems; i++) {
                if (fields[i] == "DISC_ID") {
                    string potentialSerial = values[i];
                    return fixSerial(potentialSerial);
                }
            }
        }
    }
    if (imageType == 0) {


        string prefixes[] = {
                "CPCS", "ESPM", "HPS", "LPS", "LSP", "SCAJ", "SCED", "SCES", "SCPS", "SCUS", "SIPS", "SLES", "SLKA",
                "SLPM", "SLPS", "SLUS"};

        if (firstBinPath == "") {
            return ""; // not at this stage
        }


        for (int level = 1; level < 4; level++) {
            Isodir *dirLoader = new Isodir();
            IsoDirectory dir = dirLoader->getDir(firstBinPath, level);
            delete dirLoader;
            string serialFound = "";
            if (!dir.rootDir.empty()) {
                for (string entry:dir.rootDir) {
                    cout << entry << endl;
                    string potentialSerial = fixSerial(entry);
                    for (string prefix:prefixes) {
                        int pos = potentialSerial.find(prefix.c_str(), 0);
                        if (pos == 0) {
                            serialFound = potentialSerial;
                            cout << "Serial number: " << serialFound << endl;
                            return serialFound;
                        }

                    }
                }
                string volume = fixSerial(dir.volumeName);
                for (string prefix:prefixes) {
                    int pos = volume.find(prefix.c_str(), 0);
                    if (pos == 0) {
                        serialFound = volume;
                        cout << "Serial number: " << serialFound << endl;
                        return serialFound;
                    }

                }

            } else {
                return "";
            }

        }
    }
    return "";
}

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
                firstBinPath = binPath;
            }

        }
    }
    cueStream.close();
    return result;
}

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

bool Game::verify() {
    bool result = true;

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
    cout << "GameData found: " << gameDataFound << endl;
    cout << "Game.ini found: " << gameIniFound << endl;
    cout << "Game.ini valid: " << gameIniValid << endl;
    cout << "PNG found:" << imageFound << endl;
    cout << "LIC found:" << licFound << endl;
    cout << "pcsx.cfg found: " << pcsxCfgFound << endl;
    cout << "TotalDiscs: " << discs.size() << endl;

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

void Game::recoverMissingFiles() {
    string path = Util::getWorkingPath();

    Metadata md;
    bool metadataLoaded = false;

    if (this->imageType == 1) {

        // disc link
        string destinationDir = fullPath ;
        string pbpFileName = Util::findFirstFile(EXT_PBP, destinationDir);
        if (pbpFileName != "") {
            if (discs.size() == 0) {
                automationUsed = true;
                Disc disc;
                disc.diskName = pbpFileName;
                disc.cueFound = true;
                disc.cueName = pbpFileName;
                disc.binVerified = true;
                discs.push_back(disc);

            }
        }
    }
    if (this->imageType == 0) {


        if (discs.size() == 0) {
            automationUsed = true;
            // find cue files
            string destination = fullPath ;
            for (DirEntry entry: Util::diru(destination)) {
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
            string source = path + Util::separator() + "default.lic";
            string destination = fullPath  + discs[0].diskName + ".lic";
            cerr << "SRC:" << source << " DST:" << destination << endl;
            Util::copy(source, destination);
            licFound = true;
        }
        if (!imageFound) {
            automationUsed = true;

            string source = path + Util::separator() + "default.png";
            string destination = fullPath  + discs[0].diskName + ".png";
            cerr << "SRC:" << source << " DST:" << destination << endl;
            Util::copy(source, destination);
            // maybe we can do better ?
            string serial = scanSerial();
            if (serial != "") {

                if (md.lookup(serial)) {
                    metadataLoaded = true;
                    cout << "Updating cover" << destination << endl;
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
        string source = path + Util::separator() + "pcsx.cfg";
        string destination = fullPath + "pcsx.cfg";
        cerr << "SRC:" << source << " DST:" << destination << endl;
        CfgProcessor *pcsxProcessor = new CfgProcessor();

        int region = 0;
        bool japan = false;


        if (!metadataLoaded) {
            string serial = scanSerial();
            if (serial != "") {
                metadataLoaded = md.lookup(serial);
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
        pcsxProcessor->process(source, destination, region, japan, 1, false, 39);
        delete pcsxProcessor;
        // Util::copy(source, destination);
        pcsxCfgFound = true;
    }

}

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
    tmp = valueOrDefault("discs", "");
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
            if (imageType == 0) {
                string cueFile = fullPath  + disc.diskName + EXT_CUE;
                bool discCueExists = Util::exists(cueFile);
                if (discCueExists) {
                    disc.binVerified = validateCue(cueFile, fullPath );
                    disc.cueFound = true;
                    disc.cueName = disc.diskName;
                }
                discs.push_back(disc);
            }
            if (imageType == 1) {
                string pbpName = Util::findFirstFile(EXT_PBP, fullPath );
                if (pbpName == disc.diskName) {
                    disc.cueFound = true;
                } else {
                    disc.cueFound = false;
                }

                disc.binVerified = true;

                disc.cueName = disc.diskName;
            }

        }
    }
    gameIniValid = true;


}

void Game::saveIni(string path) {
    cout << "Overwritting ini file" << path << endl;
    Inifile *ini = new Inifile();
    ini->section = "Game";
    ini->values["title"] = title;
    ini->values["publisher"] = publisher;
    ini->values["year"] = to_string(year);
    ini->values["players"] = to_string(players);
    ini->values["automation"] = to_string(automationUsed);
    ini->values["imagetype"] = to_string(imageType);
    if (memcard.empty())
    {
        ini->values["memcard"] = "SONY";
    } else
    {
        ini->values["memcard"] = memcard;
    }

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

void Game::readIni(string path) {
    parseIni(path);
    updateObj();
}