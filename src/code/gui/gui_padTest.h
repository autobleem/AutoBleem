#pragma once

#include "gui_scrollWin.h"

//******************
// GuiPadTest
//******************
class GuiPadTest   : public GuiScrollWin {
public:
    void init();
    void render() { GuiScrollWin::render(); }
    void loop();

    SDL_JoystickID joyid = 0;

    GuiPadTest(SDL_Shared<SDL_Renderer> renderer1) : GuiScrollWin(renderer1)
    {
    };
};
