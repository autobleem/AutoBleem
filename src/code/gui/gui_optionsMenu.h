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
    TTF_Font_Shared font;
    virtual int getVerticalSize() { return 0; }

    bool handlePowerShutdownAndQuit(SDL_Event &e);  // returns true if applicable event type and it was handled

    //controller and keyboard
    virtual void doArrowDown();   // move down one line
    virtual void doArrowUp();     // move up one line
    virtual void doPageDown();    // move down one page
    virtual void doPageUp();      // move up one page
    virtual void doArrowRight() {}  // move option to the right
    virtual void doArrowLeft() {}   // move option to the left

    // keyboard only
    virtual void doHome() {}    // move up top
    virtual void doEnd() {}     // move up bottom
    virtual void doEnter() { }
    virtual void doDelete() { }
    virtual void doTab() { }
    virtual void doEscape() { }

    virtual void doCircle() {}  // default = leave menu.  cancel = true.
    virtual void doCross() {}   // default = leave menu.  cancel = false.
    virtual void doTriangle() { }
    virtual void doSquare() { }
    virtual void doStart() { }
    virtual void doSelect() { }
    virtual void doL1() { }
    virtual void doR1() { }
    virtual void doL2() { }
    virtual void doR2() { }

    int selected = 0;           // the current selected line index
    int firstRow = 2;           // row 0 is the title.  this is the first row of the menu item lines
    bool changes = false;
//    bool cancelled = false;
    bool menuVisible = true;
    int offset = 0;             // y offset for the line I believe.  set by renderLogo()
};
