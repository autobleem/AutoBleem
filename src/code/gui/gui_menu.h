#pragma once

#include "gui_screen.h"
#include "gui.h"
#include <vector>
#include <string>

class GuiMenu : public GuiScreen {
public:
    GuiMenu(SDL_Shared<SDL_Renderer> _renderer, std::string _title = "")
            : GuiScreen(_renderer), title(_title) {}

    virtual void init() override;
    virtual void render() override;
    virtual void loop() override;

    virtual std::string statusLine();   // returns the status line at the bottom if you need to override it

    void makeSelectedLineVisibleOnPage();
    void renderLines();
    void renderSelectionBox();

    //controller and keyboard
    virtual void arrowDown();   // move down one line
    virtual void arrowUp();     // move up one line
    virtual void pageDown();    // move down one page
    virtual void pageUp();      // move up one page

    // keyboard only
    virtual void doHome();      // move up top
    virtual void doEnd();       // move up bottom
    virtual void doEnter() { }
    virtual void doDelete() { }
    virtual void doTab() { }
    virtual void doEscape() { }

    // you can write your own virtual loop() to do something different from these
    virtual void doCircle();    // default = leave menu.  cancel = true.
    virtual void doCross();     // default = leave menu.  cancel = false.
    virtual void doTriangle() { }
    virtual void doSquare() { }
    virtual void doStart() { }
    virtual void doSelect() { }

    bool handlePowerShutdownAndQuit(SDL_Event &e);  // returns true if applicable event type and it was handled

    shared_ptr<Gui> gui;
    std::string title;

    enum MenuType { Menu_Plain, Menu_TwoColumns };
    MenuType menuType = Menu_Plain;

    // plain menu
    std::vector<std::string> lines; // these are the menu lines

    // two column menus only
    // left column lines_L uses lines for storage.  that way lines.size() works for both types of menus
    std::vector<std::string>& lines_L = lines;
    std::vector<std::string> lines_R;
    int xoffset_L = 0;
    int xoffset_R = 500;

    int selected = 0;           // the current selected line index
    int maxVisible = 8;         // the number of lines that can fit on the display (theme dependent)
    int firstVisibleIndex = 0;  // current visible range on page
    int lastVisibleIndex = 7;   // current visible range on page
    int firstRow = 2;           // row 0 is the title.  this is the first row of the menu item lines
    int offset = 0;             // y offset for the line I believe.  set by renderLogo()
    TTF_Font_Shared font;

    bool changes=false;
    bool cancelled = false;
    bool menuVisible = true;
};
