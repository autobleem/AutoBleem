//
// Created by screemer on 2019-01-24.
//

#ifndef AUTOBLEEM_GUI_GUI_MANAGER_H
#define AUTOBLEEM_GUI_GUI_MANAGER_H
#include "gui_screen.h"
#include "inifile.h"
#include <vector>
using namespace std;

class GuiManager : public GuiScreen{
public:
    void init(SDL_Renderer *renderer1);
    void render();
    void loop();
    vector<Inifile> games;

    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    bool changes=false;

};


#endif //AUTOBLEEM_GUI_GUI_MANAGER_H
