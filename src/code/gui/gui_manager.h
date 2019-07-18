//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_screen.h"
#include "../engine/inifile.h"
#include "../main.h"
#include <vector>

//********************
// GuiManager
//********************
class GuiManager : public GuiScreen{
public:
    void init();
    void render();
    void loop();
    std::vector<Inifile> gameInis;

    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    bool changes=false;

    static bool sortByTitle(Inifile i, Inifile j) { return SortByCaseInsensitive(i.values["title"], j.values["title"]); }

    using GuiScreen::GuiScreen;
};
