//
// Created by screemer on 2019-01-24.
//

#ifndef AUTOBLEEM_GUI_GUI_SPLASH_H
#define AUTOBLEEM_GUI_GUI_SPLASH_H

#include "gui_screen.h"
class GuiSplash : public GuiScreen {
public:
    void render();
    void loop();

    int alpha;
    int start;

    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_SPLASH_H
