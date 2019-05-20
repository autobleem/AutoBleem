//
// Created by screemer on 2/13/19.
//

#include "ps_carousel.h"
#include "../gui/gui.h"

using namespace std;

#define SLOT_SIZE 120

//*******************************
// PsCarouselGame::loadTex
//*******************************
void PsCarouselGame::loadTex(SDL_Renderer *renderer) {
    shared_ptr<Gui> gui(Gui::getInstance());

    if (coverPng == nullptr) {
        SDL_Texture *renderSurface = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 226,
                                                       226);
        SDL_SetRenderTarget(renderer, renderSurface);
        SDL_RenderClear(renderer);
        SDL_Rect fullRect;

        string imagePath = (*this)->folder + Util::separator() + (*this)->base + ".png";
        if (Util::exists(imagePath)) {
            coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
        } else coverPng = nullptr;

        if (coverPng != nullptr) {
            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.h = 226, fullRect.w = 226;

            Uint32 format;
            int access;
            SDL_QueryTexture(coverPng, &format, &access, &fullRect.w, &fullRect.h);

            SDL_Rect outputRect;
            if (gui->cdJewel != nullptr) {
                outputRect.x = 23;
                outputRect.y = 5;
                outputRect.h = 217;
                outputRect.w = 199;
            } else {
                outputRect.x = 0;
                outputRect.y = 0;
                outputRect.h = 226;
                outputRect.w = 226;
            }
            if (coverPng != nullptr) {
                SDL_RenderCopy(renderer, coverPng, &fullRect, &outputRect);
            }
            if (coverPng != nullptr) {
                SDL_DestroyTexture(coverPng);
                coverPng = nullptr;
            }

            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.h = 226, fullRect.w = 226;
            if (gui->cdJewel != nullptr) {
                SDL_RenderCopy(renderer, gui->cdJewel, &fullRect, &fullRect);
            }
            coverPng = renderSurface;
        }
        SDL_SetRenderTarget(renderer, nullptr);
    }
}

//*******************************
// PsCarouselGame::freeTex
//*******************************
void PsCarouselGame::freeTex() {
    if (coverPng != nullptr) {
        SDL_DestroyTexture(coverPng);
        coverPng = nullptr;
    }
}

//*******************************
// PsCarousel::createCoverPoint
//*******************************
PsScreenpoint PsCarousel::createCoverPoint(int x, int shade, int side) {
    shade=255;
    if (side == 0) {
        PsScreenpoint point;
        point.x = 405 - SLOT_SIZE * x;
        point.y = 100;
        point.scale = 0.5f;
        point.shade = shade;
        return point;
    } else {
        PsScreenpoint point;
        point.x = 405 + 357 + SLOT_SIZE * x;
        point.y = 100;
        point.scale = 0.5f;
        point.shade = shade;
        return point;
    }
}

//*******************************
// PsCarousel::initCoverPositions
//*******************************
void PsCarousel::initCoverPositions() {
    //405 x 100
    coverPositions.clear();

    coverPositions.push_back(createCoverPoint(5, 40, 0));
    coverPositions.push_back(createCoverPoint(4, 70, 0));
    coverPositions.push_back(createCoverPoint(3, 90, 0));
    coverPositions.push_back(createCoverPoint(2, 100, 0));
    coverPositions.push_back(createCoverPoint(1, 128, 0));
    coverPositions.push_back(createCoverPoint(0, 150, 0));

    PsScreenpoint point;
    point.x = 640 - 113;
    point.y = 180;
    point.scale = 1;
    point.shade = 255;
    coverPositions.push_back(point);

    coverPositions.push_back(createCoverPoint(0, 150, 1));
    coverPositions.push_back(createCoverPoint(1, 128, 1));
    coverPositions.push_back(createCoverPoint(2, 100, 1));
    coverPositions.push_back(createCoverPoint(3, 90, 1));
    coverPositions.push_back(createCoverPoint(4, 70, 1));
    coverPositions.push_back(createCoverPoint(5, 40, 1));

    // special point to move it up
}