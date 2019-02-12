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
#include "ps_settings_back.h"

void GuiLauncher::renderText(int x, int y, string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Color textColor = {r, g, b, 0};

    if (text.size() == 0) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);
        rect.x = 0;
        rect.y = 0;
        rect.h = 0;
        rect.w = 0;
    } else {
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        text_width = surface->w;
        text_height = surface->h;
        SDL_FreeSurface(surface);
        rect.x = x;
        rect.y = y;
        rect.w = text_width;
        rect.h = text_height;
    }
    SDL_Rect inputRect;
    inputRect.x = 0;
    inputRect.y = 0;
    inputRect.w = rect.w;
    inputRect.h = rect.h;

    SDL_RenderCopy(renderer, texture, &inputRect, &rect);
    SDL_DestroyTexture(texture);


}

void GuiLauncher::renderMeta() {
    if (gameInfoVisible) {
        renderText(785, 285, gameName, 255, 255, 255, font30);
        renderText(785, 328, publisher, 255, 255, 255, font15);
        renderText(785, 350, year, 255, 255, 255, font15);
    }
}
void GuiLauncher::loadAssets() {
    shared_ptr<Gui> gui(Gui::getInstance());
    long time = SDL_GetTicks();
    // mockup
    gameName = "Resident Evil 2 - Leon";
    publisher = "Capcom.";
    year = "1999";
    players = "2 players";

    font30 = TTF_OpenFont((gui->getSonyFontPath() + "/SST-Bold.ttf").c_str(), 28);
    font15 = TTF_OpenFont((gui->getSonyFontPath() + "/SST-Bold.ttf").c_str(), 15);
    font24 = TTF_OpenFont((gui->getSonyFontPath() + "/SST-Medium.ttf").c_str(), 22);

    auto background = new PsObj(renderer, "background", gui->getSonyImagePath() + "/GR/JP_US_BG.png");
    background->x = 0;
    background->y = 0;
    background->visible = true;
    staticElements.push_back(background);
    auto footer = new PsObj(renderer, "footer", gui->getSonyImagePath() + "/GR/Footer.png");
    footer->y = 720 - footer->h;
    footer->visible = true;
    staticElements.push_back(footer);

    playButton = new PsObj(renderer, "playButton", gui->getSonyImagePath() + "/GR/Acid_C_Btn.png");
    playButton->y = 428;
    playButton->x = 540;
    playButton->visible = true;
    staticElements.push_back(playButton);

    playText = new PsZoomBtn(renderer, "playText", gui->getSonyImagePath() + "/BMP_Text/Play_Text.png");
    playText->y = 428;
    playText->x = 640 - 262 / 2;
    playText->visible = true;
    playText->ox = playText->x;
    playText->oy = playText->y;
    playText->lastTime = time;

    staticElements.push_back(playText);

    settingsBack = new PsSettingsBack(renderer, "playButton", gui->getSonyImagePath() + "/CB/Function_BG.png");
    settingsBack->setCurLen(100);
    settingsBack->visible = true;
    staticElements.push_back(settingsBack);

    // mockup

    auto cover = new PsObj(renderer, "cover", "./default.png");
    cover->x = 527;
    cover->y = 180;
    cover->visible = true;
    staticElements.push_back(cover);



}

void GuiLauncher::freeAssets() {
    for (auto obj:staticElements) {
        obj->destroy();
        delete obj;
    }
    staticElements.clear();
    TTF_CloseFont(font30);
    TTF_CloseFont(font24);
    TTF_CloseFont(font15);
}

void GuiLauncher::init() {
    loadAssets();
}

void GuiLauncher::render()
{

    shared_ptr<Gui> gui(Gui::getInstance());
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);


    for (auto obj:staticElements) {

        obj->render();
    }

    renderMeta();

    renderText(638, 644, "Enter", 80, 80, 80, font24);
    renderText(892, 644, "Console Button Guide", 80, 80, 80, font24);


    // gui->renderStatus("|@X| " + _("Play") + "  |@O| " + _("Go back") + "|");
    SDL_RenderPresent(renderer);
}

void GuiLauncher::loop() {
    bool menuVisible = true;
    while (menuVisible) {
        long time = SDL_GetTicks();
        for (auto obj:staticElements) {
            obj->update(time);
        }
        render();


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

                    if (e.jbutton.button == PCS_BTN_CROSS) {

                        settingsBack->animEndTime = time + 200;
                        settingsBack->nextLen = 280;
                        playButton->visible = false;
                        playText->visible = false;


                    };
                    if (e.jbutton.button == PCS_BTN_SQUARE) {

                        settingsBack->animEndTime = time + 200;
                        settingsBack->nextLen = 100;
                        playButton->visible = true;
                        playText->visible = true;


                    };


            }

        }
    }
    freeAssets();

}