//
// Created by screemer on 2019-01-24.
//
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>

//********************
// GuiScreen
//********************
class GuiScreen {
public:
    virtual void init(){};
    virtual void render()=0;
    virtual void loop()=0;

    void show()
    {
        init();
        render();
        loop();
    }

    GuiScreen(SDL_Renderer * renderer1)
    {
        renderer=renderer1;

    };
    SDL_Renderer * renderer;
};
