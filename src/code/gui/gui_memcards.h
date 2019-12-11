//
// Created by screemer on 2019-01-24.
//
#pragma once

#include <guigfx/gui.h>

#include <vector>
#include <string>

//********************
// GuiMemcards
//********************
class GuiMemcards : public GuiScreen {
public:
    void init();
    void render();
    void loop();

    std::vector<std::string> cards;
    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;
    using GuiScreen::GuiScreen;
};
