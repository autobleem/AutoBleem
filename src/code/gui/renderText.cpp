#include "renderText.h"

using namespace std;

// global renderText
void renderText(SDL_Renderer * renderer, int x, int y, const std::string & text, const SDL_Color & textColor, TTF_Font *font, bool center, bool background) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;

    if (text.size() == 0) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);
        rect.x = 0;
        rect.y = 0;
        rect.h = 0;
        rect.w = 0;
    } else {
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        text_width = surface->w;
        text_height = surface->h;
        SDL_FreeSurface(surface);
        rect.x = x;
        rect.y = y;
        rect.w = text_width;
        rect.h = text_height;
    }
    SDL_Rect inputRect;
    inputRect.x = 0;
    inputRect.y = 0;
    inputRect.w = rect.w;
    inputRect.h = rect.h;

    if (center) {
        rect.x = 640 - (rect.w / 2);
    }

    if (background) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 70);
        SDL_Rect backRect;
        backRect.x = rect.x - 10;
        backRect.y = rect.y - 2;
        backRect.w = rect.w + 20;
        backRect.h = rect.h + 4;

        SDL_RenderFillRect(renderer, &backRect);
    }

    SDL_RenderCopy(renderer, texture, &inputRect, &rect);
    SDL_DestroyTexture(texture);
};
