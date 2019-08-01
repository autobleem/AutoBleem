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


void GuiAbout::init() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    fx.renderer = renderer;
    font = Fonts::openFont(DirEntry::getWorkingPath() + "/about.ttf", 17);
    logo = IMG_LoadTexture(renderer, (DirEntry::getWorkingPath() + "/ablogo.png").c_str());

}

//*******************************
// GuiAbout::render
//*******************************
void GuiAbout::render() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    vector<string> credits = {gui->cfg.inifile.values["version"], " ",
                              _(".-= Code C++ and shell scripts =-."),
                              "screemer, Axanar, mGGk, nex, genderbent",
                              _(".-= Graphics =-."),
                              "KaonashiFTW, GeekAndy, rubixcube6, NewbornfromHell",
                              _(".-= Testing =-."),
                              "MagnusRC, xboxiso, Azazel, Solidius, SupaSAIAN, Kingherb, saptis",
                              _(".-= Database maintenance =-."),
                              "Screemer,Kingherb",
                              _(".-= Localization support =-."),
                              "nex(German), Azazel(Polish), gadsby(Turkish), GeekAndy(Dutch), Pardubak(Slovak), SupaSAIAN(Spanish), Mate(Czech)",
                              "Sasha(Italian), Jakejj(BR_Portuguese), jolny(Swedish), StepJefli(Danish), alucard73 / MagnusRC(French), Quenti(Occitan), ",
                              _(".-= Retroboot and emulation cores =-."),
                              "genderbent, KMFDManic"," ",
                              _("Support via Discord:") + " https://discord.gg/AHUS3RM",
                              _("This is free and open source software. It works AS IS and We take no responsibility for any issues or damage."),
                              _("Download latest:") + " https://github.com/screemerpl/cbleemsync"
    };


    gui->renderBackground();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 235);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect rect2;
    rect2.x = 0;
    rect2.y = 0;
    rect2.w = 1280;
    rect2.h = 720;

    SDL_RenderFillRect(renderer, &rect2);
    fx.render();
    int offset = 150;
    SDL_Rect rect;
    rect.x = 1280/2-100;
    rect.y = 5;
    rect.w = 200;
    rect.h = 141;
    SDL_RenderCopy(renderer, logo, nullptr, &rect);


    int line = 1;
    for (const string &s:credits) {
        gui->renderTextLine(s, line, offset, POS_CENTER, 0, font);
        line++;
    }

    gui->renderStatus("|@O| " + _("Go back") + "|",680);
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiAbout::loop
//*******************************
void GuiAbout::loop() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    bool menuVisible = true;
    while (menuVisible) {
        render();
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


                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        menuVisible = false;

                    };


            }

        }
    }
}
