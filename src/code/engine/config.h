//
// Created by screemer on 23.12.18.
//

#ifndef CBLEEMSYNC_CONFIG_H
#define CBLEEMSYNC_CONFIG_H

#include "../main.h"
#include "inifile.h"


class Config {
public:
    Inifile inifile;
    Config();
    void save();
};


#endif //CBLEEMSYNC_CONFIG_H
