//
// Created by screemer on 2019-01-24.
//

#include "gui_optionsMenu.h"
#include "../../util.h"
#include "../../environment.h"

using namespace std;

//*******************************
// GuiOptions::getThemes
//*******************************
vector<string> GuiOptions::getThemes() {
    vector<string> list;
    string uiThemePath = Env::getPathToThemesDir();
    DirEntries uiThemeFolders = DirEntry::diru_DirsOnly(uiThemePath);
    for (const DirEntry &entry : uiThemeFolders) {
        if (DirEntry::exists(uiThemePath + sep + entry.name + sep + "theme.ini")) {
            list.push_back(entry.name); // add the theme dir name
        }
    }

    return list;
}

//*******************************
// GuiOptions::getJewels
//*******************************
vector<string> GuiOptions::getJewels() {
    vector<string> list;
    DirEntries folders = DirEntry::diru_FilesOnly(Env::getWorkingPath() + sep + "evoimg/frames");
    for (const DirEntry & entry : folders) {
        if (DirEntry::getFileExtension(entry.name) == "png") {
            list.push_back(entry.name);
        }
    }

    return list;
}

//*******************************
// GuiOptions::getMusic
//*******************************
vector<string> GuiOptions::getMusic() {
    vector<string> list;
    DirEntries folders = DirEntry::diru_FilesOnly(Env::getWorkingPath() + sep + "music");
    for (const DirEntry & entry:folders) {
        if (DirEntry::getFileExtension(entry.name) == "ogg") {
            list.push_back(entry.name);
        }
    }

    return list;
}

//*******************************
// GuiOptions::getTimeoutValues
//*******************************
vector<string> GuiOptions::getTimeoutValues() {
    vector<string> list;
    for (int i=0; i <= 20; ++i) {
        list.push_back(to_string(i));
    }

    return list;
}

//*******************************
// GuiOptions::fill
//*******************************
void GuiOptions::fill() {
    lines.emplace_back(CFG_THEME, "AutoBleem Theme:", "theme", false, getThemes());
    lines.emplace_back(CFG_SHOW_ORIGAMES, "Show Internal Games:", "origames", true, vector<string> ({ "false", "true" }) );
    lines.emplace_back(CFG_UI, "UI:", "ui", false, vector<string> ({ "classic", "EvolutionUI" }) );
    lines.emplace_back(CFG_JEWEL, "Cover Style:", "jewel", false, getJewels());
    lines.emplace_back(CFG_MUSIC, "Music:", "music", false, getMusic());
    lines.emplace_back(CFG_ENABLE_BACKGROUND_MUSIC, "Background Music:", "nomusic", true, vector<string> ({ "true", "false" }) );
    lines.emplace_back(CFG_WIDESCREEN, "Widescreen:", "aspect", true, vector<string> ({ "false", "true" }) );
    lines.emplace_back(CFG_QUICK_BOOT, "QuickBoot:", "quick", true, vector<string> ({ "false", "true" }) );
    lines.emplace_back(CFG_QUICKMENU, "QuickBoot Init:", "quickmenu", false, vector<string> ({ "UI", "RetroArch" }) );
    lines.emplace_back(CFG_GFX_FILTER, "GFX Filter:", "mip", true, vector<string> ({ "true", "false" }) );
    lines.emplace_back(CFG_RACONFIG, "Update RA Config:", "raconfig", true, vector<string> ({ "false", "true" }) );
    lines.emplace_back(CFG_SHOWINGTIMEOUT, "Showing Timeout (0 = no timeout):", "showingtimeout", false, getTimeoutValues());
    lines.emplace_back(CFG_LANG, "Language:", "language", false, lang->getListOfLanguages());
}

//*******************************
// GuiOptions::init
//*******************************
void GuiOptions::init() {
    GuiOptionsMenuBase::init(); // call the base class init()
    lines.clear();
    fill();

#if 0
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
#endif

    gui->cfg.inifile.values["autoregion"] = "true"; // removing this as an option - not needed - just set to true
}

