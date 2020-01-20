#pragma once

#include "gui_stringMenu.h"
#include "../../lang.h"
#include "../../launcher/ra_integrator.h"

//*******************************
// class GuiPlaylists
//*******************************
class GuiPlaylists : public GuiStringMenu {
public:
    GuiPlaylists(SDL_Shared<SDL_Renderer> _renderer) : GuiStringMenu(_renderer) {}

    void init() override {
        for (const string& playlist : playlists) {
            lines.emplace_back(playlist + " (" + to_string(integrator->getGamesNumber(playlist)) + " " + _("games") + ")");
        }
        GuiStringMenu::init();
    }

    virtual std::string getTitle() override { return "-=" + _("Select RetroBoot Platform") + "=-"; }
    virtual std::string getStatusLine() override { return GuiStringMenu::getStatusLine(); }

    void doEnter() { doCross_Pressed(); }
    void doEscape() { doCircle_Pressed(); }

    std::vector<std::string> playlists;
    shared_ptr<RAIntegrator> integrator;
};


