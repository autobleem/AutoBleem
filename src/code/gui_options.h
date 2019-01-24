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
    void init(SDL_Renderer * renderer1);
    void render();
    void loop();
    string getOption(vector<string> list, string current, bool next);
    void renderMenuOption(string text, int line, SDL_Rect rect2, SDL_Rect logoRect, SDL_Rect textRec);
    int selOption=0;

    vector<string> themes;
    vector<string> sthemes;
    vector<string> pcsx;
    vector<string> mip;
    vector<string> nomusic;
    vector<string> autoregion;
};


#endif //AUTOBLEEM_GUI_GUI_OPTIONS_H
