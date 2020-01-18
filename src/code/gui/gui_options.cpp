//
// Created by screemer on 2019-01-24.
//

#include "gui_options.h"
#include "../util.h"
#include "../environment.h"

using namespace std;

//*******************************
// GuiOptions::init
//*******************************
void GuiOptions::init() {
    GuiOptionsMenuBase::init(); // call the base class init()

    lang = Lang::getInstance();

    autobleemUIThemes.clear();

    string uiThemePath = Env::getPathToThemesDir();
    DirEntries uiThemeFolders = DirEntry::diru_DirsOnly(uiThemePath);
    for (const DirEntry & entry : uiThemeFolders) {
        if (DirEntry::exists(uiThemePath + sep + entry.name + sep + "theme.ini")) {
            autobleemUIThemes.push_back(entry.name);
        }
    }

    mip.clear();
    mip.push_back("true");
    mip.push_back("false");

    nomusic.clear();
    nomusic.push_back("true");
    nomusic.push_back("false");

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
// GuiOptions::renderOptionLine
//*******************************
void GuiOptions::renderOptionLine(const string & text, int cfgIndex, int offset) {
    string fg = gui->themeData.values["text_fg"];
    int row = firstRow + cfgIndex;
    int height = gui->renderTextLineOptions(text, row, offset, POS_LEFT);
    int totalHeight = row * height;

#if 0
    if (selected == cfgIndex) {
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
#endif
}

//*******************************
// GuiOptions::render
//*******************************
void GuiOptions::render() {
    SDL_RenderClear(renderer);
    gui->renderBackground();
    gui->renderTextBar();
    offset = gui->renderLogo(true);
    gui->renderTextLine(title, 0, offset, POS_CENTER);

    renderOptionLine(_("AutoBleem Theme:") + " " + gui->cfg.inifile.values["theme"], CFG_THEME, offset);
    renderOptionLine(_("Show Internal Games:") + " " + getBooleanIcon("origames"), CFG_SHOW_ORIGAMES, offset);
    renderOptionLine(_("UI:") + " " + gui->cfg.inifile.values["ui"], CFG_UI, offset);
    renderOptionLine(_("Cover Style:") + " " + gui->cfg.inifile.values["jewel"], CFG_JEWEL, offset);
    renderOptionLine(_("Music:") + " " + gui->cfg.inifile.values["music"], CFG_MUSIC, offset);
    renderOptionLine(_("Background Music:") + " " + getBooleanIcon("nomusic"), CFG_ENABLE_BACKGROUND_MUSIC, offset);
    renderOptionLine(_("Widescreen:") + " " + getBooleanIcon("aspect"), CFG_WIDESCREEN, offset);
    renderOptionLine(_("QuickBoot:") + " " + getBooleanIcon("quick"), CFG_QUICK_BOOT, offset);
    renderOptionLine(_("QuickBoot Init:") + " " + gui->cfg.inifile.values["quickmenu"], CFG_QUICKMENU, offset);
    renderOptionLine(_("GFX Filter:") + " " + getBooleanIcon("mip"), CFG_GFX_FILTER, offset);
    renderOptionLine(_("Update RA Config:") + " " + getBooleanIcon("raconfig"), CFG_RACONFIG, offset);
    renderOptionLine(_("Showing Timeout (0 = no timeout):") + " " + gui->cfg.inifile.values["showingtimeout"], CFG_SHOWINGTIMEOUT, offset);
    renderOptionLine(_("Language:") + " " + gui->cfg.inifile.values["language"], CFG_LANG, offset);

    gui->cfg.inifile.values["autoregion"] = "true"; // removing this as an option - not needed - just set to true

    gui->renderSelectionBox(firstRow + selected, offset);

    gui->renderStatus(statusLine());
    SDL_RenderPresent(renderer);
}

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
    if (selected == CFG_THEME) {
        string nextValue = getPrevNextOption(autobleemUIThemes, gui->cfg.inifile.values["theme"], next);
        gui->cfg.inifile.values["theme"] = nextValue;
        init();
        gui->loadAssets();
    }

    if (selected == CFG_SHOW_ORIGAMES) {
        string nextValue = getPrevNextOption(origames, gui->cfg.inifile.values["origames"], next);
        gui->cfg.inifile.values["origames"] = nextValue;
    }

    if (selected == CFG_UI) {
        string nextValue = getPrevNextOption(ui, gui->cfg.inifile.values["ui"], next);
        gui->cfg.inifile.values["ui"] = nextValue;
    }

    if (selected == CFG_JEWEL) {
        string nextValue = getPrevNextOption(jewels, gui->cfg.inifile.values["jewel"], next);
        gui->cfg.inifile.values["jewel"] = nextValue;
    }

    if (selected == CFG_MUSIC) {
        string nextValue = getPrevNextOption(music, gui->cfg.inifile.values["music"], next);
        gui->cfg.inifile.values["music"] = nextValue;
        init();
        gui->loadAssets();
    }

    if (selected == CFG_ENABLE_BACKGROUND_MUSIC) {
        string nextValue = getPrevNextOption(nomusic, gui->cfg.inifile.values["nomusic"], next);
        gui->cfg.inifile.values["nomusic"] = nextValue;
        init();
        gui->loadAssets();
    }

    if (selected == CFG_WIDESCREEN) {
        string nextValue = getPrevNextOption(aspect, gui->cfg.inifile.values["aspect"], next);
        gui->cfg.inifile.values["aspect"] = nextValue;
    }

    if (selected == CFG_QUICK_BOOT) {
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

    if (selected == CFG_QUICKMENU) {
        string nextValue = getPrevNextOption(quickmenu, gui->cfg.inifile.values["quickmenu"], next);
        gui->cfg.inifile.values["quickmenu"] = nextValue;
    }

    if (selected == CFG_GFX_FILTER) {
        string nextValue = getPrevNextOption(mip, gui->cfg.inifile.values["mip"], next);
        gui->cfg.inifile.values["mip"] = nextValue;
    }

    if (selected == CFG_RACONFIG) {
        string nextValue = getPrevNextOption(raconfig, gui->cfg.inifile.values["raconfig"], next);
        gui->cfg.inifile.values["raconfig"] = nextValue;
    }

    if (selected == CFG_SHOWINGTIMEOUT) {
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

    if (selected == CFG_LANG) {
        string nextValue = getPrevNextOption(languages, gui->cfg.inifile.values["language"], next);
        gui->cfg.inifile.values["language"] = nextValue;
        lang->load(nextValue);
        init();
    }
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
// GuiOptions::doCirclePressed
//*******************************
void GuiOptions::doCircle() {
    Mix_PlayChannel(-1, gui->cancel, 0);
    string cfg_path = Env::getWorkingPath() + sep + "config.ini";
    gui->cfg.inifile.load(cfg_path);    // restore the original config.ini settings
    lang->load(gui->cfg.inifile.values["language"]);    // restore the original lang
    gui->loadAssets();                                  // restore original themes
    gui->overrideQuickBoot = true;
    menuVisible = false;
    exitCode = -1;
}

//*******************************
// GuiOptions::doCrossPressed
//*******************************
void GuiOptions::doCross() {
    Mix_PlayChannel(-1, gui->cancel, 0);
    gui->cfg.save();
    gui->overrideQuickBoot = true;
    menuVisible = false;
    exitCode = 0;
}

//*******************************
// GuiOptions::doJoyRight
//*******************************
void GuiOptions::doJoyRight() {
    do {
        doKeyRight();
        render();
    } while (fastForwardUntilAnotherEvent(100));
}

//*******************************
// GuiOptions::doJoyLeft
//*******************************
void GuiOptions::doJoyLeft() {
    do {
        doKeyLeft();
        render();
    } while (fastForwardUntilAnotherEvent(100));
}

//*******************************
// GuiOptions::doKeyRight
//*******************************
void GuiOptions::doKeyRight() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    doPrevNextOption(gui, lang, true);
}

//*******************************
// GuiOptions::doKeyLeft
//*******************************
void GuiOptions::doKeyLeft() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    doPrevNextOption(gui, lang, false);
}
