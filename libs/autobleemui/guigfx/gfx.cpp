//
// Created by screemer on 2019-12-11.
//


#include "gfx.h"
#include "application.h"


void Gfx::flip()
{
    SDL_RenderPresent(Application::renderer);
}

SDL_Shared<SDL_Texture> Gfx::loadImage(std::string path)
{
    return IMG_LoadTexture(Application::renderer,path.c_str());
}