//
// Created by screemer on 2019-12-11.
//

#pragma once

#include <SDL2/SDL_image.h>
#include <memory>
#include "gui_sdl_wrapper.h"

class GfxSize
{
public:
    int w ,h;
};
class Gfx {
public:
    Gfx(Gfx const &) = delete;
    Gfx &operator=(Gfx const &) = delete;

    static std::shared_ptr<Gfx> getInstance() {
        static std::shared_ptr<Gfx> gfx{new Gfx};
        return gfx;
    }

    // general screen routines
    static void flip();
    static void clear();

    // image rendering
    static SDL_Shared<SDL_Texture> loadImage(std::string path);
    static void drawImage(GfxImage image, int x, int y, int w=0, int h=0);
    static GfxSize getImageSize(GfxImage image);
    static void setImageBlend(GfxImage image);
    static void setImageNoBlend(GfxImage image);
    static void setImageAlpha(GfxImage image, int alpha);

private:
    Gfx( ){};


};



