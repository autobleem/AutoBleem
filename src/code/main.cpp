/* 
 * File:   main.cpp
 * Author: screemer
 *
 * Created on 11 Dec 2018, 20:37
 */

#include <iostream>
#include "database.h"
#include "scanner.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

using namespace std;

int scanGames(int argc, char *argv[]) {

    if (argc < 3) {
        cout << "USAGE: bleemsync /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }


    Database *db = new Database();
    if (!db->connect(argv[1])) {
        delete db;
        return EXIT_FAILURE;
    }
    if (!db->createInitialDatabase()) {
        cout << "Error creating db structure" << endl;
        db->disconnect();
        delete db;
        return EXIT_FAILURE;
    };

    Scanner *scanner = new Scanner();
    scanner->scanDirectory(argv[2]);
    scanner->updateDB(db);
    delete scanner;


    db->disconnect();
    delete db;

    return (EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    bool quit = false;
    SDL_Event event;
    SDL_Renderer *renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        return -1;
    int mixinit = Mix_Init(MIX_INIT_OGG);

    cout << mixinit;
    printf("UProblem: %s\n", Mix_GetError());


    SDL_Window *window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                          SDL_WINDOW_FULLSCREEN);
    SDL_Texture *img = NULL;
    int w, h; // texture width & height
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    img = IMG_LoadTexture(renderer, "background.jpg");
    SDL_SetTextureBlendMode(img, SDL_BLENDMODE_BLEND);

    SDL_QueryTexture(img, NULL, NULL, &w, &h);
    SDL_Rect texr;
    texr.x = 0;
    texr.y = 0;
    texr.w = w;
    texr.h = h;

    int start = SDL_GetTicks();

    // Start the screen as Opaque(255)
    int alpha = 0;

    SDL_Surface *tempScreen = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_BLENDFACTOR_SRC_ALPHA, 1280, 720, 32, 0xff000000,
                                                   0x00ff0000, 0x0000ff00, 0x000000ff);

    // Convert it to the format of the screen


    // Free the created surface
    SDL_FreeSurface(tempScreen);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        printf("Unable to play Ogg file: %s\n", Mix_GetError());
    }
    Mix_Music *music;
    music = Mix_LoadMUS("music.ogg");
    if (music == NULL) { printf("Unable to load Ogg file: %s\n", Mix_GetError()); }

    if (Mix_PlayMusic(music, 0) == -1) { printf("Unable to play Ogg file: %s\n", Mix_GetError()); }

    while (!quit) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                break;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                break;
        }


        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

        // copy the texture to the rendering context

        SDL_SetTextureAlphaMod(img, alpha);

        int current = SDL_GetTicks();
        int time = current - start;
        if (time > 15) {
            if (alpha < 255) {
                alpha++;
            } else {
                break;
            }
            start = SDL_GetTicks();
        }


        SDL_RenderCopy(renderer, img, NULL, &texr);
        // flip the backbuffer
        // this means that everything that we prepared behind the screens is actually shown
        SDL_RenderPresent(renderer);
    }

    int result = scanGames(argc, argv);
    SDL_DestroyTexture(img);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();



    // experiment for scan
}


