//
// Created by screemer on 2019-12-11.
//

#include "gfx.h"
#include "application.h"

void Gfx::flip()
{
    SDL_RenderPresent(Application::renderer);
}