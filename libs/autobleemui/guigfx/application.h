//
// Created by screemer on 2019-12-11.
//

#pragma once


#include <autobleem/config.h>
#include <autobleem/direntry.h>
#include <autobleem/padmapper.h>
#include "gui_sdl_wrapper.h"
#include "gui_font.h"


//********************
// Application
//********************
class Application {
public:
    SDL_Shared<SDL_Window> window;
    SDL_Shared<SDL_Renderer> renderer;

    Fonts fonts;
    Config cfg;
    bool inGuiLauncher = false;

    std::string getCurrentThemePath();
    std::string getCurrentThemeImagePath();
    std::string getCurrentThemeFontPath();
    std::string getCurrentThemeSoundPath();

    Application();
    ~Application();

    PadMapper mapper;
};

