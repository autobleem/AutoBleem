//
// Created by screemer on 2019-01-24.
//

#include "gui_about.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"

void GuiAbout::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());

    SDL_Texture *textTex;
    SDL_Rect textRec;


    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,  gui->backgroundImg, NULL, &gui->backgroundRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(gui->themeData.values["textx"].c_str());
    rect.y = atoi(gui->themeData.values["texty"].c_str());
    rect.w = atoi(gui->themeData.values["textw"].c_str());
    rect.h = atoi(gui->themeData.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    SDL_Rect rect2;
    rect2.x = atoi(gui->themeData.values["opscreenx"].c_str());
    rect2.y = atoi(gui->themeData.values["opscreeny"].c_str());
    rect2.w = atoi(gui->themeData.values["opscreenw"].c_str());
    rect2.h = atoi(gui->themeData.values["opscreenh"].c_str());
    SDL_RenderFillRect(renderer, &rect2);

    SDL_Rect logoRect;
    logoRect.x = rect2.x;
    logoRect.y = rect2.y;
    logoRect.w = gui->logoRect.w / 3;
    logoRect.h = gui->logoRect.h / 3;
    SDL_RenderCopy(renderer, gui->logo, NULL, &logoRect);

    gui->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h,
                   "AutoBleem - Automatic PlayStation Classic game scanner and Boot Menu", gui->font, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
    gui->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h,
                   "2018-2019 code by ScreemerPL / testing nex and justAndy", gui->font, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);

    gui->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 3,
                   "Support WWW: https://github.com/screemerpl/cbleemsync", gui->font, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
    gui->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 4,
                   "This is free and open source software. It works AS IS and I take no reponsibility for any issues.",
                   gui->font, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
    gui->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 5,
                   "If you want to donate this project send me some small amount to PayPal: screemer1@o2.pl ", gui->font,
                   &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);

    gui->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 7,
                   "If you want just to chat join ModMyClassic Discord channel and .. find 'screemer' ... ", gui->font,
                   &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);

    gui->getEmojiTextTexture(renderer,"Press |@X| to go back|",gui->font,  &textTex,
                        &textRec);
    textRec.y = atoi(gui->themeData.values["ttop"].c_str());

    if (textRec.w > atoi(gui->themeData.values["textw"].c_str())) textRec.w = atoi(gui->themeData.values["textw"].c_str());
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);

    SDL_RenderPresent(renderer);
}

void GuiAbout::loop()
{
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


                    if (e.jbutton.button == 2) {

                        menuVisible = false;

                    };


            }

        }
    }
}