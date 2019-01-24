//
// Created by screemer on 2019-01-24.
//

#include "gui_about.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "guirender.h"

void GuiAbout::render()
{
    shared_ptr<GuiRender> splash(GuiRender::getInstance());

    SDL_Texture *textTex;
    SDL_Rect textRec;
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,  splash->img, NULL, &splash->texr);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,  splash->img, NULL, &splash->texr);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(splash->themeData.values["textx"].c_str());
    rect.y = atoi(splash->themeData.values["texty"].c_str());
    rect.w = atoi(splash->themeData.values["textw"].c_str());
    rect.h = atoi(splash->themeData.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    SDL_Rect rect2;
    rect2.x = atoi(splash->themeData.values["opscreenx"].c_str());
    rect2.y = atoi(splash->themeData.values["opscreeny"].c_str());
    rect2.w = atoi(splash->themeData.values["opscreenw"].c_str());
    rect2.h = atoi(splash->themeData.values["opscreenh"].c_str());
    SDL_RenderFillRect(renderer, &rect2);

    SDL_Rect logoRect;
    logoRect.x = rect2.x;
    logoRect.y = rect2.y;
    logoRect.w = splash->logor.w / 3;
    logoRect.h = splash->logor.h / 3;
    SDL_RenderCopy(renderer, splash->logo, NULL, &logoRect);

    splash->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h,
                   "AutoBleem - Automatic PlayStation Classic game scanner and Boot Menu", splash->Sans, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
    splash->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h,
                   "2018-2019 code by ScreemerPL / testing nex and justAndy", splash->Sans, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);

    splash->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 3,
                   "Support WWW: https://github.com/screemerpl/cbleemsync", splash->Sans, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
    splash->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 4,
                   "This is free and open source software. It works AS IS and I take no reponsibility for any issues.",
                   splash->Sans, &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
    splash->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 5,
                   "If you want to donate this project send me some small amount to PayPal: screemer1@o2.pl ", splash->Sans,
                   &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);

    splash->getTextAndRect(renderer, rect2.x + 10, logoRect.y + logoRect.h + textRec.h * 7,
                   "If you want just to chat join ModMyClassic Discord channel and .. find 'screemer' ... ", splash->Sans,
                   &textTex, &textRec);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);

    splash->getEmojiTextTexture(renderer,"Press |@X| to go back|",splash->Sans,  &textTex,
                        &textRec);
    textRec.y = atoi(splash->themeData.values["ttop"].c_str());

    if (textRec.w > atoi(splash->themeData.values["textw"].c_str())) textRec.w = atoi(splash->themeData.values["textw"].c_str());
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
                case SDL_JOYBUTTONUP:  /* Handle Joystick Button Presses */
                    //drawText("Button:"+to_string(e.jbutton.button));

                    if (e.jbutton.button == 2) {

                        menuVisible = false;

                    };


            }

        }
    }
}