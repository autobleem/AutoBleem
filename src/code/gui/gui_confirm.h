//
// Created by screemer on 2019-01-24.
//
#pragma once

#include <string>
#include "gui_screen.h"

//********************
// GuiConfirm
//********************
class GuiConfirm : public GuiScreen{
public:
    void render();
    void loop();

    std::string label="";
    bool result=false;

    using GuiScreen::GuiScreen;
};
