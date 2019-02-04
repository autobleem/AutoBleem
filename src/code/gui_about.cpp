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
#include "lang.h"

void GuiAbout::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine(_("-=About=-"),0,offset,true);
    gui->renderTextLine(_("AutoBleem")+" "+gui->cfg.inifile.values["version"]+_(" - Automatic PlayStation Classic USB launcher"),1,offset);
    gui->renderTextLine(_("Code: AutoBleem Team (screemer, mGGk, nex, ThaFridge) "),2,offset);
    gui->renderTextLine(_("'Tenchu' theme by Kevzombie"),3,offset);

    gui->renderTextLine(_("Support via Discord: https://discord.gg/AHUS3RM"),5,offset);
    gui->renderTextLine(_("This is free and open source software. It works AS IS and We take no reponsibility for any issues or damage."),6,offset);

    gui->renderTextLine(_("Download latest: https://github.com/screemerpl/cbleemsync"),8,offset);
    gui->renderStatus("|@O| "+_("Go back")+"|");
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