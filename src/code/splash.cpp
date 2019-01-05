//
// Created by screemer on 2018-12-19.
//

#include "splash.h"
#include "util.h"


void Splash::logText(string message) {
    shared_ptr<Splash> splash(Splash::getInstance());
    splash->drawText(message, false);
}


extern "C"
{
void logText(char *message) {
    shared_ptr<Splash> splash(Splash::getInstance());
    splash->drawText(message, false);
}
}

#ifndef NO_GUI

void Splash::getTextAndRect(SDL_Renderer *renderer, int x, int y, const char *text, TTF_Font *font,
                            SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

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

#endif

void Splash::loadAssets() {
    themePath = Util::getWorkingPath() + Util::separator() + "theme" + Util::separator() + cfg.inifile.values["theme"] +
                Util::separator();
    themeData.load(themePath + "theme.ini");
    img = IMG_LoadTexture(renderer, (themePath + themeData.values["background"]).c_str());
    logo = IMG_LoadTexture(renderer, (themePath + themeData.values["logo"]).c_str());

    buttonO = IMG_LoadTexture(renderer, (themePath + themeData.values["circle"]).c_str());
    buttonX = IMG_LoadTexture(renderer, (themePath + themeData.values["cross"]).c_str());
    buttonT = IMG_LoadTexture(renderer, (themePath + themeData.values["triangle"]).c_str());
    buttonS = IMG_LoadTexture(renderer, (themePath + themeData.values["square"]).c_str());
    buttonSelect = IMG_LoadTexture(renderer, (themePath + themeData.values["select"]).c_str());
    buttonStart = IMG_LoadTexture(renderer, (themePath + themeData.values["start"]).c_str());

}

void Splash::display(bool forceScan) {
    this->forceScan = forceScan;


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
    Mix_Init(0);
    TTF_Init();


    SDL_Window *window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                          0);
    int w, h; // texture width & height
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    loadAssets();

    SDL_SetTextureBlendMode(img, SDL_BLENDMODE_BLEND);
    SDL_QueryTexture(img, NULL, NULL, &w, &h);

    texr.x = 0;
    texr.y = 0;
    texr.w = w;
    texr.h = h;
    SDL_QueryTexture(logo, NULL, NULL, &w, &h);
    logor.x = atoi(themeData.values["lpositionx"].c_str());
    logor.y = atoi(themeData.values["lpositiony"].c_str());
    logor.w = atoi(themeData.values["lw"].c_str());
    logor.h = atoi(themeData.values["lh"].c_str());

    if (themeData.values["loop"] != "-1") {
        if (Mix_OpenAudio(32000, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
            printf("Unable to open audio: %s\n", Mix_GetError());
        }
        Mix_Music *music;
        music = Mix_LoadMUS((themePath + themeData.values["music"]).c_str());
        if (music == NULL) { printf("Unable to load Wav file: %s\n", Mix_GetError()); }
        if (Mix_PlayMusic(music, themeData.values["loop"] == "1" ? -1 : 0) == -1) {
            printf("Unable to play music file: %s\n", Mix_GetError());
        }
    }
    int start = SDL_GetTicks();
    string fontPath = (themePath + themeData.values["font"]).c_str();
    Sans = TTF_OpenFont(fontPath.c_str(), atoi(themeData.values["fsize"].c_str()));
    Mix_VolumeMusic(0);
    int alpha = 0;
    while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                break;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                break;
        }

        SDL_Texture *textTex;
        SDL_Rect textRec;
        getTextAndRect(renderer, 0, atoi(themeData.values["ttop"].c_str()),
                       ("AutoBleem " + cfg.inifile.values["version"]).c_str(), Sans, &textTex, &textRec);
        int screencenter = 1280 / 2;
        textRec.x = screencenter - (textRec.w / 2);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_SetTextureAlphaMod(img, alpha);
        SDL_SetTextureAlphaMod(logo, alpha);
        SDL_SetTextureAlphaMod(textTex, alpha);
        Mix_VolumeMusic(alpha / 3);
        int current = SDL_GetTicks();
        int time = current - start;
        if (time > 5) {
            if (alpha < 255) {
                alpha += 1;
            } else {

                break;
            }
            start = SDL_GetTicks();
        }
        SDL_RenderCopy(renderer, img, NULL, &texr);
        SDL_RenderCopy(renderer, logo, NULL, &logor);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect rect;
        rect.x = atoi(themeData.values["textx"].c_str());
        rect.y = atoi(themeData.values["texty"].c_str());
        rect.w = atoi(themeData.values["textw"].c_str());
        rect.h = atoi(themeData.values["texth"].c_str());
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderCopy(renderer, textTex, NULL, &textRec);
        SDL_RenderPresent(renderer);
    }
}

