#pragma once

#include "gui_menu.h"
#include "../lang.h"
#include "../launcher/ra_integrator.h"

class GuiPlaylists : public GuiMenu {
public:
    GuiPlaylists(SDL_Shared<SDL_Renderer> _renderer) : GuiMenu(_renderer,
                                                       "-=" + _("Select RetroBoot Platform") + "=-") {}

    void init() override {
        GuiMenu::init();
        for (const string& playlist : playlists) {
            lines.emplace_back(playlist + " (" + to_string(integrator->getGamesNumber(playlist)) + " " + _("games") + ")");
        }
    }
    void render() override { GuiMenu::render(); };
    void loop() override { GuiMenu::loop(); };

    std::vector<std::string> playlists;
    shared_ptr<RAIntegrator> integrator;
};


