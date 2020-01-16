#include "gui_memcards.h"
#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"
#include "../engine/memcard.h"
#include "gui_confirm.h"
#include "gui_keyboard.h"
#include "../lang.h"

using namespace std;

//*******************************
// GuiMemcards::init
//*******************************
void GuiMemcards::init() {
    GuiMenu::init();    // call the base init

    Memcard *memcardOps = new Memcard(gui->pathToGamesDir);
    lines = memcardOps->list();
    delete memcardOps;
}

//*******************************
// GuiMemcards::statusLine
//*******************************
// returns the status line at the bottom
string GuiMemcards::statusLine() {
    return _("Card") + " " + to_string(selected + 1) + "/" + to_string(getVerticalSize()) +
           "   |@L1|/|@R1| " + _("Page") +
           "   |@X| " + _("Rename") +
           "  |@S| " + _("New Card") +
           "   |@T| " + _("Delete") +
           "  |@O| " + _("Go back") + "|";
}

//*******************************
// GuiMemcards::doCircle
//*******************************
void GuiMemcards::doCircle() {
    Mix_PlayChannel(-1, gui->cancel, 0);
    menuVisible = false;
}

//*******************************
// GuiMemcards::doSquare
//*******************************
void GuiMemcards::doSquare() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    GuiKeyboard *keyboard = new GuiKeyboard(renderer);
    keyboard->label = _("Enter new card name");
    keyboard->show();
    string result = keyboard->result;
    bool cancelled = keyboard->cancelled;
    delete (keyboard);

    if (result.empty()) {
        cancelled = true;
    }

    string testResult = result;
    if (Util::compareCaseInsensitive("sony", testResult)) {
        cancelled = true;
    }

    if (!cancelled) {
        Memcard *memcardOps = new Memcard(gui->pathToGamesDir);
        memcardOps->newCard(result);
        lines = memcardOps->list();
        int i = 0;
        for (const string & card : lines) {
            if (card == result) {
                selected = i;
                firstVisibleIndex = i;
                lastVisibleIndex = firstVisibleIndex + maxVisible - 1;

                if (getVerticalSize() > maxVisible) {
                    if (lastVisibleIndex >= getVerticalSize()) {
                        lastVisibleIndex = getVerticalSize() - 1;
                        firstVisibleIndex = lastVisibleIndex - maxVisible + 1;
                    }
                }
            }
            i++;
        }
        delete memcardOps;
    }
    render();
}

//*******************************
// GuiMemcards::doTriangle
//*******************************
void GuiMemcards::doTriangle() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (getVerticalSize() != 0) {
        GuiConfirm *guiConfirm = new GuiConfirm(renderer);
        guiConfirm->label = _("Delete card") + " '" + lines[selected] + "' ?";
        guiConfirm->show();
        bool result = guiConfirm->result;
        delete (guiConfirm);

        if (result) {
            Memcard *memcardOps = new Memcard(gui->pathToGamesDir);
            memcardOps->deleteCard(lines[selected]);
            lines = memcardOps->list();
            delete memcardOps;
        }
        render();
    }
}

//*******************************
// GuiMemcards::doCross
//*******************************
void GuiMemcards::doCross() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (lines.empty()) {
        return;
    }

    GuiKeyboard *keyboard = new GuiKeyboard(renderer);
    keyboard->label = _("Enter new name for card") + " '" + lines[selected] + "'";
    keyboard->result = lines[selected];
    keyboard->show();
    string result = keyboard->result;
    bool cancelled = keyboard->cancelled;
    delete (keyboard);

    if (result.empty()) {
        cancelled = true;
    }

    string testResult = result;
    if (Util::compareCaseInsensitive("sony", testResult)) {
        cancelled = true;
    }

    for (const string & card:lines) {
        if (card == result) {
            // orevent overwrite other card
            cancelled = true;
        }
    }

    if (!cancelled) {
        Memcard *memcardOps = new Memcard(gui->pathToGamesDir);
        memcardOps->rename(lines[selected], result);
        delete memcardOps;
        init();
        int pos = 0;
        for (const string & card:lines) {
            if (card == result) {
                selected = pos;
                firstVisibleIndex = pos;
                lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
            }
            pos++;
        }
    }
    render();
}
