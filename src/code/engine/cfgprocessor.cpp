//
// Created by screemer on 2019-01-15.
//

#include "cfgprocessor.h"
#include "../util.h"
#include <fstream>
#include "../DirEntry.h"
#include <iostream>
#include "../environment.h"

using namespace std;

//*******************************
// CfgProcessor::replaceProperty
//*******************************
void CfgProcessor::replaceProperty(string fullCfgFilePath, string property, string newline) {
    cout << "cfg replace, '" << fullCfgFilePath << "', '" << property << "' with: '" << newline << "'" << endl;
    if (!DirEntry::exists(fullCfgFilePath)) {
        cout << "  cfg file doesn't exist" << endl;
        return;
    }
    // do not store if file not updated (one less iocall on filesystem)
    bool fileUpdated = false;

    fstream file(fullCfgFilePath, ios::in);
    vector<string> lines;
    lines.clear();

    if (file.is_open()) {

        string line;
        vector<string> lines;

        while (getline(file, line)) {

            string::size_type pos = 0;
            string lcaseline = line;
            string lcasepattern = property;
            lcase(lcaseline);
            lcase(lcasepattern);

            if (lcaseline.rfind(lcasepattern, 0) == 0) {
                fileUpdated = true;
                cout << "  new line: '" << newline << "'" << endl;
                lines.push_back(newline);
            } else {
                lines.push_back(line);
            }
        }
        file.close();
        if (fileUpdated) {
            file.open(fullCfgFilePath, ios::out | ios::trunc);

            for (const auto &i : lines) {
                file << i << endl;
            }
            file.flush();
            file.close();
        }
    }
}

//*******************************
// CfgProcessor::getValueFromCfgFile
//*******************************
string CfgProcessor::getValueFromCfgFile(string fullCfgFilePath, string property) {
    cout << "cfg getValue, '" << fullCfgFilePath << "', '" << property << "'" << endl;
    fstream file(fullCfgFilePath, ios::in);
    vector<string> lines;
    lines.clear();

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            string lcaseline = line;
            string lcasepattern = property;
            lcase(lcaseline);
            lcase(lcasepattern);

            if (lcaseline.rfind(lcasepattern, 0) == 0) {
                string value = line.substr(lcaseline.find("=") + 1);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();   // remove the trailing /r
                }
                trim(value);    // remove leading and trailing spaces
                cout << "  return: '" << value << "'" << endl;
                return value;
            }
        }
        file.close();
    }
    cout << "  return: ''" << endl;
    return "";
}

//*******************************
// CfgProcessor::getValue
// example gamePath = "/media/Games/!SaveStates/7"
// example gamePath = "/media/Games/!SaveStates/Driver 2" or
// example gamePath = "/media/Games/Racing/Driver 2"
//*******************************
string CfgProcessor::getValue(string gamePath, string property) {
    string fullCfgFilePath = gamePath + sep + PCSX_CFG;
    if (!DirEntry::exists(fullCfgFilePath)) {
        cout << "  cfg file doesn't exist" << endl;
        cout << "  return: ''" << endl;
        return "";
    }

    return getValueFromCfgFile(fullCfgFilePath, property);
}

//*******************************
// CfgProcessor::replacePropertyInAllCfgsInDir
// example pathToCfgDir = "/media/Games/!SaveStates/12/cfg"
// example pathToCfgDir = "/media/Games/!SaveStates/Driver 2/cfg"
//*******************************
void CfgProcessor::replacePropertyInAllCfgsInDir(string pathToCfgDir, string property, string newline) {
    cout << "cfg replaceInAllCfg, '" << pathToCfgDir << "', '" << property << "'" << endl;
    for (const DirEntry &cfgEntry : DirEntry::diru_FilesOnly(pathToCfgDir)) {
        if (DirEntry::matchExtension(cfgEntry.name, ".cfg")) {
            string fullCfgFilePath = pathToCfgDir + sep + cfgEntry.name;
            replaceProperty(fullCfgFilePath, property, newline);
        }
    }
}

//*******************************
// CfgProcessor::replaceInternal
// example gamePathInSaveStates = "/media/Games/!SaveStates/12"
//*******************************
void CfgProcessor::replaceInternal(string gamePathInSaveStates, string property, string newline) {
    string realCfgPath = gamePathInSaveStates + sep + PCSX_CFG;
    replaceProperty(realCfgPath, property, newline);

    replacePropertyInAllCfgsInDir(gamePathInSaveStates + sep + "cfg", property, newline);
}

//*******************************
// CfgProcessor::replaceUSB
// example entry = "Driver 2"
// example gamePath = "/media/Games/Racing"
//*******************************
void CfgProcessor::replaceUSB(string entry, string gamePath, string property, string newline) {
    string realCfgPath = gamePath + sep + entry + sep + PCSX_CFG;
    replaceProperty(realCfgPath, property, newline);    // replace in the game dir pcsx.cfg

    realCfgPath = Env::getPathToSaveStatesDir() + sep + entry + sep + PCSX_CFG;
    replaceProperty(realCfgPath, property, newline);    // replace in the !SaveStates/game/pcsx.cfg

    // replace in the !SaveStates/game/cfg/*.cfg
    replacePropertyInAllCfgsInDir(Env::getPathToSaveStatesDir() + sep + entry + sep + "cfg", property, newline);
}

//*******************************
// CfgProcessor::replace
// example entry = "Driver 2"
// example gamePath = "/media/Games/Racing/Driver 2", internal = false
// example gamePath = "/media/Games/!SaveStates/12", internal = true
//*******************************
void CfgProcessor::replace(string entry, string gamePath, string property, string newline, bool internal) {
    if (internal)
        replaceInternal(gamePath, property, newline);
    else
        replaceUSB(entry, DirEntry::getDirNameFromPath(gamePath), property, newline);
}


//*******************************
// CfgProcessor::replaceRaConf
//*******************************
void CfgProcessor::replaceRaConf(std::string fullCfgFilePath, std::string property, std::string newline)
{
    cout << "cfg replaceRaConf, '" << fullCfgFilePath << "', '" << property << "'" << endl;
    replaceProperty(fullCfgFilePath, property, newline);
}
