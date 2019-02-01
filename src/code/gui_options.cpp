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
    retroarch.clear();
    retroarch.push_back("true");
    retroarch.push_back("false");
    adv.clear();
    adv.push_back("true");
    adv.push_back("false");
    quickboot.clear();
    quickboot.push_back("true");
    quickboot.push_back("false");

}

#define CFG_THEME      0
#define CFG_MENUTH     1
#define CFG_PCSX       2
#define CFG_QUICK      3
#define CFG_BGM        4
#define CFG_MIP        5
#define CFG_RA         6
#define CFG_ADV        7

string GuiOptions::getBooleanIcon(string input)
{
    shared_ptr<Gui> gui(Gui::getInstance());
    string value = gui->cfg.inifile.values[input];
    if (value=="true") return "|@Check|"; else return "|@Uncheck|";
}

void GuiOptions::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=Configuration=-",0,offset,true);
    gui->renderTextLine("AutoBleem Theme: " + gui->cfg.inifile.values["theme"], CFG_THEME+1, offset);
    gui->renderTextLine("Menu Theme: " + gui->cfg.inifile.values["stheme"], CFG_MENUTH+1, offset);
    gui->renderTextLine("PCSX version: " + gui->cfg.inifile.values["pcsx"], CFG_PCSX+1, offset);
    gui->renderTextLine("Disable Theme BGM: " + getBooleanIcon("nomusic"), CFG_BGM+1, offset);
    gui->renderTextLine("QuickBoot: " + getBooleanIcon("quick"), CFG_QUICK+1, offset);
    gui->cfg.inifile.values["autoregion"]="true"; // removing this as an option - not needed - just set to true

    gui->renderTextLine("GFX Filter patch: " + getBooleanIcon("mip"), CFG_MIP+1, offset);

    gui->renderTextLine("Show RetroArch: " + getBooleanIcon("retroarch"), CFG_RA+1, offset);
    gui->renderTextLine("Advanced: " + getBooleanIcon("adv"), CFG_ADV+1, offset);

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
                            if (selOption > 7) {
                                selOption = 7;
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
                            if (selOption == CFG_THEME) {
                                string nextValue = getOption(themes, gui->cfg.inifile.values["theme"], true);
                                gui->cfg.inifile.values["theme"] = nextValue;
                                init();
                                gui->loadAssets();
                            }

                            if (selOption == CFG_MENUTH) {
                                string nextValue = getOption(sthemes, gui->cfg.inifile.values["stheme"], true);
                                gui->cfg.inifile.values["stheme"] = nextValue;

                            }
                            if (selOption == CFG_BGM) {
                                string nextValue = getOption(nomusic, gui->cfg.inifile.values["nomusic"], true);
                                gui->cfg.inifile.values["nomusic"] = nextValue;
                                init();
                                gui->loadAssets();

                            }

                            if (selOption == CFG_MIP) {
                                string nextValue = getOption(mip, gui->cfg.inifile.values["mip"], true);
                                gui->cfg.inifile.values["mip"] = nextValue;


                            }
                            if (selOption == CFG_PCSX) {
                                string nextValue = getOption(pcsx, gui->cfg.inifile.values["pcsx"], true);
                                gui->cfg.inifile.values["pcsx"] = nextValue;


                            }
                            if (selOption == CFG_RA) {
                                string nextValue = getOption(retroarch, gui->cfg.inifile.values["retroarch"], true);
                                gui->cfg.inifile.values["retroarch"] = nextValue;

                            }
                            if (selOption == CFG_ADV) {
                                string nextValue = getOption(adv, gui->cfg.inifile.values["adv"], true);
                                gui->cfg.inifile.values["adv"] = nextValue;

                            }
                            if (selOption == CFG_QUICK) {
                                string nextValue = getOption(quickboot, gui->cfg.inifile.values["quick"], true);
                                gui->cfg.inifile.values["quick"] = nextValue;
                            }


                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            if (selOption == CFG_THEME) {
                                string nextValue = getOption(themes, gui->cfg.inifile.values["theme"], false);
                                gui->cfg.inifile.values["theme"] = nextValue;
                                init();
                                gui->loadAssets();
                            }
                            if (selOption == CFG_MENUTH) {
                                string nextValue = getOption(sthemes, gui->cfg.inifile.values["stheme"], false);
                                gui->cfg.inifile.values["stheme"] = nextValue;

                            }
                            if (selOption == CFG_BGM) {
                                string nextValue = getOption(nomusic, gui->cfg.inifile.values["nomusic"], false);
                                gui->cfg.inifile.values["nomusic"] = nextValue;
                                init();
                                gui->loadAssets();

                            }

                            if (selOption == CFG_MIP) {
                                string nextValue = getOption(mip, gui->cfg.inifile.values["mip"], false);
                                gui->cfg.inifile.values["mip"] = nextValue;

                            }
                            if (selOption == CFG_PCSX) {
                                string nextValue = getOption(pcsx, gui->cfg.inifile.values["pcsx"], false);
                                gui->cfg.inifile.values["pcsx"] = nextValue;

                            }
                            if (selOption == CFG_RA) {
                                string nextValue = getOption(retroarch, gui->cfg.inifile.values["retroarch"], false);
                                gui->cfg.inifile.values["retroarch"] = nextValue;

                            }
                            if (selOption == CFG_ADV) {
                                string nextValue = getOption(adv, gui->cfg.inifile.values["adv"], false);
                                gui->cfg.inifile.values["adv"] = nextValue;

                            }
                            if (selOption == CFG_QUICK) {
                                string nextValue = getOption(quickboot, gui->cfg.inifile.values["quick"], false);
                                gui->cfg.inifile.values["quick"] = nextValue;
                            }

                            render();
                        }
                    }


                    break;


            }

        }
    }
}