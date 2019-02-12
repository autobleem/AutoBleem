//
// Created by screemer on 2019-01-25.
//

#ifndef AUTOBLEEM_GUI_GUI_SELECTMEMCARD_H
#define AUTOBLEEM_GUI_GUI_SELECTMEMCARD_H


#include <vector>
#include <string>
#include "gui_screen.h"

using namespace std;
class GuiSelectMemcard : public GuiScreen  {
public:
    void init();
    void render();
    void loop();

    vector<string> cards;
    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    string cardSelected="";

    using GuiScreen::GuiScreen;
};


#endif //AUTOBLEEM_GUI_GUI_SELECTMEMCARD_H
