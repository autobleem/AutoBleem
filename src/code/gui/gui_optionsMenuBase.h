#pragma once

#include "gui_screen.h"
#include "gui.h"
#include <vector>
#include <string>

class GuiOptionsMenuBase : public GuiScreen {
public:
    GuiOptionsMenuBase(SDL_Shared<SDL_Renderer> _renderer, std::string _title = "")
        : GuiScreen(_renderer), title(_title) {}

    virtual void init() override;
    virtual void render() override {}
    virtual void loop() override {}

    virtual std::string statusLine();   // returns the status line at the bottom if you need to override it

    std::shared_ptr<Gui> gui;
    std::string title;

    virtual int getVerticalSize() { return 0; }

    //controller and keyboard
    virtual void doJoyDown() { doKeyDown(); }       // move down one line
    virtual void doJoyUp() { doKeyUp(); }           // move up one line

    // keyboard
    virtual void doKeyDown();                       // move down one line
    virtual void doKeyUp();                         // move up one line
    virtual void doKeyRight() { doJoyRight(); }
    virtual void doKeyLeft() { doJoyLeft(); }
    virtual void doPageDown();    // move down one page
    virtual void doPageUp();      // move up one page

    int selected = 0;           // the current selected line index
    int firstRow = 2;           // row 0 is the title.  this is the first row of the menu item lines
    bool changes = false;
//    bool cancelled = false;
    int offset = 0;             // y offset for the line I believe.  set by renderLogo()
};
