//
// Created by screemer on 2019-01-15.
//

#include "cfgprocessor.h"
#include "util.h"

using namespace std;

void CfgProcessor::replace(string entry, string gamePath, string property, string newline) {
    if (!Util::exists(gamePath + "!SaveStates" + Util::separator() + entry + Util::separator() + PCSX_CFG)) {
        Util::copy(gamePath + Util::separator() + entry + Util::separator() + PCSX_CFG,
                   gamePath + "!SaveStates" + Util::separator() + entry + Util::separator() + PCSX_CFG);
    }

    string realCfgPath = gamePath + "!SaveStates" + Util::separator() + entry + Util::separator() + PCSX_CFG;
    std::fstream file(realCfgPath, std::ios::in);

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
        file.open(realCfgPath, std::ios::out | std::ios::trunc);

        for (const auto &i : lines) {
            file << i << std::endl;
        }
        file.flush();
        file.close();
    }

}

void CfgProcessor::process(string source, string destination, int region, bool japan, int soundFilter, bool highres,
                           int clock) {
    if (config.inifile.values["autoregion"] == "true") {
        ifstream is;
        ofstream os;
        is.open(source);
        os.open(destination);

        string line;
        while (getline(is, line)) {
            if (line.rfind("Bios", 0) == 0) {
                if (region == 0) {
                    line = "Bios = SET_BY_PCSX";
                } else {
                    if (japan) {
                        line = "Bios = romJP.bin";
                    } else {
                        line = "Bios = romw.bin";
                    }
                }
            }
            if (line.rfind("region", 0) == 0) {
                line = "region = " + to_string(region);
            }
            if (line.rfind("gpu_neon.enhancement_enable", 0) == 0) {
                if (highres) {
                    line = "gpu_neon.enhancement_enable = 1";
                } else {
                    line = "gpu_neon.enhancement_enable = 0";
                }
            }
            if (line.rfind("spu_config.iUseInterpolation", 0) == 0) {
                line = "spu_config.iUseInterpolation = " + to_string(soundFilter);
            }

            if (line.rfind("psx_clock", 0) == 0) {
                line = "psx_clock = " + to_string(clock);
            }


            os << line << endl;
        }


        os.flush();
        os.close();
        is.close();
    } else {
        Util::copy(source, destination);
    }
}