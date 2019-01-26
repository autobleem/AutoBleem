//
// Created by screemer on 2018-12-19.
//

#include "gui.h"
#include "util.h"
#include "gui_about.h"
#include "gui_splash.h"
#include "gui_options.h"
#include "gui_memcards.h"
#include "gui_manager.h"


void Gui::logText(string message) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->drawText(message);
}


extern "C"
{
void logText(char *message) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->drawText(message);
}
}


void Gui::getTextAndRect(SDL_Renderer *renderer, int x, int y, const char *text, TTF_Font *font,
                         SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    if (strlen(text) == 0) {
        *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);
        rect->x = 0;
        rect->y = 0;
        rect->h = 0;
        rect->w = 0;
        return;
    }

    surface = TTF_RenderText_Blended(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

void Gui::renderBackground() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundImg, NULL, &backgroundRect);
}

int Gui::renderLogo(bool small) {
    if (!small) {
        SDL_RenderCopy(renderer, logo, NULL, &logoRect);
        return 0;
    } else {
        SDL_Rect rect;
        rect.x = atoi(themeData.values["opscreenx"].c_str());
        rect.y = atoi(themeData.values["opscreeny"].c_str());
        rect.w = logoRect.w / 3;
        rect.h = logoRect.h / 3;
        SDL_RenderCopy(renderer, logo, NULL, &rect);
        return rect.y + rect.h;
    }
}


