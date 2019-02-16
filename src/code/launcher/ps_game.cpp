//
// Created by screemer on 2/12/19.
//

#include "ps_game.h"
#include "../util.h"


void PsGame::loadTex(SDL_Renderer *renderer) {
    if (coverPng == nullptr) {
        string imagePath = this->folder + Util::separator() + this->base + ".png";
        coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
    }

}

void PsGame::freeTex() {
    if (coverPng != nullptr) {
        SDL_DestroyTexture(coverPng);
        coverPng = nullptr;
    }


}

PsGame *PsGame::clone() {
    PsGame *clone = new PsGame();
    clone->visible = this->visible;
    clone->gameId = this->gameId;
    clone->folder = this->folder;
    clone->title = this->title;
    clone->base = this->base;
    clone->ssFolder = this->ssFolder;
    clone->memcard = this->memcard;
    return clone;
}