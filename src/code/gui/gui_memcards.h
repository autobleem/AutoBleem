//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_screen.h"

#include <vector>
#include <string>

//********************
// GuiMemcards
//********************
class GuiMemcards : public GuiScreen {
public:
    void init() override;
    void render() override;
    void loop() override;

    std::vector<std::string> cards;
    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;
    using GuiScreen::GuiScreen;
};
