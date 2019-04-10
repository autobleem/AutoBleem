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

vector<string> row0 = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
vector<string> row1 = {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"};
vector<string> row2 = {"a", "s", "d", "f", "g", "h", "j", "k", "l", "."};
vector<string> row3 = {"z", "x", "c", "v", "b", "n", "m", "_", "-", " "};

vector<vector<string>> rows = {row0, row1, row2, row3};

void GuiKeyboard::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderLabelBox(1, offset);
    gui->renderTextLine("-= " + label + " =-", 0, offset, true);
    gui->renderTextLine(result + "#", 1, offset, true);


    SDL_Rect rect2;
    rect2.x = atoi(gui->themeData.values["opscreenx"].c_str());
    rect2.y = atoi(gui->themeData.values["opscreeny"].c_str());
    rect2.w = atoi(gui->themeData.values["opscreenw"].c_str());
    rect2.h = atoi(gui->themeData.values["opscreenh"].c_str());


    SDL_Texture *tex;
    SDL_Rect rect;
    gui->getTextAndRect(renderer, 0, 0, "*", gui->font, &tex, &rect);
    SDL_DestroyTexture(tex);


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


            string bg = gui->themeData.values["key_bg"];
            SDL_SetRenderDrawColor(renderer, gui->getR(bg), gui->getG(bg), gui->getB(bg),
                                   atoi(gui->themeData.values["keyalpha"].c_str()));
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer, &rectSelection);


            string text = rows[y][x];
            if (caps) {
                text = ucase(text);
            }

            gui->renderTextChar(text, 3 + y, offset, rectSelection.x + 10);

            if ((selx == x) && (sely == y)) {
                string fg = gui->themeData.values["text_fg"];
                SDL_SetRenderDrawColor(renderer, gui->getR(fg), gui->getG(fg), gui->getB(fg), 255);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderDrawRect(renderer, &rectSelection);
                SDL_Rect rectSelection2;
                rectSelection2.x = rectSelection.x + 1;
                rectSelection2.y = rectSelection.y + 1;
                rectSelection2.w = rectSelection.w - 2;
                rectSelection2.h = rectSelection.h - 2;
                SDL_RenderDrawRect(renderer, &rectSelection2);

            }

        }
    }


    gui->renderStatus(
            "|@X| " + _("Select") + "  |@T|  " + _("Delete") + "  |@L1| " + _("Caps") + " |@S| " + _("Space") +
            "      |@Start| " + _("Confirm") + "  |@O| " + _("Cancel") + " |");
    SDL_RenderPresent(renderer);
}

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
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        caps = false;
                        render();
                    }
                    break;
                case SDL_JOYBUTTONDOWN:

                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        caps = true;
                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (!result.empty()) {
                            result = result.substr(0, result.length() - 1);
                        }
                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        result += " ";

                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        string character = rows[sely][selx];
                        if (caps) {
                            result += ucase(character);

                        } else {
                            result += character;
                        }

                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_START,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        cancelled = false;
                        menuVisible = false;

                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        cancelled = true;
                        menuVisible = false;

                    };
                    break;
                case SDL_JOYAXISMOTION:
                    if (e.jaxis.axis == 0) {
                        if (e.jaxis.value > PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            selx++;
                            if (selx > 9) {
                                selx = 0;
                            }
                            render();
                        }
                        if (e.jaxis.value < -PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            selx--;
                            if (selx < 0) {
                                selx = 9;
                            }
                            render();
                        }
                    }
                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            sely++;
                            if (sely > 3) {
                                sely = 0;
                            }
                            render();
                        }
                        if (e.jaxis.value < -PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            sely--;
                            if (sely < 0) {
                                sely = 3;
                            }
                            render();
                        }
                    }
                    break;


            }

        }
    }
}