#pragma once
// from https://blog.galowicz.de/2016/02/21/automatic_resource_release_with_sdl/
// and modified to use a class with a conversion operator
// Take care to not call the destroy function elsewhere causing it to be destroyed twice.

#include <memory>
#include <SDL2/SDL.h>

static void SDL_DelRes(SDL_Window   *r) { SDL_DestroyWindow(r);   }
static void SDL_DelRes(SDL_Renderer *r) { SDL_DestroyRenderer(r); }
static void SDL_DelRes(SDL_Texture  *r) { SDL_DestroyTexture(r);  }
static void SDL_DelRes(SDL_Surface  *r) { SDL_FreeSurface(r);     }

template <typename T>
struct SDL_Shared {
    std::shared_ptr<T> sdl_shared_ptr;

    SDL_Shared(T* t = nullptr) : sdl_shared_ptr(t, [](T *t)
        { SDL_DelRes(t); } ) {};    // automatically destroy/free when the last shared_ptr goes away

    operator T* () { return sdl_shared_ptr.get(); };
    T & operator * () { return *sdl_shared_ptr.get(); };
    T * operator -> () { return sdl_shared_ptr.get(); };
};
