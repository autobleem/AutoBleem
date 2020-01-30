//
// Created by screemer on 23.12.18.
//

#include "config.h"
#include "../util.h"
#include "../launcher/gui_NotificationLine.h"
#include "../DirEntry.h"
#include "../environment.h"

//*******************************
// Config::Config()
//*******************************
Config::Config()
{
    std::string path=Env::getWorkingPath() + sep + "config.ini";
    inifile.load(path);

    // these are no longer used
    inifile.values.erase("stheme");
    inifile.values.erase("autoregion");
    inifile.values.erase("quick");
    inifile.values.erase("quickmenu");
    inifile.values.erase("delay");
    save();

    inifile.values["adv"]="true";

    bool aDefaultWasSet {false};
    if (inifile.values["language"]=="")
    {
        inifile.values["language"]="English";
        aDefaultWasSet = true;
    }
    if (inifile.values["ui"]=="")
    {
        inifile.values["ui"]="classic";
        aDefaultWasSet = true;
    }
    if (inifile.values["aspect"]=="")
    {
        inifile.values["aspect"]="false";
        aDefaultWasSet = true;
    }
    if (inifile.values["jewel"]=="")
    {
        inifile.values["jewel"]="default";
        aDefaultWasSet = true;
    }
    if (inifile.values["music"]=="")
    {
        inifile.values["music"]="--";
        aDefaultWasSet = true;
    }
    if (inifile.values["showingtimeout"]=="")
    {
        inifile.values["showingtimeout"]=DefaultShowingTimeoutText;
        aDefaultWasSet = true;
    }

    if (inifile.values["raconfig"]=="")
    {
        inifile.values["raconfig"]="true";
        aDefaultWasSet = true;
    }

    inifile.values["pcsx"]="bleemsync";

    if (aDefaultWasSet)
        save();
}

//*******************************
// Config::save
//*******************************
void Config::save()
{
    inifile.values["pcsx"]="bleemsync";
    std::string path=Env::getWorkingPath() + sep + "config.ini";
    inifile.save(path);
}
