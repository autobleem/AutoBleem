//
// Created by screemer on 2018-12-19.
//
#pragma once

#include <string>
#include <map>

//******************
// Inifile
//******************
//
// Example USB game inifile.  stored in map<string, string> values:
//
// [Game]
// Automation=0
// Discs=Twisted Metal 2.pbp
// Highres=0
// Imagetype=1		// IMAGE_CUE_BIN=0, IMAGE_PBP=1
// Memcard=SONY
// Players=2
// Publisher=Sony Computer Entertainment.
// Title=Twisted Metal 2
// Year=1996
//
class Inifile {
public:
    std::string section="";                     // example: "Game" (from [Game] above)
    std::string path="";                        // example: "/media/Games/Racing/007 Racing (USA)/Game.ini"
    std::string entry="";                       // example: "007 Racing (USA)"
    std::map<std::string, std::string> values;  // see example data above

    void load(const std::string & path);
    void save(const std::string & path);
    void print();
};
