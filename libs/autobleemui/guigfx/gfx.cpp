//
// Created by screemer on 2019-12-11.
//


#include "gfx.h"
#include "application.h"


void Gfx::flip()
{
    SDL_RenderPresent(Application::renderer);
}

void Gfx::clear()
{
    SDL_SetRenderDrawColor(Application::renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(Application::renderer);
}

void Gfx::drawImage(GfxImage image, int x, int y, int w, int h)
{
    GfxSize size = getImageSize(image);
    SDL_Rect src;
    src.x=0;    src.y=0;    src.w=size.w;    src.h=size.h;
    if ((w==0) && (h==0))
    {
        w=size.w; h=size.h;
    }
    SDL_Rect dest;
    dest.x=x;    dest.y=y;    dest.w=w;    dest.h=h;
    SDL_RenderCopy(Application::renderer,image,&src,&dest);
}
GfxSize Gfx::getImageSize(GfxImage image)
{
    int w,h;
    SDL_QueryTexture(image, NULL, NULL, &w, &h);
    GfxSize size;
    size.w = w;
    size.h = h;
    return size;
}

GfxImage Gfx::loadImage(std::string path)
{
    return IMG_LoadTexture(Application::renderer,path.c_str());
}

void Gfx::setImageBlend(GfxImage image)
{
    SDL_SetTextureBlendMode(image,SDL_BLENDMODE_BLEND);
}
void Gfx::setImageNoBlend(GfxImage image)
{
    SDL_SetTextureBlendMode(image,SDL_BLENDMODE_NONE);
}
void Gfx::setImageAlpha(GfxImage image, int alpha)
{
    SDL_SetTextureAlphaMod(image, alpha);
}