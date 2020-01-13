//
// Created by screemer on 2019-01-24.
//

#include "gui_keyboard.h"
#include "gui_about.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"
#include "../lang.h"
#include "../engine/scanner.h"
#include <iostream>

using namespace std;

vector<string> row0 = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
vector<string> row1 = {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"};
vector<string> row2 = {"a", "s", "d", "f", "g", "h", "j", "k", "l", "."};
vector<string> row3 = {"z", "x", "c", "v", "b", "n", "m", "_", "-", " "};

#define numColumns 10
#define numRows 4
#define xlast (numColumns-1)
#define ylast (numRows-1)
#define indentOffset 5

vector<vector<string>> rows = {row0, row1, row2, row3};

//*******************************
// GuiKeyboard::init
//*******************************
void GuiKeyboard::init() {
    gui = Gui::getInstance();
    cursorIndex = result.size(); // the "#" cursor position starts out at the end of the string
}

//*******************************
// GuiKeyboard::render
//*******************************
void GuiKeyboard::render() {
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderLabelBox(1, offset);
    gui->renderTextLine("-= " + label + " =-", 0, offset, POS_CENTER);

    //*******************************
    // drawRectangle lambda
    //*******************************
    auto drawRectangle = [&] (SDL_Rect& rect) {
        string fg = gui->themeData.values["text_fg"];
        SDL_SetRenderDrawColor(renderer, gui->getR(fg), gui->getG(fg), gui->getB(fg), 255);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_Rect rectSelection2;
        rectSelection2.x = rect.x + 1;
        rectSelection2.y = rect.y + 1;
        rectSelection2.w = rect.w - 2;
        rectSelection2.h = rect.h - 2;
        SDL_RenderDrawRect(renderer, &rectSelection2);
    };

    string displayResult = result;
    displayResult.insert(cursorIndex, "#");
    gui->renderTextLine(displayResult, 1, offset, POS_CENTER);

    SDL_Rect rect2;
    rect2.x = atoi(gui->themeData.values["opscreenx"].c_str());
    rect2.y = atoi(gui->themeData.values["opscreeny"].c_str());
    rect2.w = atoi(gui->themeData.values["opscreenw"].c_str());
    rect2.h = atoi(gui->themeData.values["opscreenh"].c_str());

    SDL_Shared<SDL_Texture> tex;
    SDL_Rect rect;
    gui->getTextAndRect(renderer, 0, 0, "*", gui->themeFont, &tex, &rect);

    if (L2_cursor_shift || usingUsbKeyboard) {
        SDL_Rect rectEditbox = gui->getTextRectangleOnScreen(displayResult, 1, offset, POS_CENTER, 0, gui->themeFont);

        // compute the bounding box around the cursor (#)
        SDL_Point textBeforeCursorSize { 0, 0 };
        if (cursorIndex > 0)    // get the size of the text before the cursor
            TTF_SizeText(gui->themeFont, displayResult.substr(0, cursorIndex).c_str(), &textBeforeCursorSize.x, &textBeforeCursorSize.y);
        SDL_Point cursorSize;
        TTF_SizeText(gui->themeFont, "#", &cursorSize.x, &cursorSize.y);    // get the cursor size
        // bounding box rectangle around the # cursor
        SDL_Rect cursorRect { rectEditbox.x + textBeforeCursorSize.x, rectEditbox.y,    // x, y position
                              cursorSize.x, cursorSize.y };                             //w, h

        drawRectangle(cursorRect);
    }

    if (!usingUsbKeyboard) {
        for (int x = 0; x < numColumns; x++) {
            for (int y = 0; y < numRows; y++) {
                SDL_Rect rectSelection;
                rectSelection.x = rect2.x + indentOffset;
                rectSelection.y = offset + rect.h * (y + 3);
                rectSelection.w = rect2.w - (indentOffset + indentOffset);
                rectSelection.h = rect.h;

                int buttonWidth = (rectSelection.w / 10) - (indentOffset + indentOffset);
                int buttonHeight = rectSelection.h - 2;

                rectSelection.w = buttonWidth;
                rectSelection.h = buttonHeight;

                rectSelection.x = rectSelection.x + ((buttonWidth + 11) * x);

                string bg = gui->themeData.values["key_bg"];
                SDL_SetRenderDrawColor(renderer, gui->getR(bg), gui->getG(bg), gui->getB(bg),
                                       atoi(gui->themeData.values["keyalpha"].c_str()));
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &rectSelection);

                string text = rows[y][x];
                if (L1_caps_shift) {
                    text = ucase(text);
                }

                gui->renderTextChar(text, 3 + y, offset, rectSelection.x + 10);

                // display rectangle around current character
                if (!L2_cursor_shift) { // don't draw rectangle if in move cursor mode
                    if ((selx == x) && (sely == y)) {
                        drawRectangle(rectSelection);
                    }
                }
            }
        }
    }

    if (usingUsbKeyboard) {
        gui->renderStatus(
                "|@Tab| " + _("Use Controller") + "  |@Enter| " + _("Confirm") +
                "  |@Esc| " + _("Cancel") + " |");
    } else {
        gui->renderStatus(
                "|@X| " + _("Select") + "  |@T|  " + _("Backspace") + "  |@L1| " + _("Caps") + "  |@L2| " +
                _("Move Cursor") + "(#)" + " |@S| " + _("Space") +
                "      |@Start| " + _("Confirm") + "  |@O| " + _("Cancel") + " |");
    }
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiKeyboard::handlePowerShutdownAndQuit
//*******************************
// returns true if applicable event type and it was handled
bool GuiKeyboard::handlePowerShutdownAndQuit(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
            gui->drawText(_("POWERING OFF... PLEASE WAIT"));
            Util::powerOff();
            return true;    // but it will never get here
        }
    } else if (e.type == SDL_QUIT) { // this is for pc Only
        menuVisible = false;
        return true;
    }
    return false;
}

