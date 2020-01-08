#pragma once
#include "gui_screen.h"
#include "../main.h"
#include "../lang.h"
#include <vector>

class GuiGameDirMenu : public GuiScreen{
public:
    void init() override;
    void render() override;
    void loop() override;

    std::vector<std::string> lines;

    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    bool changes=false;
    bool cancelled = false;

    using GuiScreen::GuiScreen;
};
