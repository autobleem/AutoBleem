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
    oy=y;
    ox=x;

}

void PsMenu::freeAssets()
{
    SDL_DestroyTexture(settings);
    SDL_DestroyTexture(guide);
    SDL_DestroyTexture(memcard);
    SDL_DestroyTexture(savestate);
}

void PsMenu::update(long time)
{
    if (animationStarted!=0)
    {
        float progress = time - animationStarted;
        progress = progress / (duration * 1.0f);
        if (progress > 1) progress = 1;
        if (progress < 0) progress = 0;

        if (transition==TR_MENUON) {
            y = oy + (progress * (targety - oy));

            if (active) {
                switch (selOption) {
                    case 0:
                        settings_scale = 1 + progress * (maxZoom - 1);
                        int neww = 118 * settings_scale;
                        int newh = 118 * settings_scale;

                        settingsxoff = -((neww - 118) / 2);
                        settingsyoff = -((newh - 118) / 2);
                        break;
                }
            } else {
                switch (selOption) {
                    case 0:
                        settings_scale = 1 + (1 - progress) * (maxZoom - 1);
                        int neww = 118 * settings_scale;
                        int newh = 118 * settings_scale;

                        settingsxoff = -((neww - 118) / 2);
                        settingsyoff = -((newh - 118) / 2);
                        break;
                }
            }

            if (progress == 1) {
                oy = y;
                animationStarted = 0;
                if (active) {
                    switch (selOption) {
                        case 0:
                            settings_scale = 1 + (maxZoom - 1);
                            int neww = 118 * settings_scale;
                            int newh = 118 * settings_scale;

                            settingsxoff = -((neww - 118) / 2);
                            settingsyoff = -((newh - 118) / 2);
                            break;
                    }


                } else {
                    switch (selOption) {
                        case 0:
                            settings_scale = 1;
                            int neww = 118 * settings_scale;
                            int newh = 118 * settings_scale;

                            settingsxoff = -((neww - 118) / 2);
                            settingsyoff = -((newh - 118) / 2);
                            break;
                    }
                }
            }
        }
    }
}



void PsMenu::render()
{
    int w = 118*settings_scale;
    int h = 118*settings_scale;
    SDL_Rect input, output;
    input.x=0, input.y=0;input.h=118, input.w=118;
    output.x=x+settingsxoff; output.y=y+settingsyoff; output.w=w;output.h=h;

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