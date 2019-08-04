//
// Created by screemer on 2/13/19.
//

#include "ps_carousel.h"
#include "../gui/gui.h"
#include "../engine/scanner.h"
#include "ra_integrator.h"
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#define SLOT_SIZE 120

//*******************************
// PsCarouselGame::loadTex
//*******************************
void PsCarouselGame::loadTex(SDL_Shared<SDL_Renderer> renderer, RAIntegrator* raIntegrator) {
    shared_ptr<Gui> gui(Gui::getInstance());

    if (!(*this)->foreign) {
        if (coverPng == nullptr) {
            SDL_Shared<SDL_Texture> renderSurface = SDL_CreateTexture(renderer,
                                                                      SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_TARGET,
                                                                      226, 226);
            SDL_Rect fullRect;

            SDL_SetRenderTarget(renderer, renderSurface);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetTextureBlendMode(renderSurface, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
            SDL_RenderFillRect(renderer, nullptr);
            SDL_SetTextureBlendMode(renderSurface, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            string imagePath = (*this)->folder + sep + (*this)->base + ".png";
            SDL_SetRenderTarget(renderer, nullptr);
            if (DirEntry::exists(imagePath)) {
                coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
            } else {
                coverPng = nullptr;
#if defined(__x86_64__) || defined(_M_X64)
                if ((*this)->internal) {
                    Metadata md;
                    if (md.lookupBySerial((*this)->serial) && md.bytes && md.dataSize) {
                        char fname[] = "/tmp/AutoBleem_XXXXXX.png";
                        int pngFile = mkstemps(fname, 4);
                        write(pngFile, md.bytes, md.dataSize);
                        close(pngFile);
                        coverPng = IMG_LoadTexture(renderer, fname);
                        remove(fname);
                    }
                }
#endif
            }

            if (coverPng != nullptr) {
                SDL_SetRenderTarget(renderer, renderSurface);
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
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
                    SDL_RenderCopy(renderer, coverPng, &fullRect, &outputRect);
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                }
                coverPng = nullptr;

                fullRect.x = 0;
                fullRect.y = 0;
                fullRect.h = 226, fullRect.w = 226;
                if (gui->cdJewel != nullptr) {
                    SDL_RenderCopy(renderer, gui->cdJewel, &fullRect, &fullRect);
                }
                coverPng = renderSurface;
            }
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }
    } else {
        if (coverPng == nullptr) {
            SDL_Shared<SDL_Texture> renderSurface = SDL_CreateTexture(renderer,
                                                                      SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_TARGET,
                                                                      226, 226);
            SDL_Rect fullRect;

            SDL_SetRenderTarget(renderer, renderSurface);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetTextureBlendMode(renderSurface, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
            SDL_RenderFillRect(renderer, nullptr);
            SDL_SetTextureBlendMode(renderSurface, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            string imagePath = string(RA_FOLDER) + sep + "thumbnails" + sep +
                    DirEntry::getFileNameWithoutExtension((*this)->db_name) + sep +
                    "Named_Boxarts" + sep + raIntegrator->escapeName((*this)->title) + ".png";


            SDL_SetRenderTarget(renderer, nullptr);
            if (DirEntry::exists(imagePath)) {
                coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
            } else {
                coverPng = IMG_LoadTexture(renderer, (DirEntry::getWorkingPath() + "/evoimg/ra-cover.png").c_str());
            }

            SDL_Rect imageCoverRect;
            int w,h;



            SDL_SetRenderTarget(renderer, renderSurface);
            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.h = 226, fullRect.w = 226;

            Uint32 format;
            int access;
            SDL_QueryTexture(coverPng, &format, &access, &fullRect.w, &fullRect.h);
            float aspectRatio = (fullRect.w*1.0f)/(fullRect.h*1.0f);
            SDL_Rect outputRect;

            // calculate output rect with aspect ratio
            int biggerSize = fullRect.w>fullRect.h ? fullRect.w : fullRect.h;


            outputRect.x = 0;
            outputRect.y = 0;
            outputRect.h = (226*fullRect.h)/biggerSize;
            outputRect.w = (226*fullRect.w)/biggerSize;
            outputRect.x = (226-outputRect.w)/2;
            outputRect.y = (226-outputRect.h)/2;


            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
            SDL_RenderCopy(renderer, coverPng, &fullRect, &outputRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            SDL_Shared<SDL_Texture> titleName;
            SDL_Rect titleRect;
            gui->getTextAndRect(renderer,0,0,(*this)->title.c_str(),gui->themeFont,&titleName,&titleRect);

            outputRect.x = 0;
            outputRect.y = 0;
            outputRect.h = 26;
            outputRect.w = 226;
            if (!DirEntry::exists(imagePath)) {
                SDL_RenderCopy(renderer, titleName, &titleRect, &outputRect);
            }

            coverPng = nullptr;

            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.h = 226, fullRect.w = 226;
            coverPng = renderSurface;

            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }
    }
}

//*******************************
// PsCarouselGame::freeTex
//*******************************
void PsCarouselGame::freeTex() {
    coverPng = nullptr;
}

//*******************************
// PsCarousel::createCoverPoint
//*******************************
PsScreenpoint PsCarousel::createCoverPoint(int x, int shade, int side) {
    shade = 255;
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