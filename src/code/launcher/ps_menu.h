//
// Created by screemer on 2/16/19.
//

#pragma once

#include <SDL2/SDL_render.h>
#include "ps_obj.h"
#include <string>

#define TR_MENUON 0
#define TR_OPTION 1

//******************
// PsMenu
//******************
class PsMenu : public PsObj {
public:
    SDL_Texture *settings= nullptr;
    SDL_Texture *guide= nullptr;
    SDL_Texture *memcard= nullptr;
    SDL_Texture *savestate= nullptr;
    SDL_Texture *resume= nullptr;

    int x,y,oy,ox;
    int xoff[4] = {0,0,0,0};
    int yoff[4] = {0,0,0,0};

    float optionscales[4] = {1.0f,1.0f,1.0f,1.0f};

    float maxZoom = 1.5;

    int selOption=0;
    int animationStarted=0;
    int targety;
    int duration;
    bool active = false;

    int direction = 0;

    std::string path;

    void loadAssets();
    void freeAssets();
    void update(long time);
    void render();

    void setResumePic(std::string picturePath);

    int transition =0;

    PsMenu(SDL_Renderer *renderer1, std::string name1, std::string texPath = "");
};
