//
// Created by steve on 12/28/19.
//
#include "gui_scrollWin.h"
#include <SDL2/SDL.h>
#include <string>
#include <iostream>

using namespace std;

//********************
// GuiScrollWin::init()
//********************
void GuiScrollWin::init() {
    gui = Gui::getInstance();
}

//********************
// GuiScrollWin::render()
//********************
void GuiScrollWin::render() {
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);

    for (int i=0; i < lines.size(); ++ i) {
        gui->renderTextLine(lines[i], i, 0, POS_LEFT);
    }
    SDL_RenderPresent(renderer);
}

//********************
// GuiScrollWin::appendLine()
//********************
void GuiScrollWin::appendLine(const std::string str) {
    if (lines.size() >= maxLines) {
        // the display is full, shift the lines up so we can overwrite the bottom line
        for (int i=1; i < maxLines; ++ i)
            lines[i-1] = lines[i];

        lines[lines.size()-1] = str;    // overwrite the last line
    } else
        lines.emplace_back(str);        // add a line at the end of the existing lines
        if (alsoWriteToCout)
            cout << str << endl;
}