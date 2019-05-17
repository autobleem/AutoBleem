//
// Created by screemer on 23.12.18.
//

#include "config.h"
#include "../util.h"

Config::Config()
{
    std::string path=Util::getWorkingPath()+Util::separator()+"config.ini";
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
    if (inifile.values["aspect"]=="")
    {
        inifile.values["aspect"]="false";
    }
    if (inifile.values["jewel"]=="")
    {
        inifile.values["jewel"]="default";
    }
    if (inifile.values["quickmenu"]=="")
    {
        inifile.values["quickmenu"]="UI";
    }
    if (inifile.values["music"]=="")
    {
        inifile.values["music"]="--";
    }
    inifile.values["pcsx"]="bleemsync";
}

void Config::save()
{
    inifile.values["pcsx"]="bleemsync";
    std::string path=Util::getWorkingPath()+Util::separator()+"config.ini";
    inifile.save(path);
}
