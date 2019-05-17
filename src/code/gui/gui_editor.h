//
// Created by screemer on 2019-01-25.
//
#pragma once

#include "gui_screen.h"
#include "../engine/inifile.h"
#include "../launcher/ps_game.h"
#include <vector>

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

    int highres;
    int speedhack;

    int clock;
    int frameskip;

    std::string gpu="";
    int dither;

    int scanlines;
    int scanlineLevel;
    int interpolation;

    PsGamePtr gameData;

    bool internal = false;
    int selOption = 5;
    std::string lastName = "";
    bool changes=false;

    void processOptionChange(bool direction);

    using GuiScreen::GuiScreen;
    SDL_Texture * cover= nullptr;
};
