//
// Created by screemer on 2/12/19.
//

#include "ps_meta.h"
#include "ps_game.h"

void
PsMeta::updateTexts(string gameNameTxt, string publisherTxt, string yearTxt, string playersTxt, bool internal, bool hd,
                    bool locked, int discs, bool favorite, int r,int g, int b) {
    this->discs = discs;
    this->internal = internal;
    this->hd = hd;
    this->locked = locked;
    this->favorite = favorite;
    this->gameName = gameNameTxt;
    this->publisher = publisherTxt;
    this->year = yearTxt;
    this->players = playersTxt;

    if (discsTex != nullptr) SDL_DestroyTexture(discsTex);
    if (gameNameTex != nullptr) SDL_DestroyTexture(gameNameTex);
    if (publisherTex != nullptr) SDL_DestroyTexture(publisherTex);
    if (yearTex != nullptr) SDL_DestroyTexture(yearTex);
    if (playersTex != nullptr) SDL_DestroyTexture(gameNameTex);

    gameNameTex = createTextTex(gameName, r,g,b, font30);
    publisherTex = createTextTex(publisher, r,g,b, font15);
    yearTex = createTextTex(year, r,g,b, font15);
    playersTex = createTextTex(playersTxt, r,g,b, font15);
    discsTex = createTextTex(to_string(discs),r,g,b,font15);
}

void PsMeta::updateTexts(PsGame *game, int r,int g, int b) {

    string appendText = game->players == 1 ? "Player" : "Players";

    updateTexts(game->title, game->publisher, to_string(game->year), to_string(game->players) + " " + appendText,
                game->internal, game->hd, game->locked, game->cds, game->favorite, r, g, b);
}

void PsMeta::destroy() {
    if (gameNameTex != nullptr) SDL_DestroyTexture(gameNameTex);
    if (publisherTex != nullptr) SDL_DestroyTexture(publisherTex);
    if (yearTex != nullptr) SDL_DestroyTexture(yearTex);
    if (playersTex != nullptr) SDL_DestroyTexture(playersTex);
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(discsTex);

    if (internalOffTex != nullptr) {

        SDL_DestroyTexture(internalOnTex);
        SDL_DestroyTexture(internalOffTex);
        SDL_DestroyTexture(hdOnTex);
        SDL_DestroyTexture(hdOffTex);
        SDL_DestroyTexture(lockOnTex);
        SDL_DestroyTexture(lockOffTex);
        SDL_DestroyTexture(cdTex);
        SDL_DestroyTexture(favoriteTex);
    }

}

void PsMeta::render() {

    if (gameName=="")
    {
        return;
    }

    if (internalOffTex == nullptr) {
        string curPath = Util::getWorkingPath();
        internalOnTex = IMG_LoadTexture(renderer, (curPath + "/evoimg/ps1.png").c_str());
        internalOffTex = IMG_LoadTexture(renderer, (curPath + "/evoimg/usb.png").c_str());
        hdOnTex = IMG_LoadTexture(renderer, (curPath + "/evoimg/hd.png").c_str());
        hdOffTex = IMG_LoadTexture(renderer, (curPath + "/evoimg/sd.png").c_str());
        lockOnTex = IMG_LoadTexture(renderer, (curPath + "/evoimg/lock.png").c_str());
        lockOffTex = IMG_LoadTexture(renderer, (curPath + "/evoimg/unlock.png").c_str());
        cdTex = IMG_LoadTexture(renderer, (curPath + "/evoimg/cd.png").c_str());
        favoriteTex = IMG_LoadTexture(renderer, (curPath + "/evoimg/favorite.png").c_str());
    }

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

        if (rect.w > 490) rect.w = 490;
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

        int offset = 190, spread=40;
        // render internal icon
        rect.x = x+120;
        SDL_RenderCopy(renderer, cdTex, &fullRect, &rect);

        SDL_QueryTexture(discsTex, &format, &access, &w, &h);
        rect.x =  x+155;
        rect.y = y + 43 + 22 + 30;
        rect.w = w;
        rect.h = h;

        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = w;
        fullRect.h = h;
        SDL_RenderCopy(renderer, discsTex, &fullRect, &rect);



        rect.x = x+offset;
        rect.y = y + 43 + 22 + 28;
        rect.w = 30;
        rect.h = 30;

        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = 30;
        fullRect.h = 30;
        if (internal)
        {
            locked = true;
            hd = false;
            SDL_RenderCopy(renderer, internalOnTex, &fullRect, &rect);
        } else
        {
            SDL_RenderCopy(renderer, internalOffTex, &fullRect, &rect);
        }
        rect.x = x+offset+spread;
        if (hd)
        {
            SDL_RenderCopy(renderer, hdOnTex, &fullRect, &rect);
        } else
        {
            SDL_RenderCopy(renderer, hdOffTex, &fullRect, &rect);
        }
        rect.x = x+offset+spread*2;
        if (locked)
        {
            SDL_RenderCopy(renderer, lockOnTex, &fullRect, &rect);
        } else
        {
            SDL_RenderCopy(renderer, lockOffTex, &fullRect, &rect);
        }
        rect.x = x+offset+spread*3;
        if (favorite)
        {
            SDL_RenderCopy(renderer, favoriteTex, &fullRect, &rect);
        }

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


    if (visible)
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
