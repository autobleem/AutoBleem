//
// Created by screemer on 2019-04-15.
//

#ifndef AUTOBLEEM_GUI_GUI_PADCONFIG_H
#define AUTOBLEEM_GUI_GUI_PADCONFIG_H

#include "gui_screen.h"

class GuiPadConfig   : public GuiScreen {
public:
    void render();
    void loop();
    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_PADCONFIG_H
