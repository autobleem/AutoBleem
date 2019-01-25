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
    void init(SDL_Renderer *renderer1, Inifile game);
    void render();
    void loop();
    Inifile game;

    bool changes=false;
};


#endif //AUTOBLEEM_GUI_GUI_EDITOR_H
