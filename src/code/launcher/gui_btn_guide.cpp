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
#include "../engine/scanner.h"

using namespace std;

//*******************************
// GuiBtnGuide::render
//*******************************
void GuiBtnGuide::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();

    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=" + _("Button Guide") + "=-", 0, offset, POS_CENTER);
    string leftColumnSpaces(30, ' ');
    string spaceOverToColumn(20, ' ');

    auto renderTextLineToColumns = [&] (const string &textLeft, const string &textRight, int line, int offset) {
        gui->renderTextLine(leftColumnSpaces + spaceOverToColumn + textRight, line, offset, POS_LEFT);
        gui->renderTextLine(leftColumnSpaces + textLeft,                      line, offset, POS_LEFT);
    };

    int line = 1;
    renderTextLineToColumns(_("DPAD"),              _("Same as classic menu"), line++, offset);

    renderTextLineToColumns("|@X| / |@O|",          _("Select or cancel highlighted option"), line++, offset);
    renderTextLineToColumns("|@S|",                 _("Run using RetroBoot"), line++, offset);
    renderTextLineToColumns("|@R1| / |@L1|",        _("Quick scroll to next letter"), line++, offset);
    renderTextLineToColumns("|@Select|",            _("Games filter"), line++, offset);
    renderTextLineToColumns("|@L2| + |@Select|",    _("Change USB Games Sub-Directory"), line++, offset);
    renderTextLineToColumns("|@L2| + |@Select|",    _("Change RetroBoot System"), line++, offset);

    renderTextLineToColumns("",                     "-=" + _("In Game") + "=-", line++, offset);
    renderTextLineToColumns("|@Select| + |@T|",     _("Emulator config MENU"), line++, offset);
    renderTextLineToColumns("RESET",                _("Quit emulation - back to AutoBleem"), line++, offset);

    renderTextLineToColumns("",                     "-=" + _("In Retroarch Game") + "=-", line++, offset);
    renderTextLineToColumns("|@Select| + |@Start|", _("Open Retroarch Menu"), line++, offset);
    renderTextLineToColumns(_("POWER"),             _("Exit to EvoUI"), line++, offset);

//    renderTextLineToColumns("|@L2| + |@R2|",        _("IN BOOT MENU TO POWER OFF THE CONSOLE (SAFE POWER OFF !!!)"), 14, offset);
    renderTextLineToColumns("|@L2| + |@R2|",        "        " + _("In Boot Menu: Safe Power Off The Console"), 14, offset);

    gui->renderStatus("|@O| " + _("Go back") + "|");
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiBtnGuide::loop
//*******************************
void GuiBtnGuide::loop() {
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
                case SDL_JOYBUTTONUP:


                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        menuVisible = false;

                    };


            }

        }
    }
}
