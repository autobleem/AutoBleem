//
// Created by screemer on 2/11/19.
//

#pragma once

#include <SDL2/SDL_render.h>
#include <string>

//******************
// PsObj
//******************
class PsObj {
public:
    SDL_Renderer *renderer;
    int x = 0, y = 0, w = 0, h = 0;
    int ox = 0, oy = 0, ow = 0, oh = 0;

    std::string name;

    SDL_Texture *tex = nullptr;

    bool visible = false;
    long lastTime = 0;

    PsObj(SDL_Renderer *renderer1, std::string name1, std::string texPath = "");

    virtual void load(const std::string & imagePath);

    virtual void destroy();

    virtual void update(long time) {};

    virtual void render();
};
