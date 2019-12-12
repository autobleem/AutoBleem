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
#include "../engine/scanner.h"
using namespace std;

vector<string> row0 = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
vector<string> row1 = {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"};
vector<string> row2 = {"a", "s", "d", "f", "g", "h", "j", "k", "l", "."};
vector<string> row3 = {"z", "x", "c", "v", "b", "n", "m", "_", "-", " "};

vector<vector<string>> rows = {row0, row1, row2, row3};

//*******************************
// GuiKeyboard::render
//*******************************
void GuiKeyboard::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    GfxTheme::drawBackground();
    gui->renderTextBar();
    int offset = GfxTheme::drawLogo(true);
    gui->renderLabelBox(1, offset);
    gui->renderTextLine("-= " + label + " =-", 0, offset, POS_CENTER);
    gui->renderTextLine(result + "#", 1, offset, POS_CENTER);

    SDL_Rect rect2;
    rect2.x = atoi(GfxTheme::get("opscreenx").c_str());
    rect2.y = atoi(GfxTheme::get("opscreeny").c_str());
    rect2.w = atoi(GfxTheme::get("opscreenw").c_str());
    rect2.h = atoi(GfxTheme::get("opscreenh").c_str());

    GfxImage  tex;
    SDL_Rect rect;
    gui->getTextAndRect(Application::renderer, 0, 0, "*", gui->themeFont, &tex, &rect);

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 4; y++) {
            SDL_Rect rectSelection;
            rectSelection.x = rect2.x + 5;
            rectSelection.y = offset + rect.h * (y + 3);
            rectSelection.w = rect2.w - 10;
            rectSelection.h = rect.h;

            int buttonWidth = (rectSelection.w / 10) - 10;
            int buttonHeight = rectSelection.h - 2;

            rectSelection.w = buttonWidth;
            rectSelection.h = buttonHeight;

            rectSelection.x = rectSelection.x + ((buttonWidth + 11) * x);

            string bg = GfxTheme::get("key_bg");
            SDL_SetRenderDrawColor(Application::renderer, gui->getR(bg), gui->getG(bg), gui->getB(bg),
                                   atoi(GfxTheme::get("keyalpha").c_str()));
            SDL_SetRenderDrawBlendMode(Application::renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(Application::renderer, &rectSelection);

            string text = rows[y][x];
            if (caps) {
                text = ucase(text);
            }

            gui->renderTextChar(text, 3 + y, offset, rectSelection.x + 10);

            if ((selx == x) && (sely == y)) {
                string fg = GfxTheme::get("text_fg");
                SDL_SetRenderDrawColor(Application::renderer, gui->getR(fg), gui->getG(fg), gui->getB(fg), 255);
                SDL_SetRenderDrawBlendMode(Application::renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderDrawRect(Application::renderer, &rectSelection);
                SDL_Rect rectSelection2;
                rectSelection2.x = rectSelection.x + 1;
                rectSelection2.y = rectSelection.y + 1;
                rectSelection2.w = rectSelection.w - 2;
                rectSelection2.h = rectSelection.h - 2;
                SDL_RenderDrawRect(Application::renderer, &rectSelection2);
            }
        }
    }

    gui->renderStatus(
            "|@X| " + _("Select") + "  |@T|  " + _("Delete") + "  |@L1| " + _("Caps") + " |@S| " + _("Space") +
            "      |@Start| " + _("Confirm") + "  |@O| " + _("Cancel") + " |");
    Gfx::flip();
}

//*******************************
// GuiKeyboard::loop
//*******************************
void GuiKeyboard::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());

    bool menuVisible = true;
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
                case SDL_JOYBUTTONUP:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        caps = false;
                        render();
                    }
                    break;
                case SDL_JOYBUTTONDOWN:

                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        caps = true;
                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (!result.empty()) {
                            result = result.substr(0, result.length() - 1);
                        }
                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        result += " ";

                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        string character = rows[sely][selx];
                        if (caps) {
                            result += ucase(character);

                        } else {
                            result += character;
                        }

                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_START, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        cancelled = false;
                        menuVisible = false;
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        cancelled = true;
                        menuVisible = false;
                    };
                    break;
                case SDL_JOYAXISMOTION:
                case SDL_JOYHATMOTION:

                    if (gui->mapper.isRight(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selx++;
                        if (selx > 9) {
                            selx = 0;
                        }
                        render();
                    }
                    if (gui->mapper.isLeft(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selx--;
                        if (selx < 0) {
                            selx = 9;
                        }
                        render();
                    }

                    if (gui->mapper.isDown(&e)) {

                        Mix_PlayChannel(-1, gui->cursor, 0);
                        sely++;
                        if (sely > 3) {
                            sely = 0;
                        }
                        render();
                    }
                    if (gui->mapper.isUp(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        sely--;
                        if (sely < 0) {
                            sely = 3;
                        }
                        render();
                    }

                    break;
            }
        }
    }
}