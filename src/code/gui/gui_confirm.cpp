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
#include "../engine/scanner.h"
using namespace std;

//*******************************
// GuiConfirm::render
//*******************************
void GuiConfirm::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=" + _("Please confirm") + "=-",0,offset, POS_CENTER);
    gui->renderTextLine(label,2,offset, POS_CENTER);


    gui->renderStatus("|@X| "+_("Confirm")+"  |@O| "+_("Cancel")+" |");
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiConfirm::loop
//*******************************
void GuiConfirm::loop()
{
    shared_ptr<Gui> gui(Gui::getInstance());
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
                case SDL_JOYBUTTONDOWN:


                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        result = true;
                        menuVisible = false;

                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        result = false;
                        menuVisible = false;

                    };


            }

        }
    }
}