//
// Created by screemer on 2019-01-24.
//
#pragma once

#include <string>
#include <guigfx/gui.h>

//********************
// GuiKeyboard
//********************
class GuiKeyboard : public GuiScreen {
public:
    void render();
    void loop();

    int selx=0;
    int sely=0;
    std::string label="";
    std::string result="";
    bool cancelled=true;
    bool caps = false;

    using GuiScreen::GuiScreen;
};
