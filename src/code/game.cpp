//
// Created by screemer on 2018-12-15.
//
#include "game.h"
#include "metadata.h"

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
    string prefixes[] = {
            "CPCS", "ESPM", "HPS", "LPS", "LSP", "SCAJ", "SCED", "SCES", "SCPS", "SCUS", "SIPS", "SLES", "SLKA", "SLPM",
            "SLPS", "SLUS"};

    if (firstBinPath == "") {
        return ""; // not at this stage
    }

    ifstream stream(firstBinPath, ios::binary);
    stream.seekg(0, ios_base::end);
    int length = stream.tellg();


    char triggerCharacters[] = {'C', 'E', 'H', 'L', 'S'};
    char bits[11];
    bool foundSerial = false;
    string serial = "";
    for (int pos = 0; pos < length; pos += 11) {

        if (pos > MAX_SERIAL_CHECK) break;
        stream.seekg(pos, ios::beg);
        stream.read(bits, 11);
        char triggerChar;
        for (int i = 0; i < 11; i++) {

            for (int charid = 0; charid < 5; charid++) {

                triggerChar = triggerCharacters[charid];

                if (bits[i] == triggerChar) {

                    pos += i;
                    stream.seekg(pos, ios::beg);
                    stream.read(bits, 11);

                    string possibleString = bits;

                    for (int prefid = 0; prefid < 16; prefid++) {
                        if (possibleString.substr(0, prefixes[prefid].size()) == prefixes[prefid]) {
                            foundSerial = true;
                            serial = possibleString;

                        }
                    }
                }
                if (foundSerial) break;
            }
        }
        if (foundSerial) break;
    }
    if (!foundSerial) {
        return "";
    }

    return fixSerial(serial);
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
            // check if it is ecm
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
    /*
    if (automationUsed) {
        result = result || automationUsed;
    }
     */
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
    cout << "EXT_PNG found:" << imageFound << endl;
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

    if (discs.size() == 0) {
        automationUsed = true;
        // find cue files
        string destination = fullPath + GAME_DATA + Util::separator();
        for (DirEntry entry: Util::dir(destination)) {
            if (entry.name[0] == '.') continue;

                if (Util::matchExtension(entry.name,EXT_CUE)) {
                    string discEntry = entry.name.substr(0, entry.name.size() - 4);
                    Disc disc;
                    disc.diskName = discEntry;
                    disc.cueFound = true;
                    disc.cueName = discEntry;
                    disc.binVerified = validateCue(destination + entry.name, fullPath + GAME_DATA + Util::separator());
                    discs.push_back(disc);
                }
        }
    }

    if (discs.size() > 0) {
        if (!licFound) {
            automationUsed = true;
            string source = path + Util::separator() + "default.lic";
            string destination = fullPath + GAME_DATA + Util::separator() + discs[0].diskName + ".lic";
            cerr << "SRC:" << source << " DST:" << destination << endl;
            Util::copy(source, destination);
            licFound = true;
        }
        if (!imageFound) {
            automationUsed = true;

            string source = path + Util::separator() + "default.png";
            string destination = fullPath + GAME_DATA + Util::separator() + discs[0].diskName + ".png";
            cerr << "SRC:" << source << " DST:" << destination << endl;
            Util::copy(source, destination);
            // maybe we can do better ?
            string serial = scanSerial();
            if (serial != "") {
                Metadata md;
                if (md.lookup(serial)) {

                    cout << "Updating cover" << destination << endl;
                    ofstream pngFile;
                    pngFile.open(destination);
                    pngFile.write(md.bytes, md.dataSize);
                    pngFile.flush();
                    pngFile.close();
                    automationUsed = false;
                    imageFound = true;
                };

            }
            imageFound = true;

        }
    }


    if (!pcsxCfgFound) {
        automationUsed = true;
        string source = path + Util::separator() + "pcsx.cfg";
        string destination = fullPath + GAME_DATA + Util::separator() + "pcsx.cfg";
        cerr << "SRC:" << source << " DST:" << destination << endl;
        Util::copy(source, destination);
        pcsxCfgFound = true;
    }

}

void Game::updateObj() {
    string tmp;
    discs.clear();

    // Use folder names for convinience and ignore game.ini
    title = valueOrDefault("title", pathName);
    //title = pathName;
    publisher = valueOrDefault("publisher", "Other");
    string automation = valueOrDefault("automation", "0");
    automationUsed = atoi(automation.c_str());
    tmp = valueOrDefault("players", "1");
    if (Util::isInteger(tmp.c_str())) players = atoi(tmp.c_str()); else players = 1;
    tmp = valueOrDefault("year", "2018");
    if (Util::isInteger(tmp.c_str())) year = atoi(tmp.c_str()); else year = 2018;
    tmp = valueOrDefault("discs", "");
    if (tmp.length() != 0) {
        vector<string> strings;
        istringstream f(tmp);
        string s;
        while (getline(f, s, ',')) {
            strings.push_back(s);
        }
        for (int i = 0; i < strings.size(); i++) {
            Disc disc;
            disc.diskName = strings[i];

            string cueFile = fullPath + GAME_DATA + Util::separator() + disc.diskName + ".cue";
            bool discCueExists = Util::exists(cueFile);
            if (discCueExists) {
                disc.binVerified = validateCue(cueFile, fullPath + GAME_DATA + Util::separator());
                disc.cueFound = true;
                disc.cueName = disc.diskName;
            }
            discs.push_back(disc);
        }
    }

    gameIniValid = true;


}

void Game::saveIni(string path) {
    cout << "Overwritting ini file" << path << endl;

    ofstream os;
    os.open(path);
    os << "[Game]" << endl;
    os << "Discs=";
    for (int i = 0; i < discs.size(); i++) {
        os << discs[i].diskName;
        if (i != discs.size() - 1) {
            os << ",";
        }
    }
    os << endl;
    os << "Title=" << title << endl;
    os << "Publisher=" << publisher << endl;
    os << "Players=" << players << endl;
    os << "Year=" << year << endl;
    os << "Automation=" << automationUsed << endl;


    os.flush();
    os.close();
    gameIniFound = true;
}

void Game::parseIni(string path) {
    iniValues.clear();
    ifstream file;
    string iniLine;
    file.open(path);

    if (!file.good()) {
        gameIniFound = false;
        return;

    }
    while (getline(file, iniLine)) {
        iniLine = trim(iniLine);
        if (iniLine.length() == 0) continue;
        if (iniLine.find('=') != string::npos) {
            iniLine = lcase(iniLine, iniLine.find('='));
            string paramName = iniLine.substr(0, iniLine.find('='));
            string paramVal = iniLine.substr(iniLine.find('=') + 1, string::npos);
            iniValues[paramName] = paramVal;
        }

        if (file.eof()) break;
    };
    gameIniFound = true;
    file.close();

}

void Game::readIni(string path) {
    parseIni(path);
    updateObj();
}