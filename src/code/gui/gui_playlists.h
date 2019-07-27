//
// Created by screemer on 2019-07-25.
//

#pragma once
#include "gui_screen.h"
#include "../main.h"
#include <vector>

class GuiPlaylists : public GuiScreen{
public:
    void init();
    void render();
    void loop();
    std::vector<std::string> playlists;

    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    bool changes=false;

    bool cancelled = false;

    using GuiScreen::GuiScreen;
};


