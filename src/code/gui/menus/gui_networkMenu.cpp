#include "gui_networkMenu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../gui.h"
#include "../gui_confirm.h"
#include "../gui_keyboard.h"
#include "../../lang.h"
#include <cassert> // assert
#include <iostream>

using namespace std;

enum { SSID=0, PW, Blank, WriteFile };

//*******************************
// GuiNetworkMenu::init
//*******************************
void GuiNetworkMenu::init() {
    GuiMenuBase::init();    // call the base init

    auto removeCRLF = [&] (string &str) {
        string crlf = "\r\n";
        for (char ch : crlf)
            str.erase( std::remove(str.begin(), str.end(), ch), str.end() );
    };

    // read from ssid.cfg if it exists
    string path = Env::getPathToBleemsyncCFGDir() + sep + "ssid.cfg";
    if (DirEntry::exists(path)) {
        ifstream ssidFile;
        ssidFile.open(path.c_str(), ios::binary);

        getline(ssidFile, ssid);
        removeCRLF(ssid);

        getline(ssidFile, password);
        removeCRLF(password);

        ssidFile.close();
    }
    else {
        ssid = "sample";
        password = "secret";
    }

    fill();
}

//*******************************
// GuiNetworkMenu::fill
//*******************************
void GuiNetworkMenu::fill() {
    lines.clear();
    lines.emplace_back(_("SSID:") + " " + ssid);
    lines.emplace_back(_("Password:") + " " + password);
    lines.emplace_back("");
    lines.emplace_back(_("Write ssid.cfg file"));
}

//*******************************
// GuiNetworkMenu::getStatusLine
//*******************************
// returns the status line at the bottom
string GuiNetworkMenu::getStatusLine() {
    switch (selected) {
        case SSID:
            return "   |@X| " + _("Edit SSID") +
                    "   |@O| " + _("Cancel") + " |";

        case PW:
            return "   |@X| " + _("Edit Password") +
                   "   |@O| " + _("Cancel") + " |";

        case Blank:
            return "";

        case WriteFile:
            return "   |@X| " + _("Write ssid.cfg file") +
                   "   |@O| " + _("Cancel") + " |";

        default:
            assert(false);
            return "";
    }
}

//*******************************
// GuiNetworkMenu::doCirclePressed
//*******************************
void GuiNetworkMenu::doCircle_Pressed() {
    Mix_PlayChannel(-1, gui->cancel, 0);
    menuVisible = false;
}

//*******************************
// GuiNetworkMenu::doCrossPressed
//*******************************
void GuiNetworkMenu::doCross_Pressed() {
    Mix_PlayChannel(-1, gui->cursor, 0);

    switch (selected) {
        case SSID: {
            GuiKeyboard *keyboard = new GuiKeyboard(renderer);
            keyboard->label = _("Enter SSID");
            keyboard->result = ssid;
            keyboard->show();
            string result = keyboard->result;
            bool cancelled = keyboard->cancelled;
            delete (keyboard);
            if (!cancelled) {
                ssid = result;
                fill();
            }
        }
            break;

        case PW: {
            GuiKeyboard *keyboard = new GuiKeyboard(renderer);
            keyboard->label = _("Enter Password");
            keyboard->result = password;
            keyboard->show();
            string result = keyboard->result;
            bool cancelled = keyboard->cancelled;
            delete (keyboard);
            if (!cancelled) {
                password = result;
                fill();
            }
        }
            break;

        case Blank:
            break;

        case WriteFile: {
            string path = Env::getPathToBleemsyncCFGDir() + sep + "ssid.cfg";
            ofstream ssidFile(path, ios_base::trunc);
            ssidFile << ssid << endl;
            ssidFile << password << endl;
            ssidFile.flush();
            ssidFile.close();
            menuVisible = false;
            }
            break;
        default:
            assert(false);
    }

    render();
}
