#pragma once
#include <guigfx/gui.h>
#include <vector>

class GuiGameDirMenu : public GuiScreen{
public:
    void init();
    void render();
    void loop();
    std::vector<std::string> textsToDisplay;

    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    bool changes=false;
    bool cancelled = false;

    SDL_Shared<SDL_Texture> backgroundImg;

    using GuiScreen::GuiScreen;
};