#if 0
//*******************************
// GuiOptions::renderOptionLine
//*******************************
void GuiOptions::renderOptionLine(const string & text, int cfgIndex, int offset) {
    int row = firstRow + cfgIndex;
    int height = gui->renderTextLineOptions(text, row, offset, POS_LEFT);
    int totalHeight = row * height;

#if 0   // use gui->renderSelectionBox instead
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

    string fg = gui->themeData.values["text_fg"];
        SDL_SetRenderDrawColor(renderer, gui->getR(fg), gui->getG(fg), gui->getB(fg), 255);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderDrawRect(renderer, &rectSelection);
    }
#endif
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
#endif

//*******************************
// void GuiOptions::getLineText
//*******************************
std::string GuiOptions::getLineText(const OptionsInfo& info) {
    std::string temp = lang->translate(info.descriptionToTranslate) + " ";
    auto value = gui->cfg.inifile.values[info.iniKey];
    if (info.keyIsBoolean) {
        if (value == "true" && info.id == CFG_QUICK_BOOT) // quick boot as both a delay value and a boolean switch
            temp += gui->cfg.inifile.values["delay"] + "s  ";
        temp += getBooleanSymbolText(info, value);
    } else {
        temp += value;  // append the current text value in the options list
    }

    return temp;
}

#if 0
//*******************************
// GuiOptions::render
//*******************************
void GuiOptions::render() {
    SDL_RenderClear(renderer);
    gui->renderBackground();
    gui->renderTextBar();
    offset = gui->renderLogo(true);
    gui->renderTextLine(getTitle(), 0, offset, POS_CENTER);

#if 0
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
#endif

    gui->renderSelectionBox(firstRow + selected, offset);

    gui->renderStatus(getStatusLine());
    SDL_RenderPresent(renderer);
}
#endif

#if 0
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
#endif

//*******************************
// GuiOptions::doPrevNextOption
//*******************************
string GuiOptions::doPrevNextOption(OptionsInfo& info, bool next) {
    int id = info.id;

    // CFG_QUICK_BOOT is a specal case
    if (id == CFG_QUICK_BOOT) {
        string nextValue = getPrevNextOption(info, gui->cfg.inifile.values[info.iniKey], next);
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

        return nextValue;
    }

    // do the default action
    string nextValue = GuiOptionsMenuBase::doPrevNextOption(info, next);

    // after doing the default these need special action afterwards
    if (id == CFG_THEME || id == CFG_MUSIC || id == CFG_ENABLE_BACKGROUND_MUSIC) {
        gui->loadAssets();

        if (id == CFG_THEME)
            font = gui->themeFont;
        else if (id == CFG_LANG)
            lang->load(nextValue);
    }
    return nextValue;
}

//*******************************
// void GuiOptions::doOptionIndex()
//*******************************
string GuiOptions::doOptionIndex(uint index) {
    if (validSelectedIndex()) {
        int id = lines[selected].id;
        if (id == CFG_QUICK_BOOT)
            return "";  // CFG_QUICK_BOOT is a special case.  don't do it
        else {
            string nextValue = GuiOptionsMenuBase::doOptionIndex(index);
            if (id == CFG_THEME || id == CFG_MUSIC || id == CFG_ENABLE_BACKGROUND_MUSIC) {
                gui->loadAssets();

                if (id == CFG_THEME)
                    font = gui->themeFont;
                else if (id == CFG_LANG)
                    lang->load(nextValue);
            }
            return nextValue;
        }
    } else
        return "";
}

//*******************************
// GuiOptions::doCircle_Pressed
//*******************************
void GuiOptions::doCircle_Pressed() {
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
// GuiOptions::doCross_Pressed
//*******************************
void GuiOptions::doCross_Pressed() {
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
    } while (fastForwardUntilAnotherEvent());
}

//*******************************
// GuiOptions::doJoyLeft
//*******************************
void GuiOptions::doJoyLeft() {
    do {
        doKeyLeft();
        render();
    } while (fastForwardUntilAnotherEvent());
}

//*******************************
// GuiOptions::doKeyRight
//*******************************
void GuiOptions::doKeyRight() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    doPrevNextOption(true);
}

//*******************************
// GuiOptions::doKeyLeft
//*******************************
void GuiOptions::doKeyLeft() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    doPrevNextOption(false);
}
