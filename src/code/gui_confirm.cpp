//
// Created by screemer on 2019-01-24.
//

#include "gui_confirm.h"
#include "gui_about.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"

void GuiConfirm::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=Please confirm=-",0,offset, true);
    gui->renderTextLine(label,2,offset, true);


    gui->renderStatus("Press |@X| Confirm  |@O| to Cancel |");
    SDL_RenderPresent(renderer);
}

void GuiConfirm::loop()
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
                case SDL_JOYBUTTONDOWN:


                    if (e.jbutton.button == PCS_BTN_CROSS) {
                        result = true;
                        menuVisible = false;

                    };
                    if (e.jbutton.button == PCS_BTN_CIRCLE) {
                        result = false;
                        menuVisible = false;

                    };


            }

        }
    }
}