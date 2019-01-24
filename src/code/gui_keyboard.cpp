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
    gui->renderTextLine("-= " + label + " =-", 0, offset, true);
    gui->renderTextLine(result + "#", 1, offset, true);
    gui->renderLabelBox(1, offset);

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


            SDL_SetRenderDrawColor(renderer, 100, 100, 100, OCD_ALPHA);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer, &rectSelection);


            string text = rows[y][x];
            if (caps) {
                text = ucase(text);
            }

            gui->renderTextChar(text, 3 + y, offset, rectSelection.x + 10);

            if ((selx == x) && (sely == y)) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, OCD_ALPHA);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderDrawRect(renderer, &rectSelection);
            }

        }
    }


    gui->renderStatus("|@X| Select  |@T|  Delete  |@R1| Caps |@S| Space      |@Start| Confirm  |@O| Cancel |");
    SDL_RenderPresent(renderer);
}

void GuiKeyboard::loop() {

    bool menuVisible = true;
    while (menuVisible) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYBUTTONUP:
                    if (e.jbutton.button == PCS_BTN_R1) {
                        caps = false;
                        render();
                    }
                    break;
                case SDL_JOYBUTTONDOWN:

                    if (e.jbutton.button == PCS_BTN_R1) {
                        caps = true;
                        render();
                    }

                    if (e.jbutton.button == PCS_BTN_TRIANGLE) {
                        if (!result.empty())
                        {
                            result = result.substr(0,result.length()-1);
                        }
                        render();
                    }

                    if (e.jbutton.button == PCS_BTN_SQUARE) {

                            result += " ";

                        render();
                    }

                    if (e.jbutton.button == PCS_BTN_CROSS) {
                        string character = rows[sely][selx];
                        if (caps) {
                            result += ucase(character);

                        } else {
                            result += character;
                        }

                        render();
                    }

                    if (e.jbutton.button == PCS_BTN_START) {
                        cancelled = false;
                        menuVisible = false;

                    };
                    if (e.jbutton.button == PCS_BTN_CIRCLE) {
                        cancelled = true;
                        menuVisible = false;

                    };
                    break;
                case SDL_JOYAXISMOTION:
                    if (e.jaxis.axis == 0) {
                        if (e.jaxis.value > 3200) {
                            selx++;
                            if (selx > 10) {
                                selx = 10;
                            }
                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            selx--;
                            if (selx < 0) {
                                selx = 0;
                            }
                            render();
                        }
                    }
                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > 3200) {
                            sely++;
                            if (sely > 3) {
                                sely = 3;
                            }
                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            sely--;
                            if (sely < 0) {
                                sely = 0;
                            }
                            render();
                        }
                    }
                    break;


            }

        }
    }
}