//
// Created by screemer on 2/8/19.
//

#ifndef AUTOBLEEM_GUI_GUI_LAUNCHER_H
#define AUTOBLEEM_GUI_GUI_LAUNCHER_H

#include "gui_screen.h"

class GuiLauncher : public GuiScreen{
public:
    void init();
    void render();
    void loop();

    using GuiScreen::GuiScreen;

};


#endif //AUTOBLEEM_GUI_GUI_LAUNCHER_H
