#include "gui_Menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <cassert>
#include "../lang.h"

using namespace std;

//*******************************
// void GuiMenu::init()
//*******************************
void GuiMenu::init()
{
    gui = Gui::getInstance();
    font = gui->themeFont;

    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisibleIndex = 0;
    lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
}

//*******************************
// GuiMenu::makeSelectedLineVisibleOnPage
//*******************************
void GuiMenu::makeSelectedLineVisibleOnPage() {
    if (selected >= 0 && lines.size() > 0) {
        if (selected >= lines.size()) {
            selected = lines.size() - 1;
        }

        if (selected < firstVisibleIndex) {
            int moveBy = firstVisibleIndex - selected;
            firstVisibleIndex -= moveBy;
            lastVisibleIndex -= moveBy;
        }
        if (selected > lastVisibleIndex) {
            int moveBy = selected - lastVisibleIndex;
            firstVisibleIndex += moveBy;
            lastVisibleIndex += moveBy;
        }
    }
}

//*******************************
// GuiMenu::renderLines
//*******************************
void GuiMenu::renderLines() {
    if (selected >= 0 && lines.size() > 0) {
        int row = firstRow;
        for (int i = firstVisibleIndex; i <= lastVisibleIndex; i++) {
            if (i < 0 || i >= lines.size()) {
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
// GuiMenu::renderSelectionBox
//*******************************
void GuiMenu::renderSelectionBox() {
    if (!lines.size() == 0) {
        gui->renderSelectionBox(selected - firstVisibleIndex + firstRow, offset);
    }
}

//*******************************
// GuiMenu::render
//*******************************
void GuiMenu::render()
{
    SDL_RenderClear(renderer);
    gui->renderBackground();
    gui->renderTextBar();
    offset = gui->renderLogo(true);
    gui->renderTextLine(title, 0, offset, POS_CENTER);

    makeSelectedLineVisibleOnPage();
    renderLines();
    renderSelectionBox();

    gui->renderStatus(statusLine());
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiMenu::statusLine
//*******************************
// the default status line for menus.  override if needed.
string GuiMenu::statusLine() {
    return _("Entry")+" " + to_string(selected + 1) + "/" + to_string(lines.size()) +
             "    |@L1|/|@R1| " + _("Page") +
             "   |@X| " + _("Select") +
             "   |@O| " + _("Close") + " |";
}

//*******************************
// GuiMenu::arrowDown
//*******************************
void GuiMenu::arrowDown() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected++;
    if (selected >= lines.size()) {
        selected = 0;
        firstVisibleIndex = selected;
        lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
    }
    render();
}

//*******************************
// GuiMenu::arrowUp
//*******************************
void GuiMenu::arrowUp() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected--;
    if (selected < 0) {
        selected = lines.size()-1;
        firstVisibleIndex = selected;
        lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
    }
    render();
}

//*******************************
// GuiMenu::pageDown
//*******************************
void GuiMenu::pageDown() {
    Mix_PlayChannel(-1, gui->home_up, 0);
    selected+=maxVisible;
    if (selected >= lines.size()) {
        selected = lines.size() - 1;
    }
    firstVisibleIndex = selected;
    lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
    render();
}

//*******************************
// GuiMenu::pageUp
//*******************************
void GuiMenu::pageUp() {
    Mix_PlayChannel(-1, gui->home_down, 0);
    selected-=maxVisible;
    if (selected < 0) {
        selected = 0;
    }
    firstVisibleIndex = selected;
    lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
    render();
}

//*******************************
// GuiMenu::doCircle
//*******************************
void GuiMenu::doCircle() {
    Mix_PlayChannel(-1, gui->cancel, 0);
    cancelled = true;
    menuVisible = false;
}

//*******************************
// GuiMenu::pagedoCrossUp
//*******************************
void GuiMenu::doCross() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    cancelled = false;
    if (!lines.empty())
    {
        menuVisible = false;
    }
}

//*******************************
// GuiMenu::loop
//*******************************
void GuiMenu::loop()
{
    menuVisible = true;
    while (menuVisible) {
        gui->watchJoystickPort();
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();
                }
            }
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYAXISMOTION:
                case SDL_JOYHATMOTION:

                    if (gui->mapper.isDown(&e)) {
                        arrowDown();
                    }
                    if (gui->mapper.isUp(&e)) {
                        arrowUp();
                    }

                    break;
                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_R1,&e)) {
                        pageDown();
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1,&e)) {
                        pageUp();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e)) {
                        doCircle();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS,&e)) {
                        doCross();
                    };
            }
        }
    }
}
