//
// Created by screemer on 2/12/19.
//

#include "ps_game.h"
#include "../util.h"

void PsGame::loadTex(SDL_Renderer *renderer) {
    string imagePath = this->folder + Util::separator() + this->base + ".png";
    coverPng = IMG_LoadTexture(renderer, imagePath.c_str());


}

void PsGame::freeTex() {
    if (coverPng != nullptr) {
        SDL_DestroyTexture(coverPng);
        coverPng = nullptr;
    }
}