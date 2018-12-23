//
// Created by screemer on 2018-12-19.
//

#include "splash.h"
#include "util.h"


void Splash::logText(string message) {
    shared_ptr<Splash> splash(Splash::getInstance());
    splash->drawText(message);
}


extern "C"
{
void logText(char *message) {
    shared_ptr<Splash> splash(Splash::getInstance());
    splash->drawText(message);
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

void Splash::display(bool forceScan) {
    this->forceScan = forceScan;
#ifndef NO_GUI
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
    Mix_Init(0);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                          0);
    int w, h; // texture width & height
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    img = IMG_LoadTexture(renderer, "background.jpg");
    SDL_SetTextureBlendMode(img, SDL_BLENDMODE_BLEND);
    SDL_QueryTexture(img, NULL, NULL, &w, &h);

    texr.x = 0;
    texr.y = 0;
    texr.w = w;
    texr.h = h;

    if (Mix_OpenAudio(32000, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        printf("Unable to play Ogg file: %s\n", Mix_GetError());
    }
    Mix_Music *music;
    music = Mix_LoadMUS("3.wav");
    if (music == NULL) { printf("Unable to load Wav file: %s\n", Mix_GetError()); }
    if (Mix_PlayMusic(music, -1) == -1) { printf("Unable to play music file: %s\n", Mix_GetError()); }
    int start = SDL_GetTicks();
    string fontPath = "RobotoCondensed-Light.ttf";
    Sans = TTF_OpenFont(fontPath.c_str(), 24);
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
        getTextAndRect(renderer, 88, 552, "AutoBleem v0.2.1 (nex)", Sans, &textTex, &textRec);
        int screencenter = 1280 / 2;
        textRec.x = screencenter - (textRec.w / 2);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);


        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_SetTextureAlphaMod(img, alpha);
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
        SDL_RenderCopy(renderer, textTex, NULL, &textRec);
        SDL_RenderPresent(renderer);
    }
#endif
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
        drawText("MENU: Press 'Start' for AutoBleem   'X' - Re/Scan Games   'O' - Original Games");
    } else {
        drawText("MENU: This is a first run or games were changed - Press 'X' to scan");
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
    SDL_DestroyRenderer(renderer);
#endif

}


void Splash::drawText(string text) {
#ifndef NO_GUI

    SDL_Texture *textTex;
    SDL_Rect textRec;
    getTextAndRect(renderer, 88, 552, text.c_str(), Sans, &textTex, &textRec);
    if (textRec.w > 1140) textRec.w = 1140;
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, img, NULL, &texr);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(textTex);
#else
    cout <<"## UI: "<< text << endl;
#endif

}