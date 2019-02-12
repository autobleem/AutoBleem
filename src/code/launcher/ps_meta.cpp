//
// Created by screemer on 2/12/19.
//

#include "ps_meta.h"

void PsMeta::updateTexts(string gameNameTxt, string publisherTxt, string yearTxt, string playersTxt) {
    this->gameName = gameNameTxt;
    this->publisher = publisherTxt;
    this->year = yearTxt;
    this->players = playersTxt;

    if (gameNameTex != nullptr) SDL_DestroyTexture(gameNameTex);
    if (publisherTex != nullptr) SDL_DestroyTexture(publisherTex);
    if (yearTex != nullptr) SDL_DestroyTexture(yearTex);
    if (playersTex != nullptr) SDL_DestroyTexture(gameNameTex);

    gameNameTex = createTextTex(gameName, 255, 255, 255, font30);
    publisherTex = createTextTex(publisher, 255, 255, 255, font15);
    yearTex = createTextTex(year, 255, 255, 255, font15);
    playersTex = createTextTex(playersTxt, 255, 255, 255, font15);


}

void PsMeta::destroy() {
    if (gameNameTex != nullptr) SDL_DestroyTexture(gameNameTex);
    if (publisherTex != nullptr) SDL_DestroyTexture(publisherTex);
    if (yearTex != nullptr) SDL_DestroyTexture(yearTex);
    if (playersTex != nullptr) SDL_DestroyTexture(gameNameTex);
    SDL_DestroyTexture(tex);

}

void PsMeta::render() {

    if (visible) {
        Uint32 format;
        int access;
        int w, h;
        SDL_Rect rect;
        SDL_Rect fullRect;

        SDL_QueryTexture(gameNameTex, &format, &access, &w, &h);
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = w;
        fullRect.h = h;
        SDL_RenderCopy(renderer, gameNameTex, &fullRect, &rect);

        SDL_QueryTexture(publisherTex, &format, &access, &w, &h);

        rect.x = x;
        rect.y = y + 43;
        rect.w = w;
        rect.h = h;

        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = w;
        fullRect.h = h;
        SDL_RenderCopy(renderer, publisherTex, &fullRect, &rect);

        SDL_QueryTexture(yearTex, &format, &access, &w, &h);

        rect.x = x;
        rect.y = y + 43 + 22;
        rect.w = w;
        rect.h = h;

        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = w;
        fullRect.h = h;
        SDL_RenderCopy(renderer, yearTex, &fullRect, &rect);

        SDL_QueryTexture(playersTex, &format, &access, &w, &h);
        rect.x = x + 35;
        rect.y = y + 43 + 22 + 30;
        rect.w = w;
        rect.h = h;

        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = w;
        fullRect.h = h;
        SDL_RenderCopy(renderer, playersTex, &fullRect, &rect);

        SDL_QueryTexture(tex, &format, &access, &w, &h);
        rect.x = x;
        rect.y = y + 43 + 22 + 28;
        rect.w = w;
        rect.h = h;

        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = w;
        fullRect.h = h;
        SDL_RenderCopy(renderer, tex, &fullRect, &rect);

    }
}

SDL_Texture *PsMeta::createTextTex(string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font) {

    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color textColor = {r, g, b, 0};

    if (text.size() == 0) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);

    } else {
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);

    }

    return texture;


}

void PsMeta::update(long time) {

    if (animEndTime != 0) {
        if (animStarted == 0) {
            animStarted = time;

        }

        if (animStarted != 0) {
            // calculate length for point in time
            long currentAnim = time - animStarted;
            long totalAnimTime = animEndTime - animStarted;
            float position = currentAnim * 1.0f / totalAnimTime * 1.0f;
            int newPos = prevPos + ((nextPos - prevPos) * position);
            y = newPos;


        }

        if (time >= animEndTime) {
            animStarted = 0;
            animEndTime = 0;
            y = nextPos;

        }

    }
    lastTime = time;
}