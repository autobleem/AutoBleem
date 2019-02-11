//
// Created by screemer on 23.12.18.
//

#include "config.h"

Config::Config()
{
    string path=Util::getWorkingPath()+Util::separator()+"config.ini";
    inifile.load(path);
    if (inifile.values["language"]=="")
    {
        inifile.values["language"]="English";
    }
}

void Config::save()
{
    string path=Util::getWorkingPath()+Util::separator()+"config.ini";
    inifile.save(path);
}