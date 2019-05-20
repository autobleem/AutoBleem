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

    int alpha;
    int start;

    using GuiScreen::GuiScreen;
};

