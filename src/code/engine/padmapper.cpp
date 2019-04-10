//
// Created by screemer on 2019-03-21.
//

#include "padmapper.h"
#include "../util.h"

void PadMapper::init()
{
    buttonNames.clear();
    buttonNames[PCS_BTN_CIRCLE]  ="circle";
    buttonNames[PCS_BTN_SQUARE]  ="square";
    buttonNames[PCS_BTN_TRIANGLE]="triangle";
    buttonNames[PCS_BTN_CROSS]   ="cross";
    buttonNames[PCS_BTN_START]   ="start";
    buttonNames[PCS_BTN_SELECT]  ="select";
    buttonNames[PCS_BTN_L1]      ="l1";
    buttonNames[PCS_BTN_L2]      ="l2";
    buttonNames[PCS_BTN_R1]      ="r1";
    buttonNames[PCS_BTN_R2]      ="r2";

    configs.clear();
    // now load all mappings
    string path = Util::getWorkingPath()+"/gpmapping";
    for (DirEntry entry:Util::diru(path))
    {
        if (Util::getFileExtension(entry.name)=="ini")
        {

            Inifile * mapping = new Inifile();
            mapping->load(path+"/"+entry.name);
            cout << "New Pad Config: " << mapping->section << endl;
                 configs[mapping->section]=mapping;
        }
    }


}

int PadMapper::translateButton(int button, string padname)
{
    string buttonName = buttonNames[button];
    // fetch mapping
    Inifile * config = configs[padname];
    if (config== nullptr)
    {
        return button;
    }
    int newBtnCode = atoi(config->values[buttonName].c_str());
    return newBtnCode;
}