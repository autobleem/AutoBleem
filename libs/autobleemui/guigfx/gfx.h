//
// Created by screemer on 2019-12-11.
//

#pragma once

#include <SDL2/SDL_image.h>
#include <memory>
#include "gui_sdl_wrapper.h"

class Gfx {
public:
    Gfx(Gfx const &) = delete;
    Gfx &operator=(Gfx const &) = delete;

    static std::shared_ptr<Gfx> getInstance() {
        static std::shared_ptr<Gfx> gfx{new Gfx};
        return gfx;
    }

    static void flip();
    static void clear();
    static SDL_Shared<SDL_Texture> loadImage(std::string path);
private:
    Gfx( ){};


};



