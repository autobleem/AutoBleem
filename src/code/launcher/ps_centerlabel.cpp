//
// Created by screemer on 2019-02-21.
//

#include "ps_centerlabel.h"
using namespace std;

//*******************************
// PsCenterLabel::PsCenterLabel
//*******************************
PsCenterLabel::PsCenterLabel(SDL_Shared<SDL_Renderer> renderer1, const string & name1, const string & texPath) : PsObj(renderer1,name1,"")
{

}

//*******************************
// PsCenterLabel::setText
//*******************************
void PsCenterLabel::setText(const string & text, int r, int g, int b)
{
    texture = createTextTex(text,r,g,b,font);
    Uint32 format;
    int access;
    int w, h;
    SDL_Rect rect;
    SDL_Rect fullRect;

    SDL_QueryTexture(texture, &format, &access, &w, &h);
    this->w=w;
    this->h=h;
    this->x = 640-w/2;

}

//*******************************
// PsCenterLabel::~PsCenterLabel
//*******************************
PsCenterLabel::~PsCenterLabel()
{
}

//*******************************
// PsCenterLabel::render
//*******************************
void PsCenterLabel::render()
{
    if (visible) {
        SDL_Rect src;
        SDL_Rect dst;
        src.x = 0;
        src.y = 0;
        src.w = w;
        src.h = h;
        dst.x = x;
        dst.y = y;
        dst.w = w;
        dst.h = h;
        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}

//*******************************
// PsCenterLabel::createTextTex
//*******************************
GfxImage  PsCenterLabel::createTextTex(const string & text, Uint8 r, Uint8 g, Uint8 b, TTF_Font_Shared font) {

    SDL_Shared<SDL_Surface> surface;
    GfxImage  texture;
    SDL_Color textColor = {r, g, b, 0};

    if (text.size() == 0) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);

    } else {
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
    }

    return texture;
}