void Gui::loadAssets() {
    themePath = Util::getWorkingPath() + Util::separator() + "theme" + Util::separator() + cfg.inifile.values["theme"] +
                Util::separator();
    themeData.load(themePath + "theme.ini");

    if (backgroundImg != NULL) {
        SDL_DestroyTexture(backgroundImg);
        SDL_DestroyTexture(logo);
        SDL_DestroyTexture(buttonT);
        SDL_DestroyTexture(buttonO);
        SDL_DestroyTexture(buttonX);
        SDL_DestroyTexture(buttonS);
        SDL_DestroyTexture(buttonStart);
        SDL_DestroyTexture(buttonSelect);
        SDL_DestroyTexture(buttonR1);
        TTF_CloseFont(font);
        backgroundImg = NULL;
    }

    logoRect.x = atoi(themeData.values["lpositionx"].c_str());
    logoRect.y = atoi(themeData.values["lpositiony"].c_str());
    logoRect.w = atoi(themeData.values["lw"].c_str());
    logoRect.h = atoi(themeData.values["lh"].c_str());

    backgroundImg = IMG_LoadTexture(renderer, (themePath + themeData.values["background"]).c_str());
    logo = IMG_LoadTexture(renderer, (themePath + themeData.values["logo"]).c_str());

    buttonO = IMG_LoadTexture(renderer, (themePath + themeData.values["circle"]).c_str());
    buttonX = IMG_LoadTexture(renderer, (themePath + themeData.values["cross"]).c_str());
    buttonT = IMG_LoadTexture(renderer, (themePath + themeData.values["triangle"]).c_str());
    buttonS = IMG_LoadTexture(renderer, (themePath + themeData.values["square"]).c_str());
    buttonSelect = IMG_LoadTexture(renderer, (themePath + themeData.values["select"]).c_str());
    buttonStart = IMG_LoadTexture(renderer, (themePath + themeData.values["start"]).c_str());
    buttonR1 = IMG_LoadTexture(renderer, (themePath + themeData.values["r1"]).c_str());

    string fontPath = (themePath + themeData.values["font"]).c_str();
    font = TTF_OpenFont(fontPath.c_str(), atoi(themeData.values["fsize"].c_str()));


    if (music != NULL) {
        Mix_HaltMusic();
    }
    if (cfg.inifile.values["nomusic"] != "true")
        if (themeData.values["loop"] != "-1") {
            if (Mix_OpenAudio(32000, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
                printf("Unable to open audio: %s\n", Mix_GetError());
            }

            music = Mix_LoadMUS((themePath + themeData.values["music"]).c_str());
            if (music == NULL) { printf("Unable to load Wav file: %s\n", Mix_GetError()); }
            if (Mix_PlayMusic(music, themeData.values["loop"] == "1" ? -1 : 0) == -1) {
                printf("Unable to play music file: %s\n", Mix_GetError());
            }
        }

}


void Gui::display(bool forceScan, string path) {
    this->path = path;
    this->forceScan = forceScan;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
    Mix_Init(0);
    TTF_Init();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    SDL_Window *window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                          0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    loadAssets();


    GuiSplash *splashScreen = new GuiSplash(renderer);
    splashScreen->show();

    delete splashScreen;


}

void Gui::saveSelection() {
    ofstream os;
    string path = cfg.inifile.values["cfg"];
    os.open(path);
    os << "#!/bin/sh" << endl << endl;
    os << "AB_SELECTION=" << menuOption << endl;
    os << "AB_THEME=" << cfg.inifile.values["stheme"] << endl;
    os << "AB_PCSX=" << cfg.inifile.values["pcsx"] << endl;
    os << "AB_MIP=" << cfg.inifile.values["mip"] << endl;

    os.flush();
    os.close();
}


bool otherMenuShift = false;

void Gui::menuSelection() {


    otherMenuShift = false;
    string retroarch = cfg.inifile.values["retroarch"];
    string adv = cfg.inifile.values["adv"];
    string mainMenu = "|@Start| AutoBleem    |@X|  Re/Scan   |@O|  Original  ";

    if (retroarch == "true") {
        mainMenu += "|@S|  RetroArch   ";
    }
    mainMenu += "|@T|  About   |@Select|  Options ";
    if (adv == "true") {
        mainMenu += "|@R1| Advanced";
    }

    string forceScanMenu = "Games changed. Press  |@X|  to scan|";
    string otherMenu = "|@X|  Memory Cards   |@O|  Game Manager |";
    cout << SDL_NumJoysticks() << "joysticks were found." << endl;
    SDL_Joystick *joystick;
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        joystick = SDL_JoystickOpen(i);
        cout << "--" << SDL_JoystickName(joystick) << endl;
    }


    if (!forceScan) {
        drawText(mainMenu);

    } else {
        drawText(forceScanMenu);

    }
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
                    if (adv != "false") {
                        if (!forceScan) {
                            if (e.jbutton.button == PCS_BTN_R1) {
                                drawText(mainMenu);
                                otherMenuShift = false;
                            }
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:

                    if (adv != "false") {
                        if (!forceScan) {
                            if (e.jbutton.button == PCS_BTN_R1) {
                                drawText(otherMenu);
                                otherMenuShift = true;
                            }
                        }
                    }

                    if (!otherMenuShift) {
                        if (!forceScan)
                            if (e.jbutton.button == PCS_BTN_START) {
                                this->menuOption = MENU_OPTION_RUN;

                                menuVisible = false;

                            };

                        if (!forceScan)
                            if (retroarch != "false") {
                                if (e.jbutton.button == PCS_BTN_SQUARE) {
                                    this->menuOption = MENU_OPTION_RETRO;

                                    menuVisible = false;

                                };
                            }

                        if (e.jbutton.button == PCS_BTN_CROSS) {
                            this->menuOption = MENU_OPTION_SCAN;

                            menuVisible = false;
                        };
                        if (e.jbutton.button == PCS_BTN_TRIANGLE) {
                            GuiAbout *aboutScreen = new GuiAbout(renderer);
                            aboutScreen->show();
                            delete aboutScreen;

                            menuSelection();
                            menuVisible = false;
                        };
                        if (e.jbutton.button == PCS_BTN_SELECT) {
                            GuiOptions *options = new GuiOptions(renderer);
                            options->show();
                            delete options;
                            menuSelection();
                            menuVisible = false;
                        };
                        if (!forceScan)
                            if (e.jbutton.button == PCS_BTN_CIRCLE) {
                                this->menuOption = MENU_OPTION_SONY;
                                menuVisible = false;


                            };
                        break;
                    } else {
                        if (e.jbutton.button == PCS_BTN_CROSS) {
                            GuiMemcards *memcardsScreen = new GuiMemcards(renderer);
                            memcardsScreen->show();
                            delete memcardsScreen;

                            menuSelection();
                            menuVisible = false;
                        };

                        if (e.jbutton.button == PCS_BTN_CIRCLE) {
                            GuiManager *managerScreen = new GuiManager(renderer);
                            managerScreen->show();
                            delete managerScreen;

                            menuSelection();
                            menuVisible = false;
                        };
                    }
            }

        }
    }

}

void Gui::finish() {

    SDL_DestroyTexture(backgroundImg);
    SDL_DestroyTexture(logo);
    SDL_DestroyTexture(buttonT);
    SDL_DestroyTexture(buttonO);
    SDL_DestroyTexture(buttonX);
    SDL_DestroyTexture(buttonS);
    SDL_DestroyTexture(buttonStart);
    SDL_DestroyTexture(buttonSelect);
    SDL_DestroyTexture(buttonR1);
    SDL_DestroyRenderer(renderer);

}


