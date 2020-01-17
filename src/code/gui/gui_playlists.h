#pragma once

#include "gui_menuBase.h"
#include "../lang.h"
#include "../launcher/ra_integrator.h"

class GuiPlaylists : public GuiMenuBase {
public:
    GuiPlaylists(SDL_Shared<SDL_Renderer> _renderer) : GuiMenuBase(_renderer,
                                                       "-=" + _("Select RetroBoot Platform") + "=-") {}

    void init() override {
        for (const string& playlist : playlists) {
            lines.emplace_back(playlist + " (" + to_string(integrator->getGamesNumber(playlist)) + " " + _("games") + ")");
        }
        GuiMenuBase::init();
    }
    void render() override { GuiMenuBase::render(); };
    void loop() override { GuiMenuBase::loop(); };

    void doEnter() { doCross_Pressed(); }
    void doEscape() { doCircle_Pressed(); }

    std::vector<std::string> playlists;
    shared_ptr<RAIntegrator> integrator;
};


