#ifndef CBLEEMSYNC_MAIN_H
#define CBLEEMSYNC_MAIN_H

#include <vector>
#include <map>
#include <cstdio>
#include <fstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <string>
#include <cstring>
#include <iostream>

#include "util.h"

#define MAX_SERIAL_CHECK 100000

static const char GAME_DATA[] = "GameData";
static const char GAME_INI[] = "Game.ini";
static const char PCSX_CFG[] = "pcsx.cfg";
static const char EXT_PNG[] = ".png";
static const char EXT_ECM[] = ".ecm";
static const char EXT_BIN[] = ".bin";
static const char EXT_CUE[] = ".cue";
static const char EXT_LIC[] = ".lic";

using namespace std;

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

static inline std::string &lcase(std::string &s, int nchars = 0) {
    if (nchars == 0) nchars = s.length();
    for (int i = 0; i < nchars; i++) {
        s[i] = tolower(s[i]);
    }
    return s;
}

void logText(string message);

#endif //CBLEEMSYNC_MAIN_H