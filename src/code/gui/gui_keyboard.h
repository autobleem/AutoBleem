//
// Created by screemer on 2019-01-24.
//

#ifndef AUTOBLEEM_GUI_GUI_KEYBOARD_H
#define AUTOBLEEM_GUI_GUI_KEYBOARD_H

#include <string>
#include "gui_screen.h"
using namespace std;
class GuiKeyboard : public GuiScreen {
public:
    void render();
    void loop();

    int selx=0;
    int sely=0;
    string label="";
    string result="";
    bool cancelled=true;
    bool caps = false;

    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_KEYBOARD_H
