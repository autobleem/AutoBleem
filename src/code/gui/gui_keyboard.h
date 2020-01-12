//
// Created by screemer on 2019-01-24.
//
#pragma once

#include <string>
#include "gui_screen.h"
#include "gui.h"

//********************
// GuiKeyboard
//********************
class GuiKeyboard : public GuiScreen {
public:
    virtual void render();
    virtual void loop();
    virtual void init();

    std::shared_ptr<Gui> gui;
    int selx=0;
    int sely=0;
    std::string label="";
    std::string result="";
    int cursorIndex = 0;
    bool L1_caps_shift = false;
    bool L2_cursor_shift = false;
    bool cancelled=true;
    bool usingUsbKeyboard = false;

    using GuiScreen::GuiScreen;
};
