//
// Created by screemer on 2019-12-04.
//

#ifndef AUTOBLEEM_GUI_GUI_APP_START_H
#define AUTOBLEEM_GUI_GUI_APP_START_H

#include "../gui/gui_screen.h"
#include "../gui/gui_font_wrapper.h"
#include "ps_game.h"

class GuiAppStart : public GuiScreen {
private:
    TTF_Font_Shared font;
    PsGamePtr game;
    int firstLine = 0;
    int maxLines = 25;
    bool readmeLoaded = false;
    std::string appName;
    std::string buffer;
    int scrolling = 0;
    int totalLines = 0;
    std::string getStringLine(const std::string& str, int lineNo);
public:
    bool result = false;
    void setGame(PsGamePtr game) {this->game=game;};
    void init();
    void render();
    void loop();

    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_APP_START_H
