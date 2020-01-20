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
        : GuiOptionsMenuBase(_renderer, "-=" + _("Configuration") + "=-") {}

    void init() override;
    void render() override;

    std::string getPrevNextOption(const std::vector<std::string> & list, const std::string & current, bool next);
    void doPrevNextOption(shared_ptr<Gui> gui, shared_ptr<Lang> lang, bool next);
    std::string getBooleanIcon(const std::string & input);
    void renderOptionLine(const std::string & text, int cfgIndex, int offset);

    int exitCode=0;
    std::shared_ptr<Lang> lang;

    std::vector<std::string> autobleemUIThemes;
    std::vector<std::string> origames;
    std::vector<std::string> ui;
    std::vector<std::string> jewels;
    std::vector<std::string> music;
    std::vector<std::string> nomusic;
    std::vector<std::string> aspect;
    std::vector<std::string> quickboot;
    std::vector<std::string> quickmenu;
    std::vector<std::string> mip;
    std::vector<std::string> raconfig;
    std::vector<std::string> showingtimeout;
    std::vector<std::string> languages;

    virtual int getVerticalSize() { return CFG_SIZE; }

    virtual void doCircle_Pressed();
    virtual void doCross_Pressed();

    virtual void doJoyRight();  // move option to the right, may fast forwward
    virtual void doJoyLeft();   // move option to the left, may fast forwward

    virtual void doKeyRight();  // move option to the right
    virtual void doKeyLeft();   // move option to the left

    virtual void doEnter() { doCross_Pressed(); }
    virtual void doEscape() { doCircle_Pressed(); }
};
