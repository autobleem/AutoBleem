//
// Created by screemer on 23.12.18.
//

#include "config.h"

Config::Config()
{
    string path=Util::getWorkingPath()+Util::separator()+"config.ini";
    inifile.load(path);
}

void Config::save()
{
    string path=Util::getWorkingPath()+Util::separator()+"config.ini";
    inifile.save(path);
}