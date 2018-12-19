//
// Created by screemer on 2018-12-19.
//

#ifndef CBLEEMSYNC_SPLASH_H
#define CBLEEMSYNC_SPLASH_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>

using namespace std;

class Splash {
private:
    Splash() {}

public:
    void display();

    void finish();

    void drawText(string text);

    SDL_Rect texr;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *img = NULL;
    TTF_Font *Sans = TTF_OpenFont("Sans.ttf", 24);


    Splash(Splash const &) = delete;

    Splash &operator=(Splash const &) = delete;

    static std::shared_ptr<Splash> getInstance() {
        static std::shared_ptr<Splash> s{new Splash};
        return s;
    }
};


#endif //CBLEEMSYNC_SPLASH_H
