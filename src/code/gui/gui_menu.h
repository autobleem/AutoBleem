#pragma once

#include "gui_screen.h"
#include "gui.h"
#include <vector>
#include <string>

class GuiMenu : public GuiScreen {
public:
    GuiMenu(SDL_Shared<SDL_Renderer> _renderer, std::string _title = "")
            : GuiScreen(_renderer), title(_title) {}

    void init() override;
    void render() override;
    void loop() override;

    virtual std::string statusLine();   // returns the status line at the bottom if you need to override it

    shared_ptr<Gui> gui;
    std::string title;
    std::vector<std::string> lines; // these are the menu lines

    int selected = 0;       // the current selected line index
    int maxVisible = 8;     // the number of lines that can fit on the display (theme dependent)
    int firstVisible = 0;   // current visible range on page
    int lastVisible = 8;    // current visible range on page
    int firstLine = 2;      // line 0 is the title.  this is the first line of the menu item lines

    bool changes=false;
    bool cancelled = false;

    //using GuiScreen::GuiScreen;
};
