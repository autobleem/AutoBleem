//
// Created by screemer on 2018-12-15.
//

#include "game.h"
#include "main.h"
#include "util.h"

using namespace std;

string Game::valueOrDefault(string name, string def) {
    string value;
    if (iniValues.find(name) != iniValues.end()) {
        value = trim(iniValues.find(name)->second);
        if (value.length() == 0) {
            return def;
        }
    } else {
        value = def;
    }
    return value;
}

bool Game::print() {
    bool result = true;


    cout << "-------------------" << endl;
    cout << "Printing game data:" << endl;
    cout << "-----------------" << endl;
    cout << "Game folder id: " << folder_id << endl;
    cout << "Game: " << title << endl;
    cout << "Players: " << players << endl;
    cout << "Publisher: " << publisher << endl;
    cout << "Year: " << year << endl;
    cout << "GameData found: " << gameDataFound << endl;
    cout << "Game.ini found: " << gameIniFound << endl;
    cout << "Game.ini valid: " << gameIniValid << endl;
    cout << "LABEL found:" << imageFound << endl;
    cout << "LIC found:" << licFound << endl;
    cout << "pcsx.cfg found: " << pcsxCfgFound << endl;
    cout << "TotalDiscs: " << discs.size() << endl;

    for (int i = 0; i < discs.size(); i++) {
        cout << "  Disc:" << i + 1 << "  " << discs[i].diskName << endl;
        if (discs[i].diskName.length() == 0) result = false;
        cout << "  CUE found: " << discs[i].cueFound << endl;
        if (discs[i].cueFound) result = false;
        cout << "  BIN correct: " << discs[i].binVerified << endl;
        if (discs[i].binVerified) result = false;
    }

    if (!gameDataFound) result = false;
    if (!gameIniFound) result = false;
    if (!gameIniValid) result = false;
    if (!imageFound) result = false;
    if (!licFound) result = false;
    if (!pcsxCfgFound) result = false;

    if (result) {
        cout << "-------OK-------" << endl;
    } else {
        cout << "------FAIL------" << endl;
    }
    return result;
}

void Game::recoverFiles() {
    string path = Util::getWorkingPath();

    if (discs.size() == 0) {
        // find any CUE
        string destination = fullPath + "GameData" + Util::separator();
        vector<DirEntry> entries = Util::dir(destination);
        for (int i = 0; i < entries.size(); i++) {
            DirEntry entry = entries[i];
            if (entry.name[0] == '.') continue;
            if (Util::strcicmp(entry.name.substr(entry.name.length() - 4).c_str(), ".cue") == 0) {
                string discEntry = entry.name.substr(0, entry.name.size() - 4);
                Disc disc;
                disc.diskName = discEntry;
                disc.cueFound = true;
                discs.push_back(disc);
            }
        }
    }

    if (discs.size() > 0) {
        if (!licFound) {
            string source = path + Util::separator() + "default.lic";
            string destination = fullPath + "GameData" + Util::separator() + discs[0].diskName + ".lic";
            cerr << "SRC:" << source << " DST:" << destination << endl;
            Util::copy(source, destination);
        }
        if (!imageFound) {
            string source = path + Util::separator() + "default.png";
            string destination = fullPath + "GameData" + Util::separator() + discs[0].diskName + ".png";
            cerr << "SRC:" << source << " DST:" << destination << endl;
            Util::copy(source, destination);

        }
    }


    if (!pcsxCfgFound) {
        string source = path + Util::separator() + "pcsx.cfg";
        string destination = fullPath + "GameData" + Util::separator() + "pcsx.cfg";
        cerr << "SRC:" << source << " DST:" << destination << endl;
        Util::copy(source, destination);
        pcsxCfgFound = true;
    }

}

void Game::updateObj() {
    string tmp;
    discs.clear();

    title = valueOrDefault("title", pathName);
    publisher = valueOrDefault("publisher", "Other");
    tmp = valueOrDefault("players", "1");
    if (Util::is_integer_name(tmp.c_str())) players = atoi(tmp.c_str()); else players = 1;
    tmp = valueOrDefault("year", "2018");
    if (Util::is_integer_name(tmp.c_str())) year = atoi(tmp.c_str()); else year = 2018;
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
            discs.push_back(disc);
        }
    }
}


void Game::parseIni(string path) {
    iniValues.clear();
    ifstream file;
    string iniLine;
    file.open(path);

    if (!file.good()) {
        gameIniFound = false;

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