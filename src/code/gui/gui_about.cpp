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
#include "../lang.h"
#include "../engine/scanner.h"

//*******************************
// GuiAbout::render
//*******************************
void GuiAbout::render() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine(_("-=About=-"), 0, offset, true);
    gui->renderTextLine(_("AutoBleem") + " " + gui->cfg.inifile.values["version"] +
                        _(" - Automatic PlayStation Classic USB launcher"), 1, offset);
    gui->renderTextLine(_("Code: AutoBleem Team") + " (screemer, mGGk, nex, ThaFridge, Axanar) ", 2, offset);
    gui->renderTextLine(_("Graphics by: ")+"Kevzombie" , 3, offset);

    gui->renderTextLine("Melancholia (Goth/Emo Type Beat) by | e s c p | & YellowTree | https://escp-music.bandcamp.com",4,offset);

    gui->renderTextLine(_("Support via Discord:") + " https://discord.gg/AHUS3RM", 5, offset);
    gui->renderTextLine(
            _("This is free and open source software. It works AS IS and We take no reponsibility for any issues or damage."),
            6, offset);

    gui->renderTextLine(_("Download latest:") + " https://github.com/screemerpl/cbleemsync", 8, offset);
    gui->renderStatus("|@O| " + _("Go back") + "|");
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiAbout::loop
//*******************************
void GuiAbout::loop() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    bool menuVisible = true;
    while (menuVisible) {
        gui->watchJoystickPort();
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();

                }
            }
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYBUTTONUP:


                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        menuVisible = false;

                    };


            }

        }
    }
}
