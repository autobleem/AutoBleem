//
// Created by screemer on 2/8/19.
//

#include "gui_launcher.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "../gui/gui.h"
#include "../lang.h"


void GuiLauncher::loadAssets() {
    shared_ptr<Gui> gui(Gui::getInstance());
    backgroundImg = IMG_LoadTexture(renderer, (gui->getSonyImagePath() + "/GR/JP_US_BG.png").c_str());
    footerImg = IMG_LoadTexture(renderer, (gui->getSonyImagePath() + "/GR/Footer.png").c_str());
    functionImg = IMG_LoadTexture(renderer, (gui->getSonyImagePath() + "/CB/Function_BG.png").c_str());
}

void GuiLauncher::freeAssets() {
    SDL_DestroyTexture(backgroundImg);
    SDL_DestroyTexture(footerImg);
    SDL_DestroyTexture(functionImg);
}

void GuiLauncher::init() {
    loadAssets();
}

void GuiLauncher::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundImg, nullptr, &gui->backgroundRect);

    SDL_Rect footerRect;
    footerRect.x = 0;
    footerRect.y = 632;
    footerRect.w = 1280;
    footerRect.h = 88;
    SDL_RenderCopy(renderer, footerImg, nullptr, &footerRect);

    SDL_Rect functionRect;
    functionRect.x = 0;
    functionRect.y = 632 - 229;
    functionRect.w = 1280;
    functionRect.h = 229;
    SDL_RenderCopy(renderer, functionImg, nullptr, &functionRect);


    gui->renderStatus("|@X| " + _("Play") + "  |@O| " + _("Go back") + "|");
    SDL_RenderPresent(renderer);
}

void GuiLauncher::loop() {
    bool menuVisible = true;
    while (menuVisible) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYBUTTONUP:


                    if (e.jbutton.button == PCS_BTN_CIRCLE) {

                        menuVisible = false;

                    };


            }

        }
    }
    freeAssets();

}