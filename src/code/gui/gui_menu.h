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

    bool firstRender = true;
    void renderLines();
    void renderSelectionBox();

    // controller button pressed
    virtual void doJoyDown_Pressed();       // move down one line
    virtual void doJoyUp_Pressed();         // move up one line
    virtual void doJoyRight_Pressed() {}
    virtual void doJoyLeft_Pressed() {}

    virtual void doCircle_Pressed();        // default = leave menu.  cancel = true.
    virtual void doCross_Pressed();         // default = leave menu.  cancel = false.
    virtual void doTriangle_Pressed() {}
    virtual void doSquare_Pressed() {}
    virtual void doStart_Pressed() {}
    virtual void doSelect_Pressed() {}

    virtual void doL1_Pressed() {}
    virtual void doR1_Pressed() {}
    virtual void doL2_Pressed() {}
    virtual void doR2_Pressed() {}

    // controller button released
    virtual void doJoyDown_Released() {}
    virtual void doJoyUp_Released() {}
    virtual void doJoyRight_Released() {}
    virtual void doJoyLeft_Released() {}

    virtual void doCircle_Released() {}
    virtual void doCross_Released() {}
    virtual void doTriangle_Released() {}
    virtual void doSquare_Released() {}
    virtual void doStart_Released() {}
    virtual void doSelect_Released() {}

    virtual void doL1_Released() {}
    virtual void doR1_Released() {}
    virtual void doL2_Released() {}
    virtual void doR2_Released() {}

    // keyboard
    virtual void doKeyDown() { doJoyDown_Pressed(); }       // move down one line
    virtual void doKeyUp() { doJoyUp_Pressed(); }           // move up one line
    virtual void doKeyRight() { doJoyRight_Pressed(); }
    virtual void doKeyLeft() { doJoyLeft_Pressed(); }
    virtual void doPageDown();                              // move down one page
    virtual void doPageUp();                                // move up one page
    virtual void doHome();                                  // move up top
    virtual void doEnd();                                   // move up bottom
    virtual void doEnter() {}
    virtual void doDelete() {}
    virtual void doTab() {}
    virtual void doEscape() {}

    bool handlePowerShutdownAndQuit(SDL_Event &e);  // returns true if applicable event type and it was handled

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
    void adjustPageBy(int moveBy);
    void computePagePosition();

    bool changes=false;
    bool cancelled = false;
    bool menuVisible = true;
};
