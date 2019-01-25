//
// Created by screemer on 2019-01-24.
//

#ifndef AUTOBLEEM_GUI_GUI_OPTIONS_H
#define AUTOBLEEM_GUI_GUI_OPTIONS_H

#include "gui_screen.h"
#include <string>
#include <vector>

using namespace std;


class GuiOptions : public GuiScreen{
public:
    void init();
    void render();
    void loop();
    string getOption(vector<string> list, string current, bool next);
    int selOption=0;

    vector<string> themes;
    vector<string> sthemes;
    vector<string> pcsx;
    vector<string> mip;
    vector<string> nomusic;
    vector<string> autoregion;

    vector<string> retroarch;
    vector<string> adv;

    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_OPTIONS_H
