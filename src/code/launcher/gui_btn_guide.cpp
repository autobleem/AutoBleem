//
// Created by screemer on 2019-03-02.
//

#include "gui_btn_guide.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "../gui/gui.h"
#include "../lang.h"

void GuiBtnGuide::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine(_("-=Button Guide=-"), 0, offset, true);
    gui->renderTextLine(_("DPAD - Same as classic menu"), 1, offset,true);
    gui->renderTextLine("|@X| / |@O|   "+_("Select or cancel highlighted option"), 2, offset,true);
    gui->renderTextLine("|@R1| / |@L1|   "+_("Quick scroll to next letter"), 3, offset,true);

    gui->renderTextLine(_("-=In Game=-"),5,offset,true);
    gui->renderTextLine("|@Select| + |@T|   "+_("Emulator config MENU"), 6, offset,true);
    gui->renderTextLine("RESET   "+_("Quit emulation - back to AutoBleem"), 7, offset,true);

    gui->renderTextLine("|@R1| + |@L1|  "+_("IN BOOT MENU TO POWER OFF THE CONSOLE (SAFE POWER OFF !!!)"), 9, offset,true);


    gui->renderStatus("|@O| " + _("Go back") + "|");
    SDL_RenderPresent(renderer);
}

void GuiBtnGuide::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());
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
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        menuVisible = false;

                    };


            }

        }
    }
}
