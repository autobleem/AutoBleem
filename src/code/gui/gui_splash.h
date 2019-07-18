//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_screen.h"

//********************
// GuiSplash
//********************
class GuiSplash : public GuiScreen {
public:
    void render();
    void loop();

    int alpha = 0;
    int start = 0;

    using GuiScreen::GuiScreen;
};

