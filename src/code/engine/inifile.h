//
// Created by screemer on 2018-12-19.
//

#ifndef CBLEEMSYNC_INIFILE_H
#define CBLEEMSYNC_INIFILE_H

#include "../main.h"
#include <map>

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
    string section="";			// example: "Game" inside [Game] above
    string path="";
    string entry="";			// directory entry name
    map<string, string> values;	// see example data above

    void load(const string & path);
    void save(const string & path);
};


#endif //CBLEEMSYNC_INIFILE_H