void Gui::getEmojiTextTexture(SDL_Renderer *renderer, string text, TTF_Font *font, SDL_Texture **texture,
                              SDL_Rect *rect) {
    if (text.empty()) text = " ";
    if (text.back() != '|') {
        text = text + "|";
    }

    vector<SDL_Texture *> textTexures;
    vector<string> textParts;
    std::string delimiter = "|";

    size_t pos = 0;
    std::string token;
    while ((pos = text.find(delimiter)) != std::string::npos) {
        token = text.substr(0, pos);
        if (token.size() > 0)
            textParts.push_back(token);
        text.erase(0, pos + delimiter.length());
    }

    for (string str:textParts) {
        if (str.empty()) continue;
        if (str[0] == '@') {
            string icon = str.substr(1);
            if (icon == "Start") {
                textTexures.push_back(buttonStart);
            }
            if (icon == "S") {
                textTexures.push_back(buttonS);
            }
            if (icon == "O") {
                textTexures.push_back(buttonO);
            }
            if (icon == "Select") {
                textTexures.push_back(buttonSelect);
            }
            if (icon == "R1") {
                textTexures.push_back(buttonR1);
            }
            if (icon == "T") {
                textTexures.push_back(buttonT);
            }
            if (icon == "X") {
                textTexures.push_back(buttonX);
            }
        } else {
            SDL_Texture *textTex;
            SDL_Rect textRec;
            getTextAndRect(renderer, 0, atoi(themeData.values["ttop"].c_str()), str.c_str(), font, &textTex, &textRec);
            textTexures.push_back(textTex);
        }
    }

    int w = 0;
    int h = 0;

    for (SDL_Texture *tex:textTexures) {

        Uint32 format;
        int access;
        int tw, th;
        SDL_QueryTexture(tex, &format, &access, &tw, &th);

        w += tw;
        if (th > h) h = th;
    }


    *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, *texture);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int xpos = 0;
    for (SDL_Texture *tex:textTexures) {
        Uint32 format;
        int access;
        int tw, th;
        SDL_QueryTexture(tex, &format, &access, &tw, &th);


        SDL_Rect posRect;
        posRect.x = xpos;

        posRect.y = 0;

        if (th != h) {
            posRect.y = (h - th) / 2;
        }
        posRect.w = tw;
        posRect.h = th;
        xpos += tw;
        SDL_RenderCopy(renderer, tex, NULL, &posRect);
    }
    rect->w = w;
    rect->h = h;
    rect->x = 0;
    rect->y = 0;
    SDL_SetRenderTarget(renderer, NULL);

    for (SDL_Texture *tex:textTexures) {
        if ((tex != buttonSelect) && (tex != buttonS) && (tex != buttonStart) && (tex != buttonO) && (tex != buttonT) &&
            (tex != buttonR1) &&
            (tex != buttonX))

            SDL_DestroyTexture(tex);
    }
    textTexures.clear();
}

void Gui::renderStatus(string text) {


    SDL_Texture *textTex;
    SDL_Rect textRec;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(themeData.values["textx"].c_str());
    rect.y = atoi(themeData.values["texty"].c_str());
    rect.w = atoi(themeData.values["textw"].c_str());
    rect.h = atoi(themeData.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    getEmojiTextTexture(renderer, text, font, &textTex, &textRec);
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    textRec.y = atoi(themeData.values["ttop"].c_str());
    if (textRec.w > atoi(themeData.values["textw"].c_str())) textRec.w = atoi(themeData.values["textw"].c_str());
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
}

void Gui::drawText(string text) {
    renderBackground();
    renderLogo(false);
    renderStatus(text);
    SDL_RenderPresent(renderer);
}

void Gui::renderLabelBox(int line, int offset) {
    SDL_Texture *textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);

    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());


    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 5;
    rectSelection.y = offset + textRec.h * (line);
    rectSelection.w = rect2.w - 10;
    rectSelection.h = textRec.h;

    SDL_SetRenderDrawColor(renderer, 230, 230, 230, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &rectSelection);
}

void Gui::renderSelectionBox(int line, int offset) {
    SDL_Texture *textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);

    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());


    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 5;
    rectSelection.y = offset + textRec.h * (line);
    rectSelection.w = rect2.w - 10;
    rectSelection.h = textRec.h;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderDrawRect(renderer, &rectSelection);
}

void Gui::renderTextLine(string text, int line, int offset) {
    renderTextLine(text, line, offset, false);
}

void Gui::renderTextLine(string text, int line, int offset, bool center) {
    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());

    SDL_Texture *textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);

    getTextAndRect(renderer, rect2.x + 10, (textRec.h * line) + offset,
                   text.c_str(), font, &textTex, &textRec);

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (center) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }


    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
}

void Gui::renderTextChar(string text, int line, int offset, int posx) {
    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());

    SDL_Texture *textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);
    getTextAndRect(renderer, posx, (textRec.h * line) + offset,
                   text.c_str(), font, &textTex, &textRec);

    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_DestroyTexture(textTex);
}

void Gui::renderTextBar() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, OCD_ALPHA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());

    SDL_RenderFillRect(renderer, &rect2);

}