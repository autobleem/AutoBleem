//
// Created by screemer on 2/12/19.
//

#pragma once

#include "ps_obj.h"
#include "ps_game.h"
#include <SDL2/SDL_ttf.h>

class PsGame;

//******************
// PsMeta
//******************
class PsMeta : public PsObj {
public:
    std::string gameName;
    std::string publisher;
    std::string year;
    std::string players;
    std::string serial = "";
    std::string region = "";

    TTF_Font *font30;
    TTF_Font *font15;
    TTF_Font *font24;

    SDL_Texture *discsTex = nullptr;
    SDL_Texture *gameNameTex = nullptr;
    SDL_Texture *publisherAndYearTex = nullptr;
    SDL_Texture *serialAndRegionTex = nullptr;
    SDL_Texture *playersTex = nullptr;

    SDL_Texture *internalOnTex = nullptr;
    SDL_Texture *internalOffTex = nullptr;
    SDL_Texture *hdOnTex = nullptr;
    SDL_Texture *hdOffTex = nullptr;
    SDL_Texture *lockOnTex = nullptr;
    SDL_Texture *lockOffTex = nullptr;
    SDL_Texture *cdTex = nullptr;
    SDL_Texture *favoriteTex = nullptr;

    int nextPos = 0;
    int prevPos = 0;
    long animEndTime = 0;
    long animStarted = 0;

    bool internal = false;
    bool hd = false;
    bool locked = false;
    bool discs = 1;
    bool favorite = false;

    void updateTexts(std::string gameNameTxt, std::string publisherTxt,
                     std::string yearTxt, const std::string & serial, const std::string & region, std::string playersTxt, bool internal, bool hd, bool locked, int discs, bool favorite, int r, int g,
                     int b);

    void updateTexts(PsGamePtr & game, int r, int g, int b);

    void destroy();

    void render();

    void update(long time);

    using PsObj::PsObj;
private:
    SDL_Texture *createTextTex(std::string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font);
};