//*******************************
// GuiKeyboard::doKbdRight
//*******************************
void GuiKeyboard::doKbdRight() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (L2_cursor_shift) {
        if (cursorIndex != result.size())
            ++cursorIndex;
    } else {
        selx++;
        if (selx > xlast) {
            selx = 0;
        }
    }
    L2_cursor_shift = true;
    usingUsbKeyboard = true;
    render();
}

//*******************************
// GuiKeyboard::doKbdLeft
//*******************************
void GuiKeyboard::doKbdLeft() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (L2_cursor_shift) {
        if (cursorIndex > 0)
            --cursorIndex;
    } else {
        selx--;
        if (selx < 0) {
            selx = xlast;
        }
    }
    L2_cursor_shift = true;
    usingUsbKeyboard = true;
    render();
}

//*******************************
// GuiKeyboard::doKbdBackspace
//*******************************
void GuiKeyboard::doKbdBackspace() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (!result.empty() && cursorIndex > 0) {
        result = result.erase(cursorIndex - 1, 1);
        --cursorIndex;
    }
    L2_cursor_shift = true;
    usingUsbKeyboard = true;
    render();
}

//*******************************
// GuiKeyboard::doKbdDelete
//*******************************
void GuiKeyboard::doKbdDelete() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (!result.empty() && cursorIndex < result.size()) {
        result = result.erase(cursorIndex, 1);
    }
    L2_cursor_shift = true;
    usingUsbKeyboard = true;
    render();
}

//*******************************
// GuiKeyboard::doKbdTab
//*******************************
void GuiKeyboard::doKbdTab() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    L2_cursor_shift = !L2_cursor_shift;
    usingUsbKeyboard = !usingUsbKeyboard;
    render();
}

//*******************************
// GuiKeyboard::doKbdEscape
//*******************************
void GuiKeyboard::doKbdEscape() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    cancelled = true;
    menuVisible = false;
}

//*******************************
// GuiKeyboard::doKbdReturn
//*******************************
void GuiKeyboard::doKbdReturn() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    cancelled = false;
    menuVisible = false;
}

//*******************************
// GuiKeyboard::doKbdTextInput
//*******************************
void GuiKeyboard::doKbdTextInput(SDL_Event& e) {
    Mix_PlayChannel(-1, gui->cursor, 0);
    result.insert(cursorIndex, e.text.text);
    cursorIndex += strlen(e.text.text);
    L2_cursor_shift = true;
    usingUsbKeyboard = true;
    render();
}

//*******************************
// GuiKeyboard::doL1_up
//*******************************
void GuiKeyboard::doL1_up() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    L1_caps_shift = false;
    render();
}

//*******************************
// GuiKeyboard::doL2_up
//*******************************
void GuiKeyboard::doL2_up() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    L2_cursor_shift = false;
    render();
}

//*******************************
// GuiKeyboard::doL1_down
//*******************************
void GuiKeyboard::doL1_down() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    L1_caps_shift = true;
    render();
}

//*******************************
// GuiKeyboard::doL2_down
//*******************************
void GuiKeyboard::doL2_down() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    L2_cursor_shift = true;
    render();
}

