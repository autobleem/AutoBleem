#pragma once

#include "../gui_screen.h"
#include "../gui.h"
#include <vector>
#include <string>
#include "../../lang.h"
#include "../gui_sdl_wrapper.h"

//*******************************
// GuiMenuBase template class
//*******************************
template<typename LineDataType>
class GuiMenuBase : public GuiScreen {
public:
    GuiMenuBase(SDL_Shared<SDL_Renderer> _renderer) : GuiScreen(_renderer) {}

    virtual void init();
    virtual void render();

    virtual std::string getTitle();
    virtual std::string getStatusLine();   // returns the status line at the bottom.  cross, circle, etc icons.

    bool firstRender = true;
    virtual void renderLineIndexOnRow(int index, int row) {}  // you must inherit from GuiMenuBase and provide this
    void renderLines();
    void renderSelectionBox();

    // controller dpad/joystick pressed
    virtual void doJoyDown();                           // move down one line, may fast forwward
    virtual void doJoyUp();                             // move up one line, may fast forwward

    // controller button pressed
    virtual void doCircle_Pressed();                    // default = leave menu.  cancel = true.
    virtual void doCross_Pressed();                     // default = leave menu.  cancel = false.

    // horizontal lists of choices like the options menu will probably override these virtuals
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

    TTF_Font_Shared font;
    bool useSmallerFont = false;    // useful for 2 column menu with long strings

    // plain menu
    std::vector<LineDataType> lines; // these are the menu lines
    virtual int getVerticalSize() { return lines.size(); }

    int selected = 0;               // the current selected index
    int maxVisible = 8;             // the number of lines that can fit on the display (theme dependent)
    int firstVisibleIndex = 0;      // current visible range on page
    int lastVisibleIndex = 7;       // current visible range on page
    int firstRow = 2;               // row 0 is the title.  this is the first row of the menu item lines
    int offset = 0;                 // y offset for the line I believe.  set by renderLogo()
    void adjustPageBy(int moveBy);  // move the page up or down by an amount
    void computePagePosition();     // complete recompute of positions based on the selected value

    bool changes=false;
    bool cancelled = false;
};

//*******************************
// void GuiMenuBase<LineDataType>::init()
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::init()
{
    font = gui->themeFont;

    maxVisible = atoi(gui->themeData.values["lines"].c_str());

    if (useSmallerFont) {
        // sometimes the left column will overwrite into the right column.
        // and the second column sometimes go off the right side.
        font = gui->themeFonts[FONT_15_BOLD];   // use a smaller font
        // compute the larger number of rows we can now display
        string themeFontSizeString = gui->themeData.values["fsize"];
        int themeFontSize = atoi(themeFontSizeString.c_str());
        maxVisible = ( ((float)themeFontSize) / ((float)15) ) * ((float) maxVisible);
        lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
    }
}

//*******************************
// GuiMenuBase<T>::adjustPageBy
//*******************************
// move the page up or down by an amount
template<typename LineDataType>
void GuiMenuBase<LineDataType>::adjustPageBy(int moveBy) {
    selected += moveBy;
    firstVisibleIndex += moveBy;
    lastVisibleIndex += moveBy;
}

//*******************************
// GuiMenuBase<LineDataType>::computePagePosition
//*******************************
// complete recompute of positions based on the selected value
template<typename LineDataType>
void GuiMenuBase<LineDataType>::computePagePosition() {
    if (getVerticalSize() == 0) {
        selected = 0;
        firstVisibleIndex = 0;
        lastVisibleIndex = 0;
    } else {
        bool AllLinesFitOnOnePage = getVerticalSize() <= maxVisible;
        bool selectedIsOnTheFirstPage = selected < maxVisible;
        bool selectedIsOnTheLastPage = selected >= (getVerticalSize() - maxVisible);

        if (AllLinesFitOnOnePage) {
            firstVisibleIndex = 0;
        } else if (selectedIsOnTheFirstPage) {
            firstVisibleIndex = 0;
        } else if (selectedIsOnTheLastPage) {
            firstVisibleIndex = getVerticalSize() - maxVisible;
        } else {
            firstVisibleIndex = selected - (maxVisible / 2);
        }
        lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
    }
}

//*******************************
// GuiMenuBase<LineDataType>::renderLines
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::renderLines() {
    if (selected >= 0 && getVerticalSize() > 0) {
        int row = firstRow;
        for (int i = firstVisibleIndex; i <= lastVisibleIndex; i++) {
            if (i < 0 || i >= getVerticalSize()) {
                break;
            }
            renderLineIndexOnRow(i, row);  // call virtual that knows how to display the data
            row++;
        }
    }
}

