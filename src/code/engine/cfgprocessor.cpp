//
// Created by screemer on 2019-01-15.
//

#include "cfgprocessor.h"
#include "../util.h"

using namespace std;

void CfgProcessor::replaceInternal(string filePath, string property, string newline) {
    if (!Util::exists(filePath)) {
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

string CfgProcessor::getValue(string entry, string gamePath, string property, bool internal) {
    string filePath;
    if (!internal) {
        filePath = gamePath + entry + Util::separator() + PCSX_CFG;
        if (!Util::exists(filePath)) {
            return "";
        }
    } else {
        filePath = gamePath + Util::separator() + PCSX_CFG;
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

void CfgProcessor::replace(string entry, string gamePath, string property, string newline, bool internal) {

    if (!internal) {
        string realCfgPath = gamePath + entry + Util::separator() + PCSX_CFG;
        replaceInternal(realCfgPath, property, newline);

        for (DirEntry cfgEntry:Util::diru(
                gamePath + "!SaveStates" + Util::separator() + entry + Util::separator() + "cfg")) {
            string path =
                    gamePath + "!SaveStates" + Util::separator() + entry + Util::separator() + "cfg" +
                    Util::separator() +
                    cfgEntry.name;
            replaceInternal(path, property, newline);
        }
    } else {
        string realCfgPath = gamePath + Util::separator() + PCSX_CFG;
        replaceInternal(realCfgPath, property, newline);

        for (DirEntry cfgEntry:Util::diru(
                gamePath + Util::separator() + "cfg")) {
            string path =
                    gamePath + Util::separator() + "cfg" +
                    Util::separator() +
                    cfgEntry.name;
            replaceInternal(path, property, newline);
        }
    }

}


