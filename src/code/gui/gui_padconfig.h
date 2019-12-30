#pragma once

#include <SDL2/SDL_joystick.h>
#include "gui_screen.h"
#include "../engine/inifile.h"

//******************
// GuiPadConfig
//******************
class GuiPadConfig   : public GuiScreen {
public:
    void render();
    void loop();

    Inifile newConfig;


    int step=0;

    SDL_JoystickID joyid;

    using GuiScreen::GuiScreen;
};