//*******************************
// GuiMenuBase<LineDataType>::renderSelectionBox
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::renderSelectionBox() {
    if (!getVerticalSize() == 0) {
        gui->renderSelectionBox(selected - firstVisibleIndex + firstRow, offset, 0, font);
    }
}

//*******************************
// GuiMenuBase<LineDataType>::render
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::render()
{
    SDL_RenderClear(renderer);
    gui->renderBackground();
    gui->renderTextBar();
    offset = gui->renderLogo(true);
    gui->renderTextLine(getTitle(), 0, offset, POS_CENTER);

    if (firstRender) {
        computePagePosition();
        firstRender = false;
    }
    renderLines();
    renderSelectionBox();

    gui->renderStatus(getStatusLine());
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiMenuBase<LineDataType>::getTitle
//*******************************
template<typename LineDataType>
std::string GuiMenuBase<LineDataType>::getTitle() {
    return "****** MISSING TITLE ******";
}

//*******************************
// GuiMenuBase<LineDataType>::getStatusLine
//*******************************
// the default status line for menus.  override if needed.
template<typename LineDataType>
std::string GuiMenuBase<LineDataType>::getStatusLine() {
    return _("Entry")+" " + to_string(selected + 1) + "/" + to_string(getVerticalSize()) +
           "    |@L1|/|@R1| " + _("Page") +
           "   |@X| " + _("Select") +
           "   |@O| " + _("Close") + " |";
}

//*******************************
// GuiMenuBase<LineDataType>::doKeyDown
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doKeyDown() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (getVerticalSize() > 1) {
        if (selected >= getVerticalSize() - 1) {
            selected = 0;
            computePagePosition();
        } else if (selected == lastVisibleIndex) {
            adjustPageBy(1);
        } else {
            ++selected;
        }
    }
}

//*******************************
// GuiMenuBase<LineDataType>::doKeyUp
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doKeyUp() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (getVerticalSize() > 1) {
        if (selected <= 0) {
            selected = getVerticalSize() - 1;
            computePagePosition();
        } else if (selected == firstVisibleIndex) {
            adjustPageBy(-1);
        } else {
            --selected;
        }
    }
}

//*******************************
// GuiMenuBase<LineDataType>::doJoyDown
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doJoyDown() {
    do {
        doKeyDown();
        render();
    } while (fastForwardUntilAnotherEvent());
}

//*******************************
// GuiMenuBase<LineDataType>::doJoyUp
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doJoyUp() {
    do {
        doKeyUp();
        render();
    } while (fastForwardUntilAnotherEvent());
}

//*******************************
// GuiMenuBase<LineDataType>::doPageDown
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doPageDown() {
    Mix_PlayChannel(-1, gui->home_up, 0);
    if (getVerticalSize() > 1) {
        if (lastVisibleIndex + maxVisible >= getVerticalSize()) {
            selected = getVerticalSize() - 1;
            computePagePosition();
        } else {
            adjustPageBy(maxVisible);
        }
    }
}

//*******************************
// GuiMenuBase<LineDataType>::doPageUp
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doPageUp() {
    Mix_PlayChannel(-1, gui->home_down, 0);
    if (getVerticalSize() > 1) {
        if (firstVisibleIndex - maxVisible < 0) {
            selected = 0;
            computePagePosition();
        } else {
            adjustPageBy(-maxVisible);
        }
    }
}

//*******************************
// GuiMenuBase<LineDataType>::doHome
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doHome() {
    Mix_PlayChannel(-1, gui->home_down, 0);
    if (getVerticalSize() > 1) {
        selected = 0;
        computePagePosition();
    }
}

//*******************************
// GuiMenuBase<LineDataType>::doEnd
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doEnd() {
    Mix_PlayChannel(-1, gui->home_down, 0);
    if (getVerticalSize() > 1) {
        selected = getVerticalSize() - 1;
        computePagePosition();
    }
}

//*******************************
// GuiMenuBase::doCircle_Pressed
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doCircle_Pressed() {
    Mix_PlayChannel(-1, gui->cancel, 0);
    cancelled = true;
    menuVisible = false;
}

//*******************************
// GuiMenuBase<LineDataType>::doCross_Pressed
//*******************************
template<typename LineDataType>
void GuiMenuBase<LineDataType>::doCross_Pressed() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    cancelled = false;
    if (!lines.empty())
    {
        menuVisible = false;
    }
}
