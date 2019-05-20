//
// Created by screemer on 2019-03-02.
//

#pragma once

#include "../gui/gui_screen.h"

//******************
// GuiBtnGuide
//******************
class GuiBtnGuide : public GuiScreen {
public:
    void render();

    void loop();

    using GuiScreen::GuiScreen;
};
