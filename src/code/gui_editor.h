//
// Created by screemer on 2019-01-25.
//

#ifndef AUTOBLEEM_GUI_GUI_EDITOR_H
#define AUTOBLEEM_GUI_GUI_EDITOR_H
#include "gui_screen.h"
#include "inifile.h"
#include <vector>
using namespace std;


class GuiEditor : public GuiScreen{
public:
    void init();
    void render();
    void loop();
    Inifile game;


    bool changes=false;

    using GuiScreen::GuiScreen;
    SDL_Texture * cover= nullptr;
};


#endif //AUTOBLEEM_GUI_GUI_EDITOR_H
