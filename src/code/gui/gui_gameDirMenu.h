#pragma once
#include "gui_screen.h"
#include "../main.h"
#include "../lang.h"
#include <vector>

class GuiGameDirMenu : public GuiScreen{
public:
    void init();
    void render();
    void loop();

    struct MenuLineData {
        std::string prefix;
        std::string name;   // a separate name makes it easy to compare what line it is, if needed
        int numGames { -1 };    // -1 = "", 0+ = " (# of games)"

        std::string toText();   // returns the text to display on the line

        // ctor
        MenuLineData(std::string _prefix, std::string _name, int _numGames) :
            prefix(_prefix), name(_name), numGames(_numGames) { };
    };
    std::vector<MenuLineData> infoToDisplay;

    int selected=0;
    int maxVisible=8;
    int firstVisible=0;
    int lastVisible=8;

    bool changes=false;
    bool cancelled = false;

    SDL_Shared<SDL_Texture> backgroundImg;

    using GuiScreen::GuiScreen;
};
