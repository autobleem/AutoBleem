//
// Created by screemer on 2018-12-19.
//

#ifndef CBLEEMSYNC_INIFILE_H
#define CBLEEMSYNC_INIFILE_H

#include "main.h"
#include <map>

class Inifile {
public:
    string section;
    map<string, string> values;

    void load(string path);
    void save(string path);
};


#endif //CBLEEMSYNC_INIFILE_H
