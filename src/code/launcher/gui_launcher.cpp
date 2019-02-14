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
#include "pcsx_interceptor.h"


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

void GuiLauncher::updateMeta() {
    PsGame *game = gamesList[selGame];
    gameName = game->title;
    publisher = game->publisher;
    year = to_string(game->year);
    if (game->players == 1) {
        players = to_string(game->players) + " " + _("Player");
    } else {
        players = to_string(game->players) + " " + _("Players");
    }
    meta->updateTexts(gameName, publisher, year, players);
}

void GuiLauncher::loadAssets() {
    shared_ptr<Gui> gui(Gui::getInstance());

    gamesList.clear();
    gui->db->getGames(&gamesList);


    gameName = "";
    publisher = "";
    year = "";
    players = "";

    if (gamesList.empty()) {
        selGame = -1;
    } else {
        selGame = 0;
    }


    long time = SDL_GetTicks();


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

    meta = new PsMeta(renderer, "meta", gui->getSonyImagePath() + "/CB/PlayerOne.png");
    meta->font15 = font15;
    meta->font24 = font24;
    meta->font30 = font30;
    meta->x = 785;
    meta->y = 285;
    meta->visible = true;
    meta->updateTexts(gameName, publisher, year, players);
    staticElements.push_back(meta);
    updateMeta();


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
    for (PsGame *game:gamesList) {
        game->freeTex();
        delete game;

    }
    gamesList.clear();
}

void GuiLauncher::init() {
    loadAssets();
}

void GuiLauncher::render() {

    shared_ptr<Gui> gui(Gui::getInstance());
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);

    for (auto obj:staticElements) {

        obj->render();
    }

    // covers render

    if (!gamesList.empty()) {
        SDL_Texture *currentGameTex = gamesList[selGame]->coverPng;

        SDL_Rect coverRect;
        coverRect.x = 527;
        coverRect.y = 180;
        coverRect.w = 226;
        coverRect.h = 226;


        SDL_Rect fullRect;
        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = 226;
        fullRect.h = 226;
        SDL_SetTextureColorMod(currentGameTex, 255, 255, 255);
        SDL_RenderCopy(renderer, currentGameTex, &fullRect, &coverRect);


    }



    renderText(638, 644, "Enter", 80, 80, 80, font24);
    renderText(892, 644, "Console Button Guide", 80, 80, 80, font24);


    SDL_RenderPresent(renderer);
}

void GuiLauncher::nextGame() {
    shared_ptr<Gui> gui(Gui::getInstance());
    Mix_PlayChannel(-1, gui->cursor, 0);
    selGame++;
    if (selGame >= gamesList.size()) {
        selGame = 0;
    }
    updateMeta();
}

void GuiLauncher::prevGame() {
    shared_ptr<Gui> gui(Gui::getInstance());
    Mix_PlayChannel(-1, gui->cursor, 0);
    selGame--;
    if (selGame < 0) {
        selGame = gamesList.size() - 1;
    }
    updateMeta();
}

void GuiLauncher::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());
    bool menuVisible = true;
    long motionStart = 0;
    long timespeed = 0;
    int motionDir = 0;

    while (menuVisible) {
        long time = SDL_GetTicks();
        for (auto obj:staticElements) {
            obj->update(time);
        }
        render();

        if (motionStart != 0) {
            long timePressed = time - motionStart;

            if (timePressed > 300) {

                if (time - timespeed > 100) {
                    if (motionDir == 0) {
                        nextGame();
                        timespeed = 0;
                    } else {
                        prevGame();
                        timespeed = 0;
                    }
                    timespeed = time;
                }
                if (timespeed == 0) {
                    timespeed = time;
                }
            } else {
                timespeed = 0;
            }
        }


        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */


                    if (e.jaxis.axis == 0) {
                        if (state == STATE_GAMES) {
                            if (e.jaxis.value > 3200) {
                                motionStart = time;
                                motionDir = 0;
                                nextGame();

                            } else if (e.jaxis.value < -3200) {
                                motionStart = time;
                                motionDir = 1;
                                prevGame();
                            } else {
                                motionStart = 0;
                            }
                        }
                    }
                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > 3200) {
                            if (state != STATE_SET) {
                                Mix_PlayChannel(-1, gui->home_down, 0);
                                settingsBack->animEndTime = time + 100;
                                settingsBack->nextLen = 280;
                                playButton->visible = false;
                                playText->visible = false;
                                meta->animEndTime = time + 200;
                                meta->nextPos = 215;
                                meta->prevPos = meta->y;
                                state = STATE_SET;
                                motionStart = 0;
                            }

                        } else if (e.jaxis.value < -3200) {
                            if (state != STATE_GAMES) {
                                Mix_PlayChannel(-1, gui->home_up, 0);
                                settingsBack->animEndTime = time + 100;
                                settingsBack->nextLen = 100;
                                playButton->visible = true;
                                playText->visible = true;
                                meta->animEndTime = time + 200;
                                meta->nextPos = 285;
                                meta->prevPos = meta->y;
                                state = STATE_GAMES;
                                motionStart = 0;
                            }
                        } else {

                        }
                    }
                    break;
                case SDL_JOYBUTTONUP:


                    if (e.jbutton.button == PCS_BTN_CIRCLE) {
                        if (state != STATE_GAMES) {
                            Mix_PlayChannel(-1, gui->home_up, 0);
                            settingsBack->animEndTime = time + 100;
                            settingsBack->nextLen = 100;
                            playButton->visible = true;
                            playText->visible = true;
                            meta->animEndTime = time + 200;
                            meta->nextPos = 285;
                            meta->prevPos = meta->y;
                            state = STATE_GAMES;
                        } else {
                            menuVisible = false;
                        }


                    };


                    if (e.jbutton.button == PCS_BTN_CROSS) {
                        if (state == STATE_GAMES) {
                            PcsxInterceptor *interceptor = new PcsxInterceptor();
                            interceptor->execute(gamesList[selGame]);
                            delete (interceptor);
                        }


                    };
                    if (e.jbutton.button == PCS_BTN_SQUARE) {



                    };


            }

        }
    }
    freeAssets();

}