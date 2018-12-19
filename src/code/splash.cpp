//
// Created by screemer on 2018-12-19.
//

#include "splash.h"
#include "util.h"


void logText(string message) {
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

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, const char *text,
                       TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
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

void Splash::display() {

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);
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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        printf("Unable to play Ogg file: %s\n", Mix_GetError());
    }
    Mix_Music *music;
    music = Mix_LoadMUS("music.wav");
    if (music == NULL) { printf("Unable to load Wav file: %s\n", Mix_GetError()); }
    if (Mix_PlayMusic(music, -1) == -1) { printf("Unable to play music file: %s\n", Mix_GetError()); }
    int start = SDL_GetTicks();
    string fontPath = "RobotoCondensed-Light.ttf";
    Sans = TTF_OpenFont(fontPath.c_str(), 24);

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
        get_text_and_rect(renderer, 88, 552, "AutoBleem v0.1.1b", Sans, &textTex, &textRec);
        int screencenter = 1280 / 2;
        textRec.x = screencenter - (textRec.w / 2);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);


        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_SetTextureAlphaMod(img, alpha);
        SDL_SetTextureAlphaMod(textTex, alpha);
        int current = SDL_GetTicks();
        int time = current - start;
        if (time > 2) {
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
}


void Splash::finish() {
    SDL_DestroyTexture(img);
    SDL_DestroyRenderer(renderer);


}


void Splash::drawText(string text) {


    SDL_Texture *textTex;
    SDL_Rect textRec;
    get_text_and_rect(renderer, 88, 552, text.c_str(), Sans, &textTex, &textRec);
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, img, NULL, &texr);
    SDL_RenderCopy(renderer, textTex, NULL, &textRec);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(textTex);

}