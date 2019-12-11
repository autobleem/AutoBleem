//
// Created by screemer on 2019-07-25.
//

#pragma once
#include <guigfx/gui.h>
#include "../launcher/ra_integrator.h"
#include <vector>

class GuiPlaylists : public GuiScreen{
public:
    void init();
    void render();
    void loop();
    std::vector<std::string> playlists;
    std::vector<int> sizes;

    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    bool changes=false;

    bool cancelled = false;

    shared_ptr<RAIntegrator> integrator;
    GfxImage  backgroundImg;

    using GuiScreen::GuiScreen;
};


