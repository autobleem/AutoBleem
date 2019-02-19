//
// Created by screemer on 2/16/19.
//

#ifndef AUTOBLEEM_GUI_PS_MENU_H
#define AUTOBLEEM_GUI_PS_MENU_H


#include <SDL2/SDL_system.h>
#include "ps_obj.h"

#define TR_MENUON 0
#define TR_OPTION 1

class PsMenu : public PsObj {
public:
    SDL_Texture *settings;
    SDL_Texture *guide;
    SDL_Texture *memcard;
    SDL_Texture *savestate;
    SDL_Texture *separator;

    int x,y,oy,ox;
    int settingsxoff =0;
    int settingsyoff =0;

    float settings_scale = 1.0f;
    float guide_scale = 1.0f;
    float mencard_scale = 1.0f;
    float savestate_scale = 1.0f;

    float maxZoom = 1.5;

    int selOption=0;
    int animationStarted=0;
    int targety;
    int duration;
    bool active = false;

    string path;

    void loadAssets();
    void freeAssets();
    void update(long time);
    void render();

    int transition =0;

    PsMenu(SDL_Renderer *renderer1, string name1, string texPath = "");
};


#endif //AUTOBLEEM_GUI_PS_MENU_H
