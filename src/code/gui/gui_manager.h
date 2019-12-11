//
// Created by screemer on 2019-01-24.
//
#pragma once

#include <guigfx/gui.h>
#include <vector>
#include "../launcher/ps_game.h"

//********************
// GuiManager
//********************
class GuiManager : public GuiScreen{
public:
    void init();
    void render();
    void loop();
    PsGames psGames;

    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    bool changes=false;

    static bool sortByTitle(PsGamePtr i, PsGamePtr j) { return SortByCaseInsensitive(i->title, j->title); }

    using GuiScreen::GuiScreen;
};
