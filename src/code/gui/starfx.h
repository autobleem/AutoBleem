//
// Created by screemer on 2019-07-31.
//

#pragma once

#include <vector>
#include <SDL2/SDL_render.h>
#include "gui_sdl_wrapper.h"

#define STARS_PER_LAYER 20
#define SPEED_DIFFERENCE 1.0f
#define SIZE_DIFFERENCE 0.34f

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720


using namespace std;

class RGB {
public:
    RGB() : RGB(0, 0, 0, 0) {};

    RGB(unsigned char r, unsigned char g, unsigned char b) : RGB(r, g, b, 0xFF) {};

    RGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    unsigned char r, g, b, a;


};

class Star {
public:
    float x, y;
    RGB color;
    float speed;
    float size;
};

class StarFx {
public:
    StarFx();
    void render();
    SDL_Shared<SDL_Renderer> renderer;

private:
    vector<Star> starLayers[7];
};


