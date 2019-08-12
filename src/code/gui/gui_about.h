//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_screen.h"
#include "starfx.h"
#include "gui_font_wrapper.h"

//********************
// GuiAbout
//********************
class GuiAbout : public GuiScreen {
public:
    StarFx fx;
    void init();
    void render();
    void loop();
    SDL_Shared<SDL_Texture> logo;
    TTF_Font_Shared font;
    using GuiScreen::GuiScreen;
};
