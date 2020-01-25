//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_optionsMenuBase.h"
#include "../gui.h"
#include "../../lang.h"
#include <string>
#include <vector>

enum {
    CFG_THEME=0,
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
#define CFG_SIZE (CFG_LAST+1)

//********************
// GuiOptions
//********************
class GuiOptions : public GuiOptionsMenuBase {
public:
    GuiOptions(SDL_Shared<SDL_Renderer> _renderer)
        : GuiOptionsMenuBase(_renderer) {}

    void init() override;
    //void render() override;
    //void loop() override;

    std::vector<std::string> getThemes();
    std::vector<std::string> getJewels();
    std::vector<std::string> getMusic();
    std::vector<std::string> getTimeoutValues();

    void fill();

    virtual std::string getTitle() override { return "-=" + _("Configuration") + "=-"; }
    virtual std::string getStatusLine() override { return "|@X| " + _("OK") + "     " + "|@O| " + _("Cancel") + "|"; }

    virtual std::string getLineText(const OptionsInfo& info);
    virtual std::string doPrevNextOption(OptionsInfo& info, bool next);
    virtual std::string doPrevNextOption(bool next) { return GuiOptionsMenuBase::doPrevNextOption(next); }

    virtual std::string doOptionIndex(uint index);

    int exitCode=0;

    virtual void doCircle_Pressed();
    virtual void doCross_Pressed();

    virtual void doJoyRight();  // move option to the right, may fast forwward
    virtual void doJoyLeft();   // move option to the left, may fast forwward

    virtual void doKeyRight();  // move option to the right
    virtual void doKeyLeft();   // move option to the left

    virtual void doEnter() { doCross_Pressed(); }
    virtual void doEscape() { doCircle_Pressed(); }
};
