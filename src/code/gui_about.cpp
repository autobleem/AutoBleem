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
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("AutoBleem - Automatic PlayStation Classic game scanner and Boot Menu",0,offset);
    gui->renderTextLine("2018-2019 code by ScreemerPL / testing nex and justAndy",1,offset);
    gui->renderTextLine("Support WWW: https://github.com/screemerpl/cbleemsync",3,offset);
    gui->renderTextLine("This is free and open source software. It works AS IS and I take no reponsibility for any issues.",4,offset);
    gui->renderTextLine("If you want to donate this project send me some small amount to PayPal: screemer1@o2.pl ",5,offset);
    gui->renderTextLine("If you want just to chat join ModMyClassic Discord channel and .. find 'screemer' ...",7,offset);
    gui->renderStatus("Press |@X| to go back|");
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