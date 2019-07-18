//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_screen.h"

//********************
// GuiAbout
//********************
class GuiAbout : public GuiScreen {
public:
    void render();
    void loop();
    using GuiScreen::GuiScreen;
};
