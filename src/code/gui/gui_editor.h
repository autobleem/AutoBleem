//
// Created by screemer on 2019-01-25.
//

#ifndef AUTOBLEEM_GUI_GUI_EDITOR_H
#define AUTOBLEEM_GUI_GUI_EDITOR_H
#include "gui_screen.h"
#include "../engine/inifile.h"
#include <vector>
using namespace std;


class GuiEditor : public GuiScreen{
public:
    void init();
    void render();
    void loop();

    void refreshData();
    Inifile game;

    int highres;
    int speedhack;

    int clock;
    int frameskip;

    string gpu="";
    int dither;

    int scanlines;
    int scanlineLevel;



    bool changes=false;

    using GuiScreen::GuiScreen;
    SDL_Texture * cover= nullptr;
};


#endif //AUTOBLEEM_GUI_GUI_EDITOR_H
