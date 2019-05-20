//
// Created by screemer on 2019-01-25.
//
#pragma once

#include <vector>
#include <string>
#include "gui_screen.h"

//********************
// GuiSelectMemcard
//********************
class GuiSelectMemcard : public GuiScreen  {
public:
    void init();
    void render();
    void loop();

    std::vector<std::string> cards;
    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    std::string cardSelected="";

    using GuiScreen::GuiScreen;
};
