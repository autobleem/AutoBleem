//
// Created by screemer on 2/13/19.
//

#pragma once

#include "ps_game.h"
#include <vector>
#include <SDL2/SDL_ttf.h>
#include "../gui/gui_sdl_wrapper.h"
#include "ra_integrator.h"

//******************
// PsScreenpoint
//******************
class PsScreenpoint {
public:
    int x = 0;
    int y = 0;
    float scale = 0.0;
    int shade = 0;
};

//******************
// PsCarouselGame
//******************
// Note that if there are less than 13 games in the gamesList the games are duplicated to fill out the carousel.
// So more than one PsCarouselGame could be using the same PsGamePtr.
struct PsCarouselGame : public PsGamePtr {
    PsCarouselGame() = delete;
    PsCarouselGame(PsGamePtr & game) : PsGamePtr(game) { };

    PsScreenpoint current;
    PsScreenpoint destination;
    PsScreenpoint actual;
    int screenPointIndex = -1;
    int nextPointIndex = -1;
    long animationStart = 0;
    long animationDuration = 0;
    bool visible = false;
    SDL_Shared<SDL_Texture> coverPng;

    void loadTex(SDL_Shared<SDL_Renderer> renderer, RAIntegrator* raIntegrator);
    void freeTex();
};

//******************
// PsCarousel
//******************
class PsCarousel {
public:
    int activeItem = 0;
    int cycleMax = 0;
    int cycleDuration = 0;
    int previousActiveElement = 0;
    std::vector<PsScreenpoint> coverPositions;

    void initCoverPositions();

    PsScreenpoint createCoverPoint(int x, int shade, int side);
};
