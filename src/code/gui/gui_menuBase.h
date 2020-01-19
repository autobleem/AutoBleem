#pragma once

#include "gui_screen.h"
#include "gui.h"
#include <vector>
#include <string>

class GuiMenuBase : public GuiScreen {
public:
    GuiMenuBase(SDL_Shared<SDL_Renderer> _renderer, std::string _title = "")
            : GuiScreen(_renderer), title(_title) {}

    virtual void init() override;
    virtual void render() override;
//    virtual void loop() override;

    virtual std::string statusLine();   // returns the status line at the bottom if you need to override it

    bool firstRender = true;
    void renderLines();
    void renderSelectionBox();

    // controller dpad/joystick pressed
    virtual void doJoyDown();                           // move down one line, may fast forwward
    virtual void doJoyUp();                             // move up one line, may fast forwward

    // controller button pressed
    virtual void doCircle_Pressed();                    // default = leave menu.  cancel = true.
    virtual void doCross_Pressed();                     // default = leave menu.  cancel = false.
    virtual void doL1_Pressed() { doPageUp(); }         // default = page up
    virtual void doR1_Pressed() { doPageDown(); }       // default = page down
    virtual void doL2_Pressed() { doHome(); }           // default = home
    virtual void doR2_Pressed() { doEnd(); }            // default = end

    // keyboard
    virtual void doKeyDown();                           // move down one line
    virtual void doKeyUp();                             // move up one line
    virtual void doEnter() { doCross_Pressed(); }       // default = doCross
    virtual void doEscape() { doCircle_Pressed(); }     // default = doCircle
    virtual void doPageDown();
    virtual void doPageUp();
    virtual void doHome();
    virtual void doEnd();

    std::shared_ptr<Gui> gui;
    std::string title;
    TTF_Font_Shared font;

    enum MenuType { Menu_Plain, Menu_TwoColumns };
    MenuType menuType = Menu_Plain;
    bool useSmallerFont = false;    // useful for 2 column menu with long strings

    // plain menu
    std::vector<std::string> lines; // these are the menu lines
    virtual int getVerticalSize() { return lines.size(); }

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
    void adjustPageBy(int moveBy);  // move the page up or down by an amount
    void computePagePosition();     // complete recompute of positions based on the selected value

    bool changes=false;
    bool cancelled = false;
};
