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
    list.push_back("--");
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
    // this is filled once and not on every render.
    // save the current lang and switch to English.  we need the "Prefix:" to be scanned in English for English.txt
    // getLineText() will do the translation
    string saveCurrentLang = lang->currentLang;
    lang->load("English");

    lines.emplace_back(CFG_THEME, _("AutoBleem Theme:"), "theme", false, getThemes());
    lines.emplace_back(CFG_SHOW_ORIGAMES, _("Show Internal Games:"), "origames", true, vector<string> ({ "false", "true" }) );
    lines.emplace_back(CFG_UI, _("UI:"), "ui", false, vector<string> ({ "classic", "EvolutionUI" }) );
    lines.emplace_back(CFG_JEWEL, _("Cover Style:"), "jewel", false, getJewels());
    lines.emplace_back(CFG_MUSIC, _("Music:"), "music", false, getMusic());
    lines.emplace_back(CFG_ENABLE_BACKGROUND_MUSIC, _("Background Music:"), "nomusic", true, vector<string> ({ "true", "false" }) );
    lines.emplace_back(CFG_WIDESCREEN, _("Widescreen:"), "aspect", true, vector<string> ({ "false", "true" }) );
    lines.emplace_back(CFG_QUICK_BOOT, _("QuickBoot:"), "quick", true, vector<string> ({ "false", "true" }) );
    lines.emplace_back(CFG_QUICKMENU, _("QuickBoot Init:"), "quickmenu", false, vector<string> ({ "UI", "RetroArch" }) );
    lines.emplace_back(CFG_GFX_FILTER, _("GFX Filter:"), "mip", true, vector<string> ({ "true", "false" }) );
    lines.emplace_back(CFG_RACONFIG, _("Update RA Config:"), "raconfig", true, vector<string> ({ "false", "true" }) );
    lines.emplace_back(CFG_SHOWINGTIMEOUT, _("Showing Timeout (0 = no timeout):"), "showingtimeout", false, getTimeoutValues());
    lines.emplace_back(CFG_LANG, _("Language:"), "language", false, lang->getListOfLanguages());

    lang->load(saveCurrentLang);
}

//*******************************
// GuiOptions::init
//*******************************
void GuiOptions::init() {
    GuiOptionsMenuBase::init(); // call the base class init()
    lines.clear();
    fill();

    gui->cfg.inifile.values["autoregion"] = "true"; // removing this as an option - not needed - just set to true
}

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
    if (id == CFG_THEME) {
        gui->loadAssets();
        font = gui->themeFont;  // get the new font for the menu
    } else if (id == CFG_LANG) {
        lang->load(nextValue);
    } else if (id == CFG_MUSIC || id == CFG_ENABLE_BACKGROUND_MUSIC) {
        gui->loadAssets();
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
            // do the default action
            string nextValue = GuiOptionsMenuBase::doOptionIndex(index);

            // after doing the default these need special action afterwards
            if (id == CFG_THEME) {
                gui->loadAssets();
                font = gui->themeFont;  // get the new font for the menu
            } else if (id == CFG_LANG) {
                lang->load(nextValue);
            } else if (id == CFG_MUSIC || id == CFG_ENABLE_BACKGROUND_MUSIC) {
                gui->loadAssets();
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
