#pragma once

#include "gui_screen.h"
#include "gui.h"
#include <vector>
#include <string>

class GuiOptionsMenu : public GuiScreen {
public:
    GuiOptionsMenu(SDL_Shared<SDL_Renderer> _renderer, std::string _title = "")
        : GuiScreen(_renderer), title(_title) {}

    virtual void init() override;
    virtual void render() override {}
    virtual void loop() override;

    virtual std::string statusLine();   // returns the status line at the bottom if you need to override it

    std::shared_ptr<Gui> gui;
    std::string title;

    virtual int getVerticalSize() { return 0; }

    bool handlePowerShutdownAndQuit(SDL_Event &e);  // returns true if applicable event type and it was handled

    //controller and keyboard
    virtual void doJoyDown();   // move down one line
    virtual void doJoyUp();     // move up one line
    virtual void doJoyRight() {}  // move option to the right
    virtual void doJoyLeft() {}   // move option to the left

    virtual void doCircle() {}
    virtual void doCross() {}
    virtual void doTriangle() { }
    virtual void doSquare() { }
    virtual void doStart() { }
    virtual void doSelect() { }

    virtual void doL1() { }
    virtual void doR1() { }
    virtual void doL2() { }
    virtual void doR2() { }

    // keyboard
    virtual void doKeyDown() { doJoyDown(); }      // move down one line
    virtual void doKeyUp() { doJoyUp(); }          // move up one line
    virtual void doKeyRight() { doJoyRight(); }
    virtual void doKeyLeft() { doJoyLeft(); }
    virtual void doPageDown();    // move down one page
    virtual void doPageUp();      // move up one page
    virtual void doHome() {}    // move up top
    virtual void doEnd() {}     // move up bottom
    virtual void doEnter() { }
    virtual void doDelete() { }
    virtual void doTab() { }
    virtual void doEscape() { }

    int selected = 0;           // the current selected line index
    int firstRow = 2;           // row 0 is the title.  this is the first row of the menu item lines
    bool changes = false;
//    bool cancelled = false;
    bool menuVisible = true;
    int offset = 0;             // y offset for the line I believe.  set by renderLogo()
};
