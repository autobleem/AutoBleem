//
// Created by screemer on 2/16/19.
//

#include "ps_menu.h"

PsMenu::PsMenu(SDL_Renderer *renderer1, string name1, string texPath) : PsObj(renderer1, name1, "")
{
    path = texPath;
    loadAssets();
}
void PsMenu::loadAssets()
{
    settings = IMG_LoadTexture(renderer, (path+"/CB/Setting_ICN.png").c_str());
    guide = IMG_LoadTexture(renderer, (path+"/CB/Manual_ICN.png").c_str());
    memcard = IMG_LoadTexture(renderer, (path+"/CB/MemoryCard_ICN.png").c_str());
    savestate = IMG_LoadTexture(renderer, (path+"/CB/Resume.png").c_str());
    x=640-118/2;
    y=520;

}

void PsMenu::freeAssets()
{

}

void PsMenu::update()
{

}



void PsMenu::render()
{
    int w = 118*settings_scale;
    int h = 118*settings_scale;
    SDL_Rect input, output;
    input.x=0, input.y=0;input.h=118, input.w=118;
    output.x=x;output.y=y;output.w=w;output.h=h;

    SDL_RenderCopy(renderer,settings, &input, &output);

    w = 118*guide_scale;
    h = 118*guide_scale;

    input.x=0, input.y=0;input.h=118, input.w=118;
    output.x=x+130;
    output.y=y;output.w=w;output.h=h;

    SDL_RenderCopy(renderer,guide, &input, &output);

    input.x=0, input.y=0;input.h=118, input.w=118;
    output.x=x+260;
    output.y=y;output.w=w;output.h=h;

    SDL_RenderCopy(renderer, memcard, &input, &output);

    input.x=0, input.y=0;input.h=118, input.w=118;
    output.x=x+130*3;
    output.y=y;output.w=w;output.h=h;

    SDL_RenderCopy(renderer, savestate, &input, &output);
}