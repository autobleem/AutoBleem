//
// Created by screemer on 2019-01-24.
//

#include "gui_splash.h"
#include "guirender.h"

void GuiSplash::render() {
    shared_ptr<GuiRender> splash(GuiRender::getInstance());
    int w, h; // texture width & height
    SDL_SetTextureBlendMode(splash->img, SDL_BLENDMODE_BLEND);
    SDL_QueryTexture(splash->img, NULL, NULL, &w, &h);
    splash->texr.x = 0;
    splash->texr.y = 0;
    splash->texr.w = w;
    splash->texr.h = h;
    SDL_QueryTexture(splash->logo, NULL, NULL, &w, &h);

    SDL_Texture *textTex;
    SDL_Rect textRec;
    splash->getTextAndRect(renderer, 0, atoi(splash->themeData.values["ttop"].c_str()),
                   ("AutoBleem " + splash->cfg.inifile.values["version"]).c_str(), splash->Sans, &textTex, &textRec);
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_SetTextureAlphaMod(splash->img, alpha);
    SDL_SetTextureAlphaMod(splash->logo, alpha);
    SDL_SetTextureAlphaMod(textTex, alpha);
    Mix_VolumeMusic(alpha / 3);

    SDL_RenderCopy(renderer, splash->img, NULL, &splash->texr);
    SDL_RenderCopy(renderer, splash->logo, NULL, &splash->logor);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(splash->themeData.values["textx"].c_str());
    rect.y = atoi(splash->themeData.values["texty"].c_str());
    rect.w = atoi(splash->themeData.values["textw"].c_str());
    rect.h = atoi(splash->themeData.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_RenderPresent(renderer);
}

void GuiSplash::loop() {
    shared_ptr<GuiRender> splash(GuiRender::getInstance());

    Mix_VolumeMusic(0);
    alpha = 0;
    start = SDL_GetTicks();
    while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                break;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                break;
        }
        render();
        int current = SDL_GetTicks();
        int time = current - start;
        if (time > 5) {
            if (alpha < 255) {
                alpha += 1;
            } else {

                break;
            }
            start = SDL_GetTicks();
        }

    }
}