//
// Created by screemer on 2019-01-24.
//

#include "gui_options.h"
#include "util.h"
#include "gui.h"


string GuiOptions::getOption(vector<string> list, string current, bool next) {
    int pos = 0;
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == current) {
            pos = i;
        }
    }

    if (next) {
        pos++;
        if (pos >= list.size()) {
            pos = list.size() - 1;
        }
    } else {
        pos--;
        if (pos < 0) pos = 0;
    }

    return list[pos];
}

void GuiOptions::renderMenuOption(string text, int line, SDL_Rect rect2, SDL_Rect logoRect, SDL_Rect textRec) {
    shared_ptr<Gui> gui(Gui::getInstance());
    SDL_Texture *textTex;
    gui->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * line,
                   text.c_str(), gui->font, &textTex,
                   &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
}


void GuiOptions::init(SDL_Renderer *renderer1)
{
    this->renderer = renderer1;
    themes.clear();
    sthemes.clear();
    sthemes.push_back("default");
    vector<DirEntry> folders = Util::diru(Util::getWorkingPath() + Util::separator() + "theme");
    for (DirEntry entry:folders) {
        themes.push_back(entry.name);
    }
    folders = Util::diru("/media/themes");
    for (DirEntry entry:folders) {
        sthemes.push_back(entry.name);
    }
    pcsx.clear();
    pcsx.push_back("original");
    pcsx.push_back("bleemsync");
    mip.clear();
    mip.push_back("true");
    mip.push_back("false");
    nomusic.clear();
    nomusic.push_back("true");
    nomusic.push_back("false");
    autoregion.clear();
    autoregion.push_back("true");
    autoregion.push_back("false");
}
void GuiOptions::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());

    SDL_Texture *textTex;
    SDL_Rect textRec;
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, gui->backgroundImg, NULL, &gui->backgroundRect);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, gui->backgroundImg, NULL, &gui->backgroundRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(gui->themeData.values["textx"].c_str());
    rect.y = atoi(gui->themeData.values["texty"].c_str());
    rect.w = atoi(gui->themeData.values["textw"].c_str());
    rect.h = atoi(gui->themeData.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    SDL_Rect rect2;
    rect2.x = atoi(gui->themeData.values["opscreenx"].c_str());
    rect2.y = atoi(gui->themeData.values["opscreeny"].c_str());
    rect2.w = atoi(gui->themeData.values["opscreenw"].c_str());
    rect2.h = atoi(gui->themeData.values["opscreenh"].c_str());
    SDL_RenderFillRect(renderer, &rect2);

    SDL_Rect logoRect;
    logoRect.x = rect2.x;
    logoRect.y = rect2.y;
    logoRect.w = gui->logoRect.w / 3;
    logoRect.h = gui->logoRect.h / 3;
    SDL_RenderCopy(renderer, gui->logo, NULL, &logoRect);

    gui->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 0,
                   "Configuration:", gui->font, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);

    renderMenuOption("AutoBleem Theme: " + gui->cfg.inifile.values["theme"], 1, rect2, logoRect, textRec);
    renderMenuOption("Menu Theme: " + gui->cfg.inifile.values["stheme"], 2, rect2, logoRect, textRec);
    renderMenuOption("Disable Theme BGM: " + gui->cfg.inifile.values["nomusic"], 3, rect2, logoRect, textRec);
    renderMenuOption("Process configuration on scan: " + gui->cfg.inifile.values["autoregion"], 4, rect2, logoRect, textRec);
    renderMenuOption("Mipmap patch: " + gui->cfg.inifile.values["mip"], 5, rect2, logoRect, textRec);
    renderMenuOption("Overmount PCSX: " + gui->cfg.inifile.values["pcsx"], 6, rect2, logoRect, textRec);


    gui->getEmojiTextTexture(renderer, "Press |@X| to go back|", gui->font, &textTex,
                        &textRec);

    textRec.y = atoi(gui->themeData.values["ttop"].c_str());


    if (textRec.w > atoi(gui->themeData.values["textw"].c_str())) textRec.w = atoi(gui->themeData.values["textw"].c_str());
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);


    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 10;
    rectSelection.y = logoRect.y + logoRect.h + textRec.h * (selOption + 1);
    rectSelection.w = rect2.w - 20;
    rectSelection.h = textRec.h;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderDrawRect(renderer, &rectSelection);

    SDL_RenderPresent(renderer);
}

