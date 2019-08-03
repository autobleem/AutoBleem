//
// Created by screemer on 2019-01-15.
//

#include "cfgprocessor.h"
#include "../util.h"
#include <fstream>
#include "../DirEntry.h"

using namespace std;

//*******************************
// CfgProcessor::replaceInternal
//*******************************
void CfgProcessor::replaceInternal(string filePath, string property, string newline) {
    if (!DirEntry::exists(filePath)) {
        return;
    }
    // do not store if file not updated (one less iocall on filesystem)
    bool fileUpdated = false;

    fstream file(filePath, ios::in);
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
                lines.push_back(newline);
            } else {
                lines.push_back(line);
            }
        }
        file.close();
        if (fileUpdated) {
            file.open(filePath, ios::out | ios::trunc);

            for (const auto &i : lines) {
                file << i << endl;
            }
            file.flush();
            file.close();
        }
    }
}

//*******************************
// CfgProcessor::getValue
//*******************************
string CfgProcessor::getValue(string entry, string gamePath, string property, bool internal) {
    string filePath;
    if (!internal) {
        filePath = gamePath + entry + DirEntry::separator() + PCSX_CFG;
        if (!DirEntry::exists(filePath)) {
            return "";
        }
    } else {
        filePath = gamePath + DirEntry::separator() + PCSX_CFG;
    }
    fstream file(filePath, ios::in);
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
                return value;
            }
        }
        file.close();
    }
    return "";
}

//*******************************
// CfgProcessor::replaceRaConf
//*******************************
void CfgProcessor::replaceRaConf(std::string filePath, std::string property, std::string newline)
{
    replaceInternal(filePath, property, newline);
}
//*******************************
// CfgProcessor::replace
//*******************************
void CfgProcessor::replace(string entry, string gamePath, string property, string newline, bool internal) {

    if (!internal) {
        string realCfgPath = gamePath + sep + entry + sep + PCSX_CFG;
        replaceInternal(realCfgPath, property, newline);

        for (const DirEntry &cfgEntry : DirEntry::diru(gamePath + sep + "!SaveStates" + sep + entry + sep + "cfg")) {
            string path = gamePath + sep + "!SaveStates" + sep + entry + sep + "cfg" + sep + cfgEntry.name;
            replaceInternal(path, property, newline);
        }
    } else {
        string realCfgPath = gamePath + sep + PCSX_CFG;
        replaceInternal(realCfgPath, property, newline);

        for (const DirEntry &cfgEntry : DirEntry::diru(gamePath + sep + "cfg")) {
            string path = gamePath + sep + "cfg" + sep + cfgEntry.name;
            replaceInternal(path, property, newline);
        }
    }
}
