//
// Created by screemer on 2/12/19.
//

#pragma once

#include <string>
#include <vector>
#include <memory>

//******************
// PsGame
//******************
class PsGame {
public:
    int gameId = 0;
    std::string title;
    std::string publisher;
    int year = 0;
    std::string serial;
    std::string region;
    int players = 0;

    std::string memcard;
    std::string folder;     // game folder.  internal example: "/gaadata/8/", USB example: "/media/Games/Racing/007 Racing"
    std::string ssFolder;  // !SaveStates folder.  ex: "/Games/!SaveStates/8", "/Games/!SaveStates/007 Racing"

    std::string base; // file name of the game.  not sure if extension is included.  
                      // code looks for .pbp extension and replaces it with cue.  but elsewhere .png is appended without removing extension.

    bool internal = false;
    bool hd = false;
    bool locked = false;
    int cds = 1;
    bool favorite = false;
    bool foreign = false; // to state it is not PS1 game (RA)
    std::string core_path;
    std::string image_path;
    std::string core_name;
    std::string db_name;


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

void operator += (PsGames &dest, const PsGames &src);
