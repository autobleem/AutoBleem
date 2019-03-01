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
    if (inifile.values["delay"] == "") {
        inifile.values["delay"] = "1";
    }

    if (inifile.values["ui"]=="")
    {
        inifile.values["ui"]="classic";
    }
    inifile.values["pcsx"]="bleemsync";
}

void Config::save()
{
    inifile.values["pcsx"]="bleemsync";
    string path=Util::getWorkingPath()+Util::separator()+"config.ini";
    inifile.save(path);
}
