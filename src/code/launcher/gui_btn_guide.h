//
// Created by screemer on 2019-03-02.
//

#ifndef AUTOBLEEM_GUI_GUI_BTN_GUIDE_H
#define AUTOBLEEM_GUI_GUI_BTN_GUIDE_H

#include "../gui/gui_screen.h"

class GuiBtnGuide : public GuiScreen {
public:
    void render();

    void loop();

    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_BTN_GUIDE_H
