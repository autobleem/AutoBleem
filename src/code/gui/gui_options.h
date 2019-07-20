//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_screen.h"
#include <string>
#include <vector>

//********************
// GuiOptions
//********************
class GuiOptions : public GuiScreen{
public:
    void init();
    void render();
    void loop();
    std::string getOption(const std::vector<std::string> & list, const std::string & current, bool next);
    std::string getBooleanIcon(const std::string & input);
    void renderOptionLine(const std::string & text, int pos, int offset);

    int selOption=0;
    int totalHeight=0;
    int exitCode=0;

    std::vector<std::string> themes;
    std::vector<std::string> sthemes;
    std::vector<std::string> pcsx;
    std::vector<std::string> mip;
    std::vector<std::string> nomusic;
    std::vector<std::string> autoregion;
    std::vector<std::string> quickboot;
    std::vector<std::string> quickmenu;
    std::vector<std::string> retroarch;
    std::vector<std::string> adv;
    std::vector<std::string> languages;
    std::vector<std::string> ui;
    std::vector<std::string> aspect;
    std::vector<std::string> origames;
    std::vector<std::string> jewels;
    std::vector<std::string> music;
    std::vector<std::string> showingtimeout;

    using GuiScreen::GuiScreen;
};
