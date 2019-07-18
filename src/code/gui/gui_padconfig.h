//
// Created by screemer on 2019-04-15.
//

#ifndef AUTOBLEEM_GUI_GUI_PADCONFIG_H
#define AUTOBLEEM_GUI_GUI_PADCONFIG_H

#include <SDL2/SDL_joystick.h>
#include "gui_screen.h"
#include "../engine/inifile.h"

class GuiPadConfig   : public GuiScreen {
public:
    void render();
    void loop();

    Inifile newConfig;


    int step=0;

    SDL_JoystickID joyid;

    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_PADCONFIG_H