void Splash::saveSelection() {
    string path = "/media/lolhack/selection.txt";
    ofstream os;
    os.open(path);
    os << menuOption << endl;
    os.flush();
    os.close();
}

void Splash::menuSelection() {
#ifndef NO_GUI
    cout << SDL_NumJoysticks() << "joysticks were found." << endl;
    SDL_Joystick *joystick;
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        joystick = SDL_JoystickOpen(i);
        cout << "--" << SDL_JoystickName(joystick) << endl;
    }

    if (!forceScan) {
        drawText("-  AutoBleem Games   O  Re/Scan   O  Original Games   O  RetroArch   O  About   -  Options", true);

    } else {
        drawText("Games changed. Press  O  to scan", true);

    }
    bool menuVisible = true;
    while (menuVisible) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            // this is for pc Only
            if (e.type == SDL_QUIT)
                break;
            switch (e.type) {
                case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
                    //drawText("Button:"+to_string(e.jbutton.button));
                    if (!forceScan)
                        if (e.jbutton.button == 9) {
                            this->menuOption = MENU_OPTION_RUN;

                            menuVisible = false;

                        };

                    if (!forceScan)
                        if (e.jbutton.button == 3) {
                            this->menuOption = MENU_OPTION_RETRO;

                            menuVisible = false;

                        };
                    if (e.jbutton.button == 2) {
                        this->menuOption = MENU_OPTION_SCAN;

                        menuVisible = false;
                    };
                    if (!forceScan)
                        if (e.jbutton.button == 1) {
                            this->menuOption = MENU_OPTION_SONY;
                            menuVisible = false;


                        };
                    break;
            }

        }
    }


#endif
}

void Splash::finish() {
#ifndef NO_GUI
    SDL_DestroyTexture(img);
    SDL_DestroyTexture(logo);
    SDL_DestroyTexture(buttonT);
    SDL_DestroyTexture(buttonO);
    SDL_DestroyTexture(buttonX);
    SDL_DestroyTexture(buttonS);
    SDL_DestroyTexture(buttonStart);
    SDL_DestroyTexture(buttonSelect);
    SDL_DestroyRenderer(renderer);
#endif

}

void Splash::drawIcons(bool forceScan) {
    SDL_Rect iconRect;
    iconRect.w = atoi(themeData.values["iconw"].c_str());
    iconRect.h = atoi(themeData.values["iconh"].c_str());
    iconRect.y = atoi(themeData.values["texty"].c_str());
    if (!forceScan) {
        iconRect.x =  atoi(themeData.values["iconstart"].c_str());;
        SDL_RenderCopy(renderer, buttonStart, NULL, &iconRect);
        iconRect.x =  atoi(themeData.values["iconx"].c_str());
        SDL_RenderCopy(renderer, buttonX, NULL, &iconRect);
        iconRect.x = atoi(themeData.values["icono"].c_str());
        SDL_RenderCopy(renderer, buttonO, NULL, &iconRect);
        iconRect.x = atoi(themeData.values["icons"].c_str());
        SDL_RenderCopy(renderer, buttonS, NULL, &iconRect);
        iconRect.x = atoi(themeData.values["icont"].c_str());
        SDL_RenderCopy(renderer, buttonT, NULL, &iconRect);
        iconRect.x = atoi(themeData.values["iconselect"].c_str());
        SDL_RenderCopy(renderer, buttonSelect, NULL, &iconRect);
    } else
    {
        iconRect.x =  atoi(themeData.values["iconrescan"].c_str());
        SDL_RenderCopy(renderer, buttonX, NULL, &iconRect);
    }
}

void Splash::drawText(string text, bool showIcons) {
#ifndef NO_GUI

    SDL_Texture *textTex;
    SDL_Rect textRec;
    getTextAndRect(renderer, 0, atoi(themeData.values["ttop"].c_str()), text.c_str(), Sans, &textTex, &textRec);
    if (textRec.w > atoi(themeData.values["textw"].c_str())) textRec.w = atoi(themeData.values["textw"].c_str());
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, img, NULL, &texr);
    SDL_RenderCopy(renderer, logo, NULL, &logor);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(themeData.values["textx"].c_str());
    rect.y = atoi(themeData.values["texty"].c_str());
    rect.w = atoi(themeData.values["textw"].c_str());
    rect.h = atoi(themeData.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderCopy(renderer, textTex, NULL, &textRec);

    if (showIcons)
        drawIcons(forceScan);

    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(textTex);
#else
    cout <<"## UI: "<< text << endl;
#endif

}