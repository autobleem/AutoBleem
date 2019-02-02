//
// Created by screemer on 2019-01-15.
//

#include "cfgprocessor.h"
#include "util.h"

using namespace std;

void CfgProcessor::replaceInternal(string filePath, string property, string newline) {
    if (!Util::exists(filePath)) {
        return;
    }
    std::fstream file(filePath, std::ios::in);

    vector<string> lines;
    lines.clear();

    if (file.is_open()) {

        std::string line;
        std::vector<std::string> lines;

        while (std::getline(file, line)) {

            std::string::size_type pos = 0;
            string lcaseline = line;
            string lcasepattern = property;
            lcase(lcaseline);
            lcase(lcasepattern);

            if (lcaseline.rfind(lcasepattern, 0) == 0) {
                lines.push_back(newline);
            } else {
                lines.push_back(line);
            }


        }
        file.close();
        file.open(filePath, std::ios::out | std::ios::trunc);

        for (const auto &i : lines) {
            file << i << std::endl;
        }
        file.flush();
        file.close();

    }
}

void CfgProcessor::replace(string entry, string gamePath, string property, string newline) {

    string realCfgPath = gamePath + entry + Util::separator() + PCSX_CFG;
    replaceInternal(realCfgPath, property, newline);

    for (DirEntry cfgEntry:Util::diru(
            gamePath + "!SaveStates" + Util::separator() + entry + Util::separator() + "cfg")) {
        string path =
                gamePath + "!SaveStates" + Util::separator() + entry + Util::separator() + "cfg" + Util::separator() +
                cfgEntry.name;
        replaceInternal(path, property, newline);
    }


}

