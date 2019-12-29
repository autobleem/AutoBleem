//
// Created by steve on 12/28/19.
//

#pragma once

#include "gui_screen.h"
#include <vector>
#include "gui.h"

//********************
// GuiScrollWin
//********************
class GuiScrollWin : public GuiScreen{
public:
    void init();    // call this from your init()
    void render();  // call this from your render()
    //void loop();  // not used in this class.  base GuiScreen::show() will call virtual loop() in your class.

    shared_ptr<Gui> gui;

    int maxLines = 18;
    std::vector<std::string> lines;
    bool alsoWriteToCout = false;

    void appendLine(const std::string str);

    using GuiScreen::GuiScreen;
};
