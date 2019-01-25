//
// Created by screemer on 2019-01-24.
//

#ifndef AUTOBLEEM_GUI_GUI_ABOUT_H
#define AUTOBLEEM_GUI_GUI_ABOUT_H

#include "gui_screen.h"

class GuiAbout : public GuiScreen {
public:
    void render();
    void loop();
    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_ABOUT_H