void GuiOptions::loop()
{
    shared_ptr<Gui> gui(Gui::getInstance());

    render();

    bool menuVisible = true;
    while (menuVisible) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYBUTTONUP:  /* Handle Joystick Button Presses */

                    if (e.jbutton.button == 2) {
                        gui->cfg.save();
                        menuVisible = false;
                    };
                    break;
                case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > 3200) {
                            selOption++;
                            if (selOption > 5) {
                                selOption = 5;
                            }
                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            selOption--;
                            if (selOption < 0) {
                                selOption = 0;
                            }
                            render();
                        }
                    }

                    if (e.jaxis.axis == 0) {
                        if (e.jaxis.value > 3200) {
                            if (selOption == 0) {
                                string nextValue = getOption(themes, gui->cfg.inifile.values["theme"], true);
                                gui->cfg.inifile.values["theme"] = nextValue;
                                init(renderer);
                                gui->loadAssets();
                            }

                            if (selOption == 1) {
                                string nextValue = getOption(sthemes, gui->cfg.inifile.values["stheme"], true);
                                gui->cfg.inifile.values["stheme"] = nextValue;

                            }
                            if (selOption == 2) {
                                string nextValue = getOption(nomusic, gui->cfg.inifile.values["nomusic"], true);
                                gui->cfg.inifile.values["nomusic"] = nextValue;
                                init(renderer);
                                gui->loadAssets();

                            }
                            if (selOption == 3) {
                                string nextValue = getOption(autoregion, gui->cfg.inifile.values["autoregion"], true);
                                gui->cfg.inifile.values["autoregion"] = nextValue;


                            }
                            if (selOption == 4) {
                                string nextValue = getOption(mip, gui->cfg.inifile.values["mip"], true);
                                gui->cfg.inifile.values["mip"] = nextValue;


                            }
                            if (selOption == 5) {
                                string nextValue = getOption(pcsx, gui->cfg.inifile.values["pcsx"], true);
                                gui->cfg.inifile.values["pcsx"] = nextValue;


                            }
                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            if (selOption == 0) {
                                string nextValue = getOption(themes, gui->cfg.inifile.values["theme"], false);
                                gui->cfg.inifile.values["theme"] = nextValue;
                                init(renderer);
                                gui->loadAssets();
                            }
                            if (selOption == 1) {
                                string nextValue = getOption(sthemes, gui->cfg.inifile.values["stheme"], false);
                                gui->cfg.inifile.values["stheme"] = nextValue;

                            }
                            if (selOption == 2) {
                                string nextValue = getOption(nomusic, gui->cfg.inifile.values["nomusic"], false);
                                gui->cfg.inifile.values["nomusic"] = nextValue;
                                init(renderer);
                                gui->loadAssets();

                            }
                            if (selOption == 3) {
                                string nextValue = getOption(autoregion, gui->cfg.inifile.values["autoregion"], false);
                                gui->cfg.inifile.values["autoregion"] = nextValue;

                            }
                            if (selOption == 4) {
                                string nextValue = getOption(mip, gui->cfg.inifile.values["mip"], false);
                                gui->cfg.inifile.values["mip"] = nextValue;

                            }
                            if (selOption == 5) {
                                string nextValue = getOption(pcsx, gui->cfg.inifile.values["pcsx"], false);
                                gui->cfg.inifile.values["pcsx"] = nextValue;

                            }
                            render();
                        }
                    }


                    break;


            }

        }
    }
}