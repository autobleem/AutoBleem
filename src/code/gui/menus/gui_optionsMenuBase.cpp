#include "gui_optionsMenuBase.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <cassert>
#include "../../lang.h"

using namespace std;

#if 0
//*******************************
// void GuiOptionsMenuBase::init()
//*******************************
void GuiOptionsMenuBase::init()
{
}
#endif

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
