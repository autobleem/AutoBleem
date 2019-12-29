
#include "gui_padTest.h"
#include <unistd.h>
#include <SDL2/SDL.h>
#include <string>
#include "gui.h"
#include "../lang.h"
#include <iostream>
#include <sstream>

using namespace std;

//*******************************
// GuiPadTest::init
//*******************************
void GuiPadTest::init() {
    GuiScrollWin::init();
    if (joyid != -1) {
        SDL_Joystick *joy = SDL_JoystickFromInstanceID(joyid);

        appendLine("-=" + _("New GamePad found") + "=-");
        appendLine(SDL_JoystickName(joy));
    }
    appendLine("Hold down three buttons to exit");
}

//*******************************
// GuiPadTest::loop
//*******************************
void GuiPadTest::loop() {
    // eat any events in the queue
    SDL_Event e;
    while (SDL_PollEvent(&e))
        ;

    shared_ptr<Gui> gui(Gui::getInstance());
    int buttonDownCount = 0;
    while (buttonDownCount >= 0 && buttonDownCount < 3) {
        gui->watchJoystickPort();
        SDL_Event e;
        auto status = SDL_PollEvent(&e);
        if (status) {
            if (e.type == SDL_KEYDOWN) {
                appendLine("SDL_KEYDOWN = " + to_string(e.key.keysym.scancode));
            } else if (e.type == SDL_KEYUP) {
                appendLine("SDL_KEYUP = " + to_string(e.key.keysym.scancode));

            } else if (e.type == SDL_JOYBUTTONDOWN) {
                appendLine("SDL_JOYBUTTONDOWN = " + to_string(e.jbutton.button));
                buttonDownCount++;
            } else if (e.type == SDL_JOYBUTTONUP) {
                appendLine("SDL_JOYBUTTONUP = " + to_string(e.jbutton.button));
                if (buttonDownCount > 0)
                    buttonDownCount--;

            } else if (e.type == SDL_JOYHATMOTION) {
                appendLine("SDL_JOYHATMOTION = " + to_string(e.jhat.value));
            } else if (e.type == SDL_JOYAXISMOTION) {
                appendLine("SDL_JOYAXISMOTION = " + to_string(e.jaxis.axis) + ", " + to_string(e.jaxis.value));

            } else if (e.type == SDL_MOUSEMOTION ||
                       e.type == SDL_MOUSEBUTTONDOWN ||
                       e.type == SDL_MOUSEBUTTONUP ||
                       e.type == SDL_MOUSEWHEEL ||
                       e.type == SDL_WINDOWEVENT
                      ) {
                ; // ignore
            } else {
                stringstream strhex;
                strhex << std::hex << e.type;
                appendLine("something else = 0x" + strhex.str());
            }
            render();
        }
    }
    appendLine("Release all buttons now");
    sleep(3);
    while (SDL_PollEvent(&e))
        ;   // eat any events in the queue
}
