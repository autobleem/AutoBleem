//
// Created by screemer on 2019-01-24.
//

#ifndef AUTOBLEEM_GUI_GUI_CONFIRM_H
#define AUTOBLEEM_GUI_GUI_CONFIRM_H

#include <string>
#include "gui_screen.h"
using namespace std;
class GuiConfirm : public GuiScreen{
public:
    void render();
    void loop();

    string label="";
    bool result=false;

    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_CONFIRM_H
