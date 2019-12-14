//
// Created by screemer on 2019-01-24.
//

#include "gui_options.h"
#include "../util.h"
#include "../environment.h"

using namespace std;

enum {
    CFG_THEME=0,
    CFG_PS1GAMES_SELECT,
    CFG_SHOW_ORIGAMES,
    CFG_UI,
    CFG_JEWEL,
    CFG_MUSIC,
    CFG_ENABLE_BACKGROUND_MUSIC,
    CFG_WIDESCREEN,
    CFG_QUICK_BOOT,
    CFG_QUICKMENU,
    CFG_GFX_FILTER,
    CFG_RACONFIG,
    CFG_SHOWINGTIMEOUT,
    CFG_LANG
};
#define CFG_LAST CFG_LANG

// CFG_PS1GAMES_SELECT select states
// in config.ini: "ps1select" = "All Games", "Internal Only", "Games Subdir", "Favorites"
vector<string> ps1SelectStrings { "All Games", "Internal Only", "Games Subdir", "Favorites" };

//*******************************
// GuiOptions::getPrevNextOption
//*******************************
string GuiOptions::getPrevNextOption(const vector<string> & list, const string & current, bool next) {
    // find current position
    int pos = 0;
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == current) {
            pos = i;
            break;
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

//*******************************
// GuiOptions::doPrevNextOption
//*******************************
void GuiOptions::doPrevNextOption(shared_ptr<Gui> gui, shared_ptr<Lang> lang, bool next) {
    if (selOption == CFG_THEME) {
        string nextValue = getPrevNextOption(autobleemUIThemes, gui->cfg.inifile.values["theme"], next);
        gui->cfg.inifile.values["theme"] = nextValue;
        init();
        gui->loadAssets();
    }

    if (selOption == CFG_PS1GAMES_SELECT) {
        string nextValue = getPrevNextOption(ps1SelectState, gui->cfg.inifile.values["ps1select"], next);
        gui->cfg.inifile.values["ps1select"] = nextValue;
        init();
        gui->loadAssets();
    }

    if (selOption == CFG_SHOW_ORIGAMES) {
        string nextValue = getPrevNextOption(origames, gui->cfg.inifile.values["origames"], next);
        gui->cfg.inifile.values["origames"] = nextValue;
    }

    if (selOption == CFG_UI) {
        string nextValue = getPrevNextOption(ui, gui->cfg.inifile.values["ui"], next);
        gui->cfg.inifile.values["ui"] = nextValue;
    }

    if (selOption == CFG_JEWEL) {
        string nextValue = getPrevNextOption(jewels, gui->cfg.inifile.values["jewel"], next);
        gui->cfg.inifile.values["jewel"] = nextValue;
    }

    if (selOption == CFG_MUSIC) {
        string nextValue = getPrevNextOption(music, gui->cfg.inifile.values["music"], next);
        gui->cfg.inifile.values["music"] = nextValue;
        init();
        gui->loadAssets();
    }

    if (selOption == CFG_ENABLE_BACKGROUND_MUSIC) {
        string nextValue = getPrevNextOption(nomusic, gui->cfg.inifile.values["nomusic"], next);
        gui->cfg.inifile.values["nomusic"] = nextValue;
        init();
        gui->loadAssets();
    }

    if (selOption == CFG_WIDESCREEN) {
        string nextValue = getPrevNextOption(aspect, gui->cfg.inifile.values["aspect"], next);
        gui->cfg.inifile.values["aspect"] = nextValue;
    }

    if (selOption == CFG_QUICK_BOOT) {
        string nextValue = getPrevNextOption(quickboot, gui->cfg.inifile.values["quick"], next);
        if (next) {
            string last = gui->cfg.inifile.values["quick"];
            gui->cfg.inifile.values["quick"] = nextValue;
            int delay = atoi(gui->cfg.inifile.values["delay"].c_str());
            delay++;
            if (last == "false") delay = 1;
            gui->cfg.inifile.values["delay"] = to_string(delay);
        } else {
            int delay = atoi(gui->cfg.inifile.values["delay"].c_str());
            delay++;
            gui->cfg.inifile.values["delay"] = to_string(1);
            gui->cfg.inifile.values["quick"] = nextValue;
        }
    }

    if (selOption == CFG_QUICKMENU) {
        string nextValue = getPrevNextOption(quickmenu, gui->cfg.inifile.values["quickmenu"], next);
        gui->cfg.inifile.values["quickmenu"] = nextValue;
    }

    if (selOption == CFG_GFX_FILTER) {
        string nextValue = getPrevNextOption(mip, gui->cfg.inifile.values["mip"], next);
        gui->cfg.inifile.values["mip"] = nextValue;
    }

    if (selOption == CFG_RACONFIG) {
        string nextValue = getPrevNextOption(raconfig, gui->cfg.inifile.values["raconfig"], next);
        gui->cfg.inifile.values["raconfig"] = nextValue;
    }

    if (selOption == CFG_SHOWINGTIMEOUT) {
//        if (next) {
            string nextValue = getPrevNextOption(showingtimeout, gui->cfg.inifile.values["showingtimeout"], next);
            gui->cfg.inifile.values["showingtimeout"] = nextValue;
//        } else {
//              // it looks like it was preventing wrap around from 0 to 20 but wrap around isn't in getPrevNextOption
//            string curValue = gui->cfg.inifile.values["showingtimeout"];
//            string nextValue = getPrevNextOption(showingtimeout, curValue, false);
//            if (curValue != "0")
//                gui->cfg.inifile.values["showingtimeout"] = nextValue;
//        }
    }

    if (selOption == CFG_LANG) {
        string nextValue = getPrevNextOption(languages, gui->cfg.inifile.values["language"], next);
        gui->cfg.inifile.values["language"] = nextValue;
        lang->load(nextValue);
        init();
    }
}

//*******************************
// GuiOptions::init
//*******************************
void GuiOptions::init() {
    shared_ptr<Lang> lang(Lang::getInstance());

    autobleemUIThemes.clear();

    string uiThemePath = Env::getPathToThemesDir();
    DirEntries uiThemeFolders = DirEntry::diru_DirsOnly(uiThemePath);
    for (const DirEntry & entry : uiThemeFolders) {
        if (DirEntry::exists(uiThemePath + sep + entry.name + sep + "theme.ini")) {
            autobleemUIThemes.push_back(entry.name);
        }
    }

    ps1SelectState = ps1SelectStrings;

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
    autoregion.push_back("false");
    autoregion.push_back("true");
    adv.clear();
    adv.push_back("false");
    adv.push_back("true");
    quickboot.clear();
    quickboot.push_back("false");
    quickboot.push_back("true");
    languages.clear();
    languages = lang->getListOfLanguages();
    ui.clear();
    ui.push_back("classic");
    ui.push_back("EvolutionUI");
    aspect.clear();
    aspect.push_back("false");
    aspect.push_back("true");
    origames.clear();
    origames.push_back("false");
    origames.push_back("true");
    jewels.clear();
    jewels.push_back("none");
    jewels.push_back("default");

    DirEntries folders = DirEntry::diru_FilesOnly(Env::getWorkingPath() + sep + "evoimg/frames");
    for (const DirEntry & entry : folders) {
        if (DirEntry::getFileExtension(entry.name) == "png") {
            jewels.push_back(entry.name);
        }
    }
    quickmenu.clear();
    quickmenu.push_back("UI");
    quickmenu.push_back("RetroArch");
    music.clear();
    music.push_back("--");
    folders = DirEntry::diru_FilesOnly(Env::getWorkingPath() + sep + "music");
    for (const DirEntry & entry:folders) {
        if (DirEntry::getFileExtension(entry.name) == "ogg") {
            music.push_back(entry.name);
        }
    }
    for (int i=0; i <= 20; ++i) {
        showingtimeout.push_back(to_string(i));
    }

    raconfig.clear();
    raconfig.push_back("false");
    raconfig.push_back("true");
}

//*******************************
// GuiOptions::getBooleanIcon
//*******************************
string GuiOptions::getBooleanIcon(const string & input) {
    shared_ptr<Gui> gui(Gui::getInstance());
    string value = gui->cfg.inifile.values[input];
    if (input == "quick") {
        if (value == "true") return gui->cfg.inifile.values["delay"] + "s  " + "|@Check|"; else return "|@Uncheck|";
    }

    if ((input != "nomusic") && (input != "mip")) {
        if (value == "true") return "|@Check|"; else return "|@Uncheck|";
    } else {
        if (value != "true") return "|@Check|"; else return "|@Uncheck|";
    }
}

//*******************************
// GuiOptions::renderOptionLine
//*******************************
void GuiOptions::renderOptionLine(const string & text, int pos, int offset) {
    shared_ptr<Gui> gui(Gui::getInstance());
    string fg = gui->themeData.values["text_fg"];
    int height = gui->renderTextLineOptions(text, pos, offset, POS_LEFT);
    totalHeight += height;

    if (selOption + 1 == pos) {
        SDL_Rect rect2;
        rect2.x = atoi(gui->themeData.values["opscreenx"].c_str());
        rect2.y = atoi(gui->themeData.values["opscreeny"].c_str());
        rect2.w = atoi(gui->themeData.values["opscreenw"].c_str());
        rect2.h = atoi(gui->themeData.values["opscreenh"].c_str());

        SDL_Rect rectSelection;
        rectSelection.x = rect2.x + 5;
        rectSelection.y = offset + totalHeight;
        rectSelection.w = rect2.w - 10;
        rectSelection.h = height;

        SDL_SetRenderDrawColor(renderer, gui->getR(fg), gui->getG(fg), gui->getB(fg), 255);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderDrawRect(renderer, &rectSelection);
    }
}

//*******************************
// GuiOptions::render
//*******************************
void GuiOptions::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    totalHeight = 0;
    gui->renderTextLine("-=" + _("Configuration") + "=-", 0, offset, POS_CENTER);

    renderOptionLine(_("AutoBleem Theme:") + " " + gui->cfg.inifile.values["theme"], CFG_THEME + 1, offset);
    renderOptionLine(_("PS1 Games Selection:") + " " + gui->cfg.inifile.values["ps1select"], CFG_PS1GAMES_SELECT + 1, offset);
    renderOptionLine(_("Show Internal Games:") + " " + getBooleanIcon("origames"), CFG_SHOW_ORIGAMES + 1, offset);
    renderOptionLine(_("UI:") + " " + gui->cfg.inifile.values["ui"], CFG_UI + 1, offset);
    renderOptionLine(_("Cover Style:") + " " + gui->cfg.inifile.values["jewel"], CFG_JEWEL + 1, offset);
    renderOptionLine(_("Music:") + " " + gui->cfg.inifile.values["music"], CFG_MUSIC + 1, offset);
    renderOptionLine(_("Background Music:") + " " + getBooleanIcon("nomusic"), CFG_ENABLE_BACKGROUND_MUSIC + 1, offset);
    renderOptionLine(_("Widescreen:") + " " + getBooleanIcon("aspect"), CFG_WIDESCREEN + 1, offset);
    renderOptionLine(_("QuickBoot:") + " " + getBooleanIcon("quick"), CFG_QUICK_BOOT + 1, offset);
    renderOptionLine(_("QuickBoot Init:") + " " + gui->cfg.inifile.values["quickmenu"], CFG_QUICKMENU + 1, offset);
    renderOptionLine(_("GFX Filter:") + " " + getBooleanIcon("mip"), CFG_GFX_FILTER + 1, offset);
    renderOptionLine(_("Update RA Config:") + " " + getBooleanIcon("raconfig"), CFG_RACONFIG + 1, offset);
    renderOptionLine(_("Showing Timeout (0 = no timeout):") + " " + gui->cfg.inifile.values["showingtimeout"], CFG_SHOWINGTIMEOUT + 1, offset);
    renderOptionLine(_("Language:") + " " + gui->cfg.inifile.values["language"], CFG_LANG + 1, offset);

    gui->cfg.inifile.values["autoregion"] = "true"; // removing this as an option - not needed - just set to true

    gui->renderStatus("|@X| " + _("OK") + "     " + "|@O| " + _("Cancel") + "|");

    //   gui->renderSelectionBox(selOption+1,offset);
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiOptions::loop
//*******************************
void GuiOptions::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());
    shared_ptr<Lang> lang(Lang::getInstance());

    render();

    bool waitForButtonToBeReleased = true;
    bool menuVisible = true;
    while (menuVisible) {
        gui->watchJoystickPort();
        SDL_Event e;

        if (waitForButtonToBeReleased) {
            if (SDL_PollEvent(&e) && e.type == SDL_KEYDOWN)
                continue;   // wait until the button that was pressed is released
            else
                waitForButtonToBeReleased = false;
        }

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
                case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        string cfg_path = Env::getWorkingPath() + sep + "config.ini";
                        gui->cfg.inifile.load(cfg_path);    // restore the original config.ini settings
                        lang->load(gui->cfg.inifile.values["language"]);    // restore the original lang
                        gui->loadAssets();                                  // restore original themes
                        gui->overrideQuickBoot = true;
                        menuVisible = false;
                        exitCode = -1;
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        gui->cfg.save();
                        gui->overrideQuickBoot = true;
                        menuVisible = false;
                        exitCode = 0;
                    };
                    break;

                case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                case SDL_JOYHATMOTION:

                    int device;
                    if (e.type==SDL_JOYHATMOTION)
                    {
                        device = e.jhat.which;
                    } else
                    {
                        device = e.jaxis.which;
                    }
                    if (gui->mapper.isUp(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selOption--;
                        if (selOption < 0) {
                            selOption = CFG_LAST;
                        }
                        render();
                    }

                    if (gui->mapper.isDown(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selOption++;
                        if (selOption > CFG_LAST) {
                            selOption = 0;
                        }
                        render();
                    }

                    if (gui->mapper.isRight(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        doPrevNextOption(gui, lang, true);
                        render();
                    }

                    if (gui->mapper.isLeft(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        doPrevNextOption(gui, lang, false);
                        render();
                    }
                    break;
            }
        }
    }
}