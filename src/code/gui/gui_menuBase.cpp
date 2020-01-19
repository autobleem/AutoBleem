#include "gui_MenuBase.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <cassert>
#include "../lang.h"

using namespace std;

//*******************************
// void GuiMenuBase::init()
//*******************************
void GuiMenuBase::init()
{
    gui = Gui::getInstance();
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
// GuiMenuBase::adjustPageBy
//*******************************
// move the page up or down by an amount
void GuiMenuBase::adjustPageBy(int moveBy) {
    selected += moveBy;
    firstVisibleIndex += moveBy;
    lastVisibleIndex += moveBy;
}

//*******************************
// GuiMenuBase::computePagePosition
//*******************************
// complete recompute of positions based on the selected value
void GuiMenuBase::computePagePosition() {
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
// GuiMenuBase::renderLines
//*******************************
void GuiMenuBase::renderLines() {
    if (selected >= 0 && getVerticalSize() > 0) {
        int row = firstRow;
        for (int i = firstVisibleIndex; i <= lastVisibleIndex; i++) {
            if (i < 0 || i >= getVerticalSize()) {
                break;
            }
            if (menuType == Menu_Plain) {
                gui->renderTextLine(lines[i], row, offset, POS_LEFT, 0, font);
            } else if (menuType == Menu_TwoColumns) {
                gui->renderTextLineToColumns(lines_L[i], lines_R[i], xoffset_L, xoffset_R, row, offset, font);
            } else {
                assert(false);
            }
            row++;
        }
    }
}

//*******************************
// GuiMenuBase::renderSelectionBox
//*******************************
void GuiMenuBase::renderSelectionBox() {
    if (!getVerticalSize() == 0) {
        gui->renderSelectionBox(selected - firstVisibleIndex + firstRow, offset, 0, font);
    }
}

//*******************************
// GuiMenuBase::render
//*******************************
void GuiMenuBase::render()
{
    SDL_RenderClear(renderer);
    gui->renderBackground();
    gui->renderTextBar();
    offset = gui->renderLogo(true);
    gui->renderTextLine(title, 0, offset, POS_CENTER);

    if (firstRender) {
        computePagePosition();
        firstRender = false;
    }
    renderLines();
    renderSelectionBox();

    gui->renderStatus(statusLine());
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiMenuBase::statusLine
//*******************************
// the default status line for menus.  override if needed.
string GuiMenuBase::statusLine() {
    return _("Entry")+" " + to_string(selected + 1) + "/" + to_string(getVerticalSize()) +
             "    |@L1|/|@R1| " + _("Page") +
             "   |@X| " + _("Select") +
             "   |@O| " + _("Close") + " |";
}

//*******************************
// GuiMenuBase::doKeyDown
//*******************************
void GuiMenuBase::doKeyDown() {
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
// GuiMenuBase::doKeyUp
//*******************************
void GuiMenuBase::doKeyUp() {
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
// GuiMenuBase::doJoyDown
//*******************************
void GuiMenuBase::doJoyDown() {
    do {
        doKeyDown();
        render();
    } while (fastForwardUntilAnotherEvent(100));
}

//*******************************
// GuiMenuBase::doJoyUp
//*******************************
void GuiMenuBase::doJoyUp() {
    do {
        doKeyUp();
        render();
    } while (fastForwardUntilAnotherEvent(100));
}

//*******************************
// GuiMenuBase::doPageDown
//*******************************
void GuiMenuBase::doPageDown() {
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
// GuiMenuBase::doPageUp
//*******************************
void GuiMenuBase::doPageUp() {
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
// GuiMenuBase::doHome
//*******************************
void GuiMenuBase::doHome() {
    Mix_PlayChannel(-1, gui->home_down, 0);
    if (getVerticalSize() > 1) {
        selected = 0;
        computePagePosition();
    }
}

//*******************************
// GuiMenuBase::doEnd
//*******************************
void GuiMenuBase::doEnd() {
    Mix_PlayChannel(-1, gui->home_down, 0);
    if (getVerticalSize() > 1) {
        selected = getVerticalSize() - 1;
        computePagePosition();
    }
}

//*******************************
// GuiMenuBase::doCircle_Pressed
//*******************************
void GuiMenuBase::doCircle_Pressed() {
    Mix_PlayChannel(-1, gui->cancel, 0);
    cancelled = true;
    menuVisible = false;
}

//*******************************
// GuiMenuBase::doCross_Pressed
//*******************************
void GuiMenuBase::doCross_Pressed() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    cancelled = false;
    if (!lines.empty())
    {
        menuVisible = false;
    }
}
