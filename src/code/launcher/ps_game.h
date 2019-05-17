//
// Created by screemer on 2/12/19.
//

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>

class PsGame {
public:
    int gameId = 0;
    std::string title = "";
    std::string publisher = "";
    int year = 0;
    int players = 0;

    std::string memcard = "";
    std::string folder = "";
    std::string ssFolder = "";

    std::string base = "";

    bool internal = false;
    bool hd = false;
    bool locked = false;
    int cds = 1;
    bool favorite = false;

    void setMemCard(std::string name);
    std::string findResumePicture();
    bool isResumeSlotActive(int slot);
    std::string findResumePicture(int slot);
    void storeResumePicture(int slot);
    bool isCleanExit();
    void removeResumePoint(int slot);
};

using PsGamePtr = std::shared_ptr<PsGame>;
using PsGames = std::vector<PsGamePtr>;