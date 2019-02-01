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
    gui->renderTextLine("-=About=-",0,offset,true);
    gui->renderTextLine("AutoBleem "+gui->cfg.inifile.values["version"]+" - Automatic PlayStation Classic USB launcher",1,offset);
    gui->renderTextLine("Code: AutoBleem Team (screemer, mGGk, nex, ThaFridge) ",2,offset);
    gui->renderTextLine("'Tenchu' theme by Kevzombie ",3,offset);

    gui->renderTextLine("Support via Discord: https://discord.gg/AHUS3RM",5,offset);
    gui->renderTextLine("This is free and open source software. It works AS IS and We take no reponsibility for any issues or damage.",6,offset);

    gui->renderTextLine("Download latest: https://github.com/screemerpl/cbleemsync",8,offset);
    gui->renderStatus("|@O| Go back|");
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


                    if (e.jbutton.button == PCS_BTN_CIRCLE) {

                        menuVisible = false;

                    };


            }

        }
    }
}