//
// Created by screemer on 2019-01-25.
//
#pragma once

#include <autobleem/autobleemui.h>
#include <guigfx/gui.h>
#include "../launcher/ps_game.h"

//********************
// GuiEditor
//********************
class GuiEditor : public GuiScreen{
public:
    void init();
    void render();
    void loop();

    void refreshData();
    Inifile gameIni;
    std::string gameFolder; // ex: /Games/Racing/007 Racing

    int highres = 0;
    int speedhack = 0;

    int clock = 0;
    int frameskip = 0;

    std::string gpu="";
    int dither = 0;

    int scanlines = 0;
    int scanlineLevel = 0;
    int interpolation = 0;

    PsGamePtr gameData;

    bool internal = false;
    int selOption = 5;
    std::string lastName;
    bool changes=false;

    void processOptionChange(bool direction);

    using GuiScreen::GuiScreen;
    GfxImage  cover;
};
