//
// Created by screemer on 2019-04-15.
//

#include "gui_padconfig.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"
#include "../lang.h"
#include "../environment.h"
#include "../gui/gui_confirm.h"
#include <iostream>

using namespace std;

//*******************************
// GuiPadConfig::render
//*******************************
void GuiPadConfig::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    SDL_Joystick *joy = SDL_JoystickFromInstanceID(joyid);

    gui->renderTextLine("-=" + _("New GamePad found") + "=-", 0, offset, true);
    gui->renderTextLine(SDL_JoystickName(joy), 1, offset, true);
    newConfig.section = SDL_JoystickName(joy);

    /*
    defaultConfig->values["dpad"]    ="analogue";
    defaultConfig->values["analogue1"]    ="na";
    defaultConfig->values["analogue2"]    ="na";
    defaultConfig->values["dpaddeadzone"]    ="32000";
    */

    gui->renderTextLine(_("Press a key for") + "  |@X| " + newConfig.values["cross"], 3, offset, true);
    if (step > 0)
        gui->renderTextLine(_("Press a key for") + "  |@O| " + newConfig.values["circle"], 4, offset, true);
    if (step > 1)
        gui->renderTextLine(_("Press a key for") + "  |@S| " + newConfig.values["square"], 5, offset, true);
    if (step > 2)
        gui->renderTextLine(_("Press a key for") + "  |@T| " + newConfig.values["triangle"], 6, offset, true);
    if (step > 3)
        gui->renderTextLine(_("Press a key for") + "  |@Start| " + newConfig.values["start"], 7, offset, true);
    if (step > 4)
        gui->renderTextLine(_("Press a key for") + "  |@Select| " + newConfig.values["select"], 8, offset, true);
    if (step > 5)
        gui->renderTextLine(_("Press a key for") + "  |@L1| " + newConfig.values["l1"], 9, offset, true);
    if (step > 6)
        gui->renderTextLine(_("Press a key for") + "  |@L2| " + newConfig.values["l2"], 10, offset, true);
    if (step > 7)
        gui->renderTextLine(_("Press a key for") + "  |@R1| " + newConfig.values["r1"], 11, offset, true);
    if (step > 8)
        gui->renderTextLine(_("Press a key for") + "  |@R2| " + newConfig.values["r2"], 12, offset, true);
    if (step > 9)
        gui->renderTextLine(_("Press any DPAD button (to determine if it's digital or analogue)") + ": " + newConfig.values["dpad"], 13, offset, true);
    if (step > 10)
        gui->renderTextLine(_("Press |@X| to save controller mapping, or |@O| to retry configuration"), 14, offset, true);

    SDL_RenderPresent(renderer);
}

//*******************************
// GuiPadConfig::loop
//*******************************
void GuiPadConfig::loop() {
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

            switch (e.type) {
                case SDL_JOYHATMOTION: {
                    if (step == 10) {
                        // digital
                        cout <<  "dpad is digital" << endl;
                        newConfig.values["dpad"] = "digital";
                        newConfig.values["analogue1"] = "na";
                        newConfig.values["analogue2"] = "na";
                        newConfig.values["dpaddeadzone"] = "32000";
                        step++;
                        render();
                    }
                    break;
                }
                case SDL_JOYAXISMOTION: {
                    if (step == 10) {
                        // analogue
                        newConfig.values["dpad"] = "analogue";
                        newConfig.values["analogue1"] = "1";
                        newConfig.values["analogue2"] = "1";
                        newConfig.values["dpaddeadzone"] = "32000";
                        step++;
                        render();
                    }
                    break;
                }
                case SDL_JOYBUTTONUP: {
                    if (step == 0) newConfig.values["cross"] = to_string(e.jbutton.button);
                    if (step == 1) newConfig.values["circle"] = to_string(e.jbutton.button);
                    if (step == 2) newConfig.values["square"] = to_string(e.jbutton.button);
                    if (step == 3) newConfig.values["triangle"] = to_string(e.jbutton.button);
                    if (step == 4) newConfig.values["start"] = to_string(e.jbutton.button);
                    if (step == 5) newConfig.values["select"] = to_string(e.jbutton.button);

                    if (step == 6) newConfig.values["l1"] = to_string(e.jbutton.button);
                    if (step == 7) newConfig.values["l2"] = to_string(e.jbutton.button);
                    if (step == 8) newConfig.values["r1"] = to_string(e.jbutton.button);
                    if (step == 9) newConfig.values["r2"] = to_string(e.jbutton.button);

                    // for step 10 see the two case statements above

                    if (step < 11) {
                        step++;
                    } else {
                        if (to_string(e.jbutton.button) == newConfig.values["cross"]) {
                            string name = newConfig.section;
                            name.erase(std::remove(name.begin(), name.end(), '/'), name.end());
                            name.erase(std::remove(name.begin(), name.end(), '\\'), name.end());
                            newConfig.path = Env::getWorkingPath() + sep + "gpmapping/" + name + ".ini";
                            newConfig.save(newConfig.path);
                            gui->mapper.reload();
                            menuVisible == false;
                            return;
                        } else if (to_string(e.jbutton.button) == newConfig.values["circle"]) {
                            step = 0;
                            newConfig.values.clear();
                        } else {
                            continue;    // again until cross or circle pressed
                        }
                    }
                    render();
                    break;
                } // case SDL_JOYBUTTONUP
            } // switch
        } // if (SDL_PollEvent(&e))
    } // while
}
