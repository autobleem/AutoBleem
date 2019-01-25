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



void GuiOptions::init()
{
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
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=Configuration=-",0,offset,true);
    gui->renderTextLine("AutoBleem Theme: " + gui->cfg.inifile.values["theme"], 1, offset);
    gui->renderTextLine("Menu Theme: " + gui->cfg.inifile.values["stheme"], 2, offset);
    gui->renderTextLine("Disable Theme BGM: " + gui->cfg.inifile.values["nomusic"], 3, offset);
    gui->renderTextLine("Process configuration on scan: " + gui->cfg.inifile.values["autoregion"], 4, offset);
    gui->renderTextLine("Mipmap patch: " + gui->cfg.inifile.values["mip"], 5, offset);
    gui->renderTextLine("Overmount PCSX: " + gui->cfg.inifile.values["pcsx"], 6, offset);
    gui->renderStatus("|@O| Go back|");

    gui->renderSelectionBox(selOption+1,offset);

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

                    if (e.jbutton.button == PCS_BTN_CIRCLE) {
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
                                init();
                                gui->loadAssets();
                            }

                            if (selOption == 1) {
                                string nextValue = getOption(sthemes, gui->cfg.inifile.values["stheme"], true);
                                gui->cfg.inifile.values["stheme"] = nextValue;

                            }
                            if (selOption == 2) {
                                string nextValue = getOption(nomusic, gui->cfg.inifile.values["nomusic"], true);
                                gui->cfg.inifile.values["nomusic"] = nextValue;
                                init();
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
                                init();
                                gui->loadAssets();
                            }
                            if (selOption == 1) {
                                string nextValue = getOption(sthemes, gui->cfg.inifile.values["stheme"], false);
                                gui->cfg.inifile.values["stheme"] = nextValue;

                            }
                            if (selOption == 2) {
                                string nextValue = getOption(nomusic, gui->cfg.inifile.values["nomusic"], false);
                                gui->cfg.inifile.values["nomusic"] = nextValue;
                                init();
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