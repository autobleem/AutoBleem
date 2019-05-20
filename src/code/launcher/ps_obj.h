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
    int x, y, w, h;
    int ox, oy, ow, oh;

    std::string name;

    SDL_Texture *tex = nullptr;

    bool visible = false;
    long lastTime = 0;

    PsObj(SDL_Renderer *renderer1, std::string name1, std::string texPath = "");

    virtual void load(std::string imagePath);

    virtual void destroy();

    virtual void update(long time) {};

    virtual void render();
};
