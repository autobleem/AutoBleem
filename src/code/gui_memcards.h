//
// Created by screemer on 2019-01-24.
//

#ifndef AUTOBLEEM_GUI_GUI_MEMCARDS_H
#define AUTOBLEEM_GUI_GUI_MEMCARDS_H
#include "gui_screen.h"

#include <vector>
#include <string>
using namespace std;

class GuiMemcards : public GuiScreen {
public:
    void init();
    void render();
    void loop();

    vector<string> cards;
    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;
    using GuiScreen::GuiScreen;
};



#endif //AUTOBLEEM_GUI_GUI_MEMCARDS_H
