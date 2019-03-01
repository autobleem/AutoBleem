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

#include "../gui/gui.h"
#include "../lang.h"

vector<string> headers = {_("SETTINGS"), _("GUIDE"), _("MEMORY CARD"), _("RESUME")};
vector<string> texts = {_("Customize PlayStationClassic or AutoBleem settings"), _("Show authors information"),
                        _("Edit Memory Card information"), _("Resume game from saved state point")};

bool wayToSort(PsGame * i, PsGame * j ) {
    string name1 = i->title;
    string name2 = j->title;
    name1 = lcase(name1);
    name2 = lcase(name2);
    return name1 < name2;
}

// Text rendering routines - places text at x,y with selected color and font
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

// just update metadata section to be visible on the screen
void GuiLauncher::updateMeta() {
    if (gamesList.empty()) {
        gameName = "";
        meta->updateTexts(gameName, publisher, year, players);
        return;
    }
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

// load all assets needed by the screen
void GuiLauncher::loadAssets() {
    shared_ptr<Gui> gui(Gui::getInstance());

    for (int i=0;i<100;i++) {
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    }
    staticElements.clear();
    frontElemets.clear();
    gamesList.clear();
    gui->db->getGames(&gamesList);

    vector<PsGame*> internal;
    Database * internalDB = new Database();
#if defined(__x86_64__) || defined(_M_X64)
    internalDB->connect("internal.db");
#else
    internalDB->connect("/media/System/Databases/internal.db");
#endif
    internalDB->getInternalGames(&internal);
    internalDB->disconnect();
    delete internalDB;
    for (auto internalGame:internal)
    {
        gamesList.push_back(internalGame);
    }


    sort(gamesList.begin(), gamesList.end(), wayToSort);

    for (auto game:gamesList) {
        game->loadTex(renderer);
    }

    if (gamesList.size() > 0) {
        while (gamesList.size() < 13) {
            for (PsGame *game:gamesList) {
                gamesList.push_back(game->clone());
            }

        }
    }


    gameName = "";
    publisher = "";
    year = "";
    players = "";


    carouselPositions.initCoverPositions();
    if (gamesList.empty()) {
        selGame = -1;
    } else {
        selGame = 0;
        setInitialPositions(0);
    }

    if (gui->lastSelIndex != 0) {
        selGame = gui->lastSelIndex;
        setInitialPositions(selGame);
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
    playButton->visible = selGame != -1;
    staticElements.push_back(playButton);

    playText = new PsZoomBtn(renderer, "playText", gui->getSonyImagePath() + "/BMP_Text/Play_Text.png");
    playText->y = 428;
    playText->x = 640 - 262 / 2;
    playText->visible = selGame != -1;
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

    arrow = new PsMoveBtn(renderer, "arrow", gui->getSonyImagePath() + "/GR/arrow.png");
    arrow->x = 640 - 12;
    arrow->y = 360;
    arrow->originaly = arrow->y;
    arrow->visible = false;
    staticElements.push_back(arrow);

    xButton = new PsObj(renderer, "xbtn", gui->getSonyImagePath() + "/GR/X_Btn_ICN.png");
    xButton->x = 605;
    xButton->y = 640;
    xButton->visible = true;
    staticElements.push_back(xButton);


    oButton = new PsObj(renderer, "obtn", gui->getSonyImagePath() + "/GR/Circle_Btn_ICN.png");
    oButton->x = 725;
    oButton->y = 640;
    oButton->visible = true;
    staticElements.push_back(oButton);

    tButton = new PsObj(renderer, "tbtn", gui->getSonyImagePath() + "/GR/Tri_Btn_ICN.png");
    tButton->x = 870;
    tButton->y = 640;
    tButton->visible = true;
    staticElements.push_back(tButton);


    menu = new PsMenu(renderer, "menu", gui->getSonyImagePath());
    menu->loadAssets();


    menuHead = new PsCenterLabel(renderer, "header");
    menuHead->font = font30;
    menuHead->visible = false;
    menuHead->y = 545;
    menuText = new PsCenterLabel(renderer, "menuText");
    menuText->visible = false;
    menuText->font = font24;
    menuText->y = 585;

    menuHead->setText(headers[0], 255, 255, 255);
    menuText->setText(_("Customize PlayStationClassic or AutoBleem settings"), 255, 255, 255);

    staticElements.push_back(menuHead);
    staticElements.push_back(menuText);

    sselector = new PsStateSelector(renderer, "selector");
    sselector->font30 = font30;
    sselector->font24 = font24;
    sselector->visible = false;

    frontElemets.push_back(sselector);

    updateMeta();

    if (selGame >= 0) {
        menu->setResumePic(gamesList[selGame]->findResumePicture());
    }

}

// memory cleanup for assets disposal
void GuiLauncher::freeAssets() {
    for (auto obj:staticElements) {
        obj->destroy();
        delete obj;
    }

    for (auto obj:frontElemets) {
        obj->destroy();
        delete obj;
    }
    staticElements.clear();
    frontElemets.clear();
    TTF_CloseFont(font30);
    TTF_CloseFont(font24);
    TTF_CloseFont(font15);
    for (PsGame *game:gamesList) {
        game->freeTex();
        delete game;

    }
    gamesList.clear();
    menu->freeAssets();
}

// run when screen is loaded
void GuiLauncher::init() {
    loadAssets();
}

// start scroll animation to next game
void GuiLauncher::scrollLeft(int speed) {
    scrolling = true;
    long time = SDL_GetTicks();
    for (PsGame *game:gamesList) {

        if (game->visible) {
            int nextIndex = game->screenPointIndex;

            if (game->screenPointIndex != 0) {
                nextIndex = game->screenPointIndex - 1;
            } else {
                game->visible = false;

            }
            game->destination = carouselPositions.coverPositions[nextIndex];
            game->animationDuration = speed;
            game->animationStart = time;

            game->screenPointIndex = nextIndex;
            game->current = game->actual;
        }
    }
}

// start scroll animation to previous game
void GuiLauncher::scrollRight(int speed) {
    scrolling = true;
    long time = SDL_GetTicks();
    for (PsGame *game:gamesList) {
        if (game->visible) {
            int nextIndex = game->screenPointIndex;
            if (game->screenPointIndex != carouselPositions.coverPositions.size() - 1) {
                nextIndex = game->screenPointIndex + 1;
            } else {
                game->visible = false;

            }
            game->destination = carouselPositions.coverPositions[nextIndex];
            game->animationDuration = speed;
            game->animationStart = time;

            game->screenPointIndex = nextIndex;
            game->current = game->actual;
        }
    }
}

// update potentially visible covers to save the memory
void GuiLauncher::updateVisibility() {
    bool allAnimationFinished = true;
    for (PsGame *game:gamesList) {
        if ((game->animationStart != 0) && game->visible) {
            allAnimationFinished = false;
        }
    }


    if (allAnimationFinished && scrolling) {
        setInitialPositions(selGame);
        scrolling = false;
    }
}

// this method runs during the loop to update positions of the covers during animation
void GuiLauncher::updatePositions() {


    long currentTime = SDL_GetTicks();
    for (PsGame *game:gamesList) {
        if (game->visible) {
            if (game->animationStart != 0) {
                long position = currentTime - game->animationStart;
                float delta = position * 1.0f / game->animationDuration;
                game->actual.x = game->current.x + (game->destination.x - game->current.x) * delta;
                game->actual.y = game->current.y + (game->destination.y - game->current.y) * delta;
                game->actual.scale = game->current.scale + (game->destination.scale - game->current.scale) * delta;
                game->actual.shade = game->current.shade + (game->destination.shade - game->current.shade) * delta;

                if (delta > 1.0f) {
                    game->actual = game->destination;
                    game->current = game->destination;
                    game->animationStart = 0;
                }


            }

        }
    }
    updateVisibility();
}

// render method called every loop
void GuiLauncher::render() {
    if (sselector != nullptr) {
        sselector->frame = menu->savestate;
    }

    shared_ptr<Gui> gui(Gui::getInstance());
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);

    for (auto obj:staticElements) {

        obj->render();
    }
    // covers render

    if (!gamesList.empty()) {
        for (auto game:gamesList) {
            if (game->visible) {
                SDL_Texture *currentGameTex = game->coverPng;
                PsScreenpoint point = game->actual;

                SDL_Rect coverRect;
                coverRect.x = point.x;
                coverRect.y = point.y;
                coverRect.w = 226 * point.scale;
                coverRect.h = 226 * point.scale;


                SDL_Rect fullRect;
                fullRect.x = 0;
                fullRect.y = 0;
                fullRect.w = 226;
                fullRect.h = 226;
                SDL_SetTextureColorMod(currentGameTex, point.shade, point.shade, point.shade);
                SDL_RenderCopy(renderer, currentGameTex, &fullRect, &coverRect);
            }
        }


    }


    menu->render();


    renderText(638, 640, _("Enter"), 60, 60, 60, font24);
    renderText(760, 640, _("Cancel"), 60, 60, 60, font24);
    renderText(902, 640, _("Console Button Guide"), 60, 60, 60, font24);

    for (auto obj:frontElemets) {

        obj->render();
    }

    SDL_RenderPresent(renderer);
}

// handler of next game
void GuiLauncher::nextGame(int speed) {
    shared_ptr<Gui> gui(Gui::getInstance());
    Mix_PlayChannel(-1, gui->cursor, 0);
    scrollLeft(speed);
    selGame++;
    if (selGame >= gamesList.size()) {
        selGame = 0;
    }
    updateMeta();
    menu->setResumePic(gamesList[selGame]->findResumePicture());
}

// handler of prev game
void GuiLauncher::prevGame(int speed) {
    shared_ptr<Gui> gui(Gui::getInstance());
    Mix_PlayChannel(-1, gui->cursor, 0);
    scrollRight(speed);
    selGame--;
    if (selGame < 0) {
        selGame = gamesList.size() - 1;
    }
    updateMeta();
    menu->setResumePic(gamesList[selGame]->findResumePicture());
}

// just small method to get next / prev game
int GuiLauncher::getNextId(int id) {
    int next = id + 1;
    if (next >= gamesList.size()) {
        return 0;
    }
    return next;
}

int GuiLauncher::getPreviousId(int id) {
    int prev = id - 1;
    if (prev < 0) {
        return gamesList.size() - 1;
    }
    return prev;
}


// initialize a table with positions for covers
void GuiLauncher::setInitialPositions(int selected) {
    for (PsGame *game:gamesList) {
        game->visible = false;
    }

    gamesList[selected]->visible = true;
    gamesList[selected]->current = this->carouselPositions.coverPositions[6];
    gamesList[selected]->screenPointIndex = 6;

    int prev = getPreviousId(selected);
    if (!gamesList[prev]->visible) {
        gamesList[prev]->current = this->carouselPositions.coverPositions[5];
        gamesList[prev]->visible = true;
        gamesList[prev]->screenPointIndex = 5;
    }
    prev = getPreviousId(prev);
    if (!gamesList[prev]->visible) {
        gamesList[prev]->current = this->carouselPositions.coverPositions[4];
        gamesList[prev]->visible = true;
        gamesList[prev]->screenPointIndex = 4;
    }
    prev = getPreviousId(prev);
    if (!gamesList[prev]->visible) {
        gamesList[prev]->current = this->carouselPositions.coverPositions[3];
        gamesList[prev]->visible = true;
        gamesList[prev]->screenPointIndex = 3;
    }
    prev = getPreviousId(prev);
    if (!gamesList[prev]->visible) {
        gamesList[prev]->current = this->carouselPositions.coverPositions[2];
        gamesList[prev]->visible = true;
        gamesList[prev]->screenPointIndex = 2;
    }
    prev = getPreviousId(prev);
    if (!gamesList[prev]->visible) {
        gamesList[prev]->current = this->carouselPositions.coverPositions[1];
        gamesList[prev]->visible = true;
        gamesList[prev]->screenPointIndex = 1;
    }
    prev = getPreviousId(prev);
    if (!gamesList[prev]->visible) {
        gamesList[prev]->current = this->carouselPositions.coverPositions[0];
        gamesList[prev]->visible = true;
        gamesList[prev]->screenPointIndex = 0;
    }

    int next = getNextId(selected);
    if (!gamesList[next]->visible) {
        gamesList[next]->current = this->carouselPositions.coverPositions[7];
        gamesList[next]->visible = true;
        gamesList[next]->screenPointIndex = 7;
    }
    next = getNextId(next);
    if (!gamesList[next]->visible) {
        gamesList[next]->current = this->carouselPositions.coverPositions[8];
        gamesList[next]->visible = true;
        gamesList[next]->screenPointIndex = 8;
    }
    next = getNextId(next);
    if (!gamesList[next]->visible) {
        gamesList[next]->current = this->carouselPositions.coverPositions[9];
        gamesList[next]->visible = true;
        gamesList[next]->screenPointIndex = 9;
    }
    next = getNextId(next);
    if (!gamesList[next]->visible) {
        gamesList[next]->current = this->carouselPositions.coverPositions[10];
        gamesList[next]->visible = true;
        gamesList[next]->screenPointIndex = 10;
    }
    next = getNextId(next);
    if (!gamesList[next]->visible) {
        gamesList[next]->current = this->carouselPositions.coverPositions[11];
        gamesList[next]->visible = true;
        gamesList[next]->screenPointIndex = 11;
    }
    next = getNextId(next);
    if (!gamesList[next]->visible) {
        gamesList[next]->current = this->carouselPositions.coverPositions[12];
        gamesList[next]->visible = true;
        gamesList[next]->screenPointIndex = 12;
    }

    for (PsGame *game:gamesList) {
        game->actual = game->current;
        game->destination = game->current;

    }


}

void GuiLauncher::moveMainCover(int state) {
    if (selGame == -1) {
        return;
    }
    PsScreenpoint point1;
    point1.x = 640 - 113;
    point1.y = 180;
    point1.scale = 1;
    point1.shade = 255;

    PsScreenpoint point2;
    point2.x = 640 - 113;
    point2.y = 90;
    point2.scale = 1;
    point2.shade = 220;

    long time = SDL_GetTicks();

    if (state == STATE_GAMES) {
        gamesList[selGame]->destination = point1;
        gamesList[selGame]->animationStart = time;
        gamesList[selGame]->animationDuration = 200;
    } else {
        gamesList[selGame]->destination = point2;
        gamesList[selGame]->animationStart = time;
        gamesList[selGame]->animationDuration = 200;


    }
}

void GuiLauncher::switchState(int state, int time) {
    shared_ptr<Gui> gui(Gui::getInstance());
    if (state == STATE_GAMES) {
        Mix_PlayChannel(-1, gui->home_up, 0);
        settingsBack->animEndTime = time + 100;
        settingsBack->nextLen = 100;
        playButton->visible = true;
        playText->visible = true;
        meta->animEndTime = time + 200;
        meta->nextPos = 285;
        meta->prevPos = meta->y;
        this->state = STATE_GAMES;
        arrow->visible = false;
        arrow->animationStarted = time;
        menu->duration = 200;
        menu->targety = 520;
        menu->animationStarted = time;
        menu->active = false;
        menuHead->visible = false;
        menuText->visible = false;

        moveMainCover(state);
    } else {
        Mix_PlayChannel(-1, gui->home_down, 0);
        settingsBack->animEndTime = time + 100;
        settingsBack->nextLen = 280;
        playButton->visible = false;
        playText->visible = false;
        meta->animEndTime = time + 200;
        meta->nextPos = 215;
        meta->prevPos = meta->y;
        this->state = STATE_SET;
        arrow->visible = true;
        arrow->animationStarted = time;
        menu->duration = 200;
        menu->targety = 440;
        menu->animationStarted = time;
        menu->active = true;
        menuHead->visible = true;
        menuText->visible = true;
        moveMainCover(state);
    }
}

// event loop
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

        menu->update(time);
        updatePositions();
        render();

        if (motionStart != 0) {
            long timePressed = time - motionStart;
            if (timePressed > 300) {
                if (time - timespeed > 100) {
                    if (motionDir == 0) {
                        if (!scrolling) {
                            nextGame(60);
                        }
                    } else {
                        if (!scrolling) {
                            prevGame(60);
                        }
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
                                if (!scrolling) {
                                    motionStart = time;
                                    motionDir = 0;
                                    scrolling = true;
                                    nextGame(110);
                                }

                            } else if (e.jaxis.value < -3200) {
                                if (!scrolling) {
                                    motionStart = time;
                                    motionDir = 1;
                                    scrolling = true;
                                    prevGame(110);
                                }
                            } else {
                                motionStart = 0;
                            }

                        } else if (state == STATE_SET) {
                            if (e.jaxis.value > 3200) {

                                if (menu->selOption != 3) {
                                    if (menu->animationStarted == 0) {
                                        Mix_PlayChannel(-1, gui->cursor, 0);
                                        menu->transition = TR_OPTION;
                                        menu->direction = 1;
                                        menu->duration = 100;
                                        menuHead->setText(headers[menu->selOption + 1], 255, 255, 255);
                                        menuText->setText(texts[menu->selOption + 1], 255, 255, 255);
                                        menu->animationStarted = time;
                                    }

                                }

                            } else if (e.jaxis.value < -3200) {
                                if (menu->selOption != 0) {
                                    if (menu->animationStarted == 0) {
                                        Mix_PlayChannel(-1, gui->cursor, 0);
                                        menu->transition = TR_OPTION;
                                        menu->direction = 0;
                                        menu->duration = 100;
                                        menuHead->setText(headers[menu->selOption - 1], 255, 255, 255);
                                        menuText->setText(texts[menu->selOption - 1], 255, 255, 255);
                                        menu->animationStarted = time;
                                    }
                                }

                            } else {

                            }
                        } else if (state == STATE_RESUME) {
                            if ((e.jaxis.value > 3200) && (sselector->selSlot != 3)) {
                                Mix_PlayChannel(-1, gui->cursor, 0);
                                sselector->selSlot++;
                            } else if (e.jaxis.value < -3200 && (sselector->selSlot != 0)) {
                                Mix_PlayChannel(-1, gui->cursor, 0);
                                sselector->selSlot--;
                            }
                        }
                    }
                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > 3200) {
                            if (scrolling) {
                                continue;
                            }
                            if (state == STATE_GAMES) {
                                if (menu->animationStarted == 0) {
                                    menu->transition = TR_MENUON;
                                    switchState(STATE_SET, time);
                                    motionStart = 0;
                                }
                            }

                        } else if (e.jaxis.value < -3200) {
                            if (scrolling) {
                                continue;
                            }
                            if (state == STATE_SET) {
                                if (menu->animationStarted == 0) {
                                    menu->transition = TR_MENUON;
                                    switchState(STATE_GAMES, time);
                                    motionStart = 0;
                                }
                            }
                        } else {

                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == PCS_BTN_CIRCLE) {
                        if (state == STATE_SET) {
                            if (menu->animationStarted == 0) {
                                menu->transition = TR_MENUON;
                                switchState(STATE_GAMES, time);
                                motionStart = 0;
                            }
                        } else if (state == STATE_GAMES) {
                            menuVisible = false;
                        } else if (state == STATE_RESUME) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            sselector->visible = false;
                            arrow->visible = true;
                            state = STATE_SET;
                        }


                    };


                    if (e.jbutton.button == PCS_BTN_CROSS) {
                        if (state == STATE_GAMES) {
                            gui->startingGame = true;
                            gui->runningGame = gamesList[selGame]->clone();
                            gui->lastSelIndex = selGame;
                            gui->resumepoint = -1;
                            menuVisible = false;
                        } else if (state == STATE_SET) {
                            if (menu->selOption == 3) {
                                bool resumeAvailable = false;
                                for (int i = 0; i < 4; i++) {
                                    if (gamesList[selGame]->isResumeSlotActive(i)) {
                                        resumeAvailable = true;
                                    }
                                }

                                if (resumeAvailable) {
                                    Mix_PlayChannel(-1, gui->cursor, 0);
                                    sselector->visible = true;
                                    sselector->loadSaveStateImages(gamesList[selGame],false);
                                    state = STATE_RESUME;
                                    sselector->selSlot = 0;
                                } else {
                                    Mix_PlayChannel(-1, gui->cancel, 0);
                                }
                            }
                        }

                    };
                    if (e.jbutton.button == PCS_BTN_SQUARE) {

                    };


            }

        }
    }
    freeAssets();

}
