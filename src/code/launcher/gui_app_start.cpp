//
// Created by screemer on 2019-12-04.
//

#include "gui_app_start.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <sstream>
#include <iostream>
#include "../gui/gui.h"
#include "../lang.h"
#include "../engine/scanner.h"


using namespace std;

std::string GuiAppStart::getStringLine(const std::string& str, int lineNo)
{
    std::string line;
    std::istringstream stream(str);
    while (lineNo-- >= 0)
        std::getline(stream, line);
    return line;
}

void GuiAppStart::init()
{
    font = Fonts::openNewSharedFont(Environment::getWorkingPath()+ sep + "november.ttf",20);
    // Try to load app.ini
    appName = game->title;


    if (DirEntry::exists(game->readme_path)) {
        std::ifstream t(game->readme_path);
        t.seekg(0, std::ios::end);
        size_t size = t.tellg();
        cout << "Readme file size:" << size << endl;
        buffer="";
        t.seekg(0);
        std::string temp;
        while(std::getline(t, temp)) {
            buffer = buffer + temp + "\n";
        };

        int newlines = 0;
        const char * p = &buffer.c_str()[0];
        for ( int i = 0; i < size; i++ ) {
            if ( p[i] == '\n' ) {
                newlines++;
            }
        }
        totalLines = newlines+1;
        readmeLoaded = true;


    }
}

void GuiAppStart::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
// readme:
    SDL_SetRenderDrawColor(renderer,0, 0, 0, 128);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect rect2;
    rect2.x = 10;
    rect2.y = 10;
    rect2.w = 1260;
    rect2.h = 600;

    SDL_RenderFillRect(renderer, &rect2);

    // scrollbar
    rect2.x = 1240;
    rect2.y = 40;
    rect2.w = 20;
    rect2.h = 20*25;

    SDL_RenderFillRect(renderer, &rect2);

    // draw scroll position
    if (maxLines<totalLines) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        int heightOfBar = 500/(totalLines-maxLines);

        rect2.x = 1242;
        rect2.y = 40 + firstLine*heightOfBar;
        rect2.w = 16;
        rect2.h = heightOfBar;
        SDL_RenderFillRect(renderer, &rect2);

    }
    int offset = 15;
    gui->renderTextLine(appName,0, offset ,0,10,font);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, rect2.x, 35,rect2.w,35);

    if (scrolling>0)
    {
        firstLine++;
    }
    if (scrolling<0)
    {
        firstLine--;
    }
    if (firstLine<0)
    {
        firstLine=0;
    }
    if (maxLines<totalLines) {
        if (firstLine > totalLines - maxLines) {
            firstLine = totalLines - maxLines;
        }
    } else
    {
        if (firstLine > totalLines - maxLines) {
            firstLine = 0;
        }
    }

    int currentLine =2;
    if (!readmeLoaded)
    {
        gui->renderTextLine(_("ReadMe file not found"),2,offset,0,10,font);
    } else
    {
        for (int i=firstLine;i<firstLine+maxLines;i++)
        {
            std::string lineInFile = getStringLine(buffer,i);
            gui->renderTextLine(getStringLine(buffer,i),currentLine,offset,0,10,font);
            currentLine++;
        }
    }


    gui->renderStatus("|@X| " + _("OK") + "  |@O| " + _("Cancel") +"|");
    SDL_RenderPresent(renderer);
}


void GuiAppStart::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());
    bool menuVisible = true;
    while (menuVisible) {
        gui->watchJoystickPort();
        SDL_Event e;
        render();
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();

                }
            }

            switch (e.type) {
                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {
                        result = true;
                        menuVisible = false;
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        result = false;
                        menuVisible = false;
                    };

                case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                case SDL_JOYHATMOTION:

                    if (totalLines!=0) {
                        if (gui->mapper.isUp(&e)) {
                            scrolling = -1;
                        }
                        if (gui->mapper.isDown(&e)) {


                            scrolling = 1;
                        }
                        if (gui->mapper.isCenter(&e)) {
                            scrolling = 0;
                        }
                    }

            }



        }
    }
}
