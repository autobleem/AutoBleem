#include "gui_optionsMenuBase.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <cassert>
#include "../lang.h"

using namespace std;

//*******************************
// void GuiOptionsMenuBase::init()
//*******************************
void GuiOptionsMenuBase::init()
{
    gui = Gui::getInstance();
}

#if 0
//*******************************
// GuiOptionsMenuBase::render
//*******************************
void GuiOptionsMenuBase::render()
{
    SDL_RenderClear(renderer);
    gui->renderBackground();
    gui->renderTextBar();
    offset = gui->renderLogo(true);
    gui->renderTextLine(title, 0, offset, POS_CENTER);

    gui->renderStatus(getStatusLine());
    SDL_RenderPresent(renderer);
}
#endif

//*******************************
// GuiOptionsMenuBase::getStatusLine
//*******************************
// the default status line for menus.  override if needed.
string GuiOptionsMenuBase::getStatusLine() {
    return "|@X| " + _("OK") + "     " + "|@O| " + _("Cancel") + "|";
}

//*******************************
// GuiOptionsMenuBase::doKeyDown
//*******************************
void GuiOptionsMenuBase::doKeyDown() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected++;
    if (selected > getVerticalSize() - 1) {
        selected = 0;
    }
}

//*******************************
// GuiOptionsMenuBase::doKeyUp
//*******************************
void GuiOptionsMenuBase::doKeyUp() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected--;
    if (selected < 0) {
        selected = getVerticalSize() - 1;
    }
}

//*******************************
// GuiOptionsMenuBase::doKeyeDown
//*******************************
void GuiOptionsMenuBase::doPageDown() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected = getVerticalSize() - 1;
}

//*******************************
// GuiOptionsMenuBase::doPageUp
//*******************************
void GuiOptionsMenuBase::doPageUp() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected = 0;
}