//*******************************
// GuiKeyboard::doTriangle
//*******************************
void GuiKeyboard::doTriangle() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (!result.empty() && cursorIndex > 0) {
        result = result.erase(cursorIndex - 1, 1);
        --cursorIndex;
    }
    render();
}

//*******************************
// GuiKeyboard::doSquare
//*******************************
void GuiKeyboard::doSquare() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    result.insert(cursorIndex, " ");
    ++cursorIndex;
    render();
}

//*******************************
// GuiKeyboard::doCross
//*******************************
void GuiKeyboard::doCross() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    string character = rows[sely][selx];
    string ch;
    if (L1_caps_shift)
        ch = ucase(character);
    else
        ch = character;
    result.insert(cursorIndex, ch);
    ++cursorIndex;
    render();
}

//*******************************
// GuiKeyboard::doStart
//*******************************
void GuiKeyboard::doStart() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    cancelled = false;
    menuVisible = false;
}

//*******************************
// GuiKeyboard::doCircle
//*******************************
void GuiKeyboard::doCircle() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    cancelled = true;
    menuVisible = false;
}

//*******************************
// GuiKeyboard::doRight
//*******************************
void GuiKeyboard::doRight() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (L2_cursor_shift) {
        if (cursorIndex != result.size())
            ++cursorIndex;
    } else {
        selx++;
        if (selx > xlast) {
            selx = 0;
        }
    }
    render();
}

//*******************************
// GuiKeyboard::doLeft
//*******************************
void GuiKeyboard::doLeft() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (L2_cursor_shift) {
        if (cursorIndex > 0)
            --cursorIndex;
    } else {
        selx--;
        if (selx < 0) {
            selx = xlast;
        }
    }
    render();
}

//*******************************
// GuiKeyboard::doDown
//*******************************
void GuiKeyboard::doDown() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (!L2_cursor_shift) {
        sely++;
        if (sely > ylast) {
            sely = 0;
        }
    }
    render();
}

//*******************************
// GuiKeyboard::doUp
//*******************************
void GuiKeyboard::doUp() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (!L2_cursor_shift) {
        sely--;
        if (sely < 0) {
            sely = ylast;
        }
    }
    render();
}

//*******************************
// GuiKeyboard::loop
//*******************************
void GuiKeyboard::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());

    menuVisible = true;
    while (menuVisible) {
        gui->watchJoystickPort();
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (handlePowerShutdownAndQuit(e))
                continue;

            switch (e.type) {
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_RIGHT) {
                        doKbdRight();

                    } else if (e.key.keysym.sym == SDLK_LEFT) {
                        doKbdLeft();

                    } else if (e.key.keysym.sym == SDLK_BACKSPACE) {
                        doKbdBackspace();

                    } else if (e.key.keysym.sym == SDLK_DELETE) {
                        doKbdDelete();

                    } else if (e.key.keysym.sym == SDLK_TAB) {
                        doKbdTab();

                    } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                        doKbdEscape();

                    } else if (e.key.keysym.sym == SDLK_RETURN) {
                        doKbdReturn();
                    }
                    break;

                case SDL_TEXTINPUT:
                    doKbdTextInput(e);
                    break;

                case SDL_JOYBUTTONUP:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {
                        doL1_up();
                    } else if (e.jbutton.button == gui->_cb(PCS_BTN_L2, &e)) {
                        doL2_up();
                    }
                    break;

                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {     // caps shift
                        doL1_down();
                    } else if (e.jbutton.button == gui->_cb(PCS_BTN_L2, &e)) {     // move cursor shift
                        doL2_down();
                    }

                    if (!L2_cursor_shift) {
                        if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE, &e)) {   // delete char on the left
                            doTriangle();
                        } else if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE, &e)) {     //insert space
                            doSquare();
                        } else if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {
                            doCross();
                        } else if (e.jbutton.button == gui->_cb(PCS_BTN_START, &e)) {  // Confirm
                            doStart();
                        } else if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) { // Cancel
                            doCircle();
                        }
                    }
                    break;

                case SDL_JOYAXISMOTION:
                case SDL_JOYHATMOTION:
                    if (gui->mapper.isRight(&e)) {
                        doRight();
                    } else if (gui->mapper.isLeft(&e)) {
                        doLeft();
                    }

                    if (!L2_cursor_shift) {
                        if (gui->mapper.isDown(&e)) {
                            doDown();
                        } else if (gui->mapper.isUp(&e)) {
                            doUp();
                        }
                    }

                    break;
            }
        }
    }
}