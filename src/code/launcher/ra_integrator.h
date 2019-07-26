//
// Created by screemer on 2019-07-25.
//

#pragma once

#include <cstring>
#include "../main.h"
#include "ps_game.h"

using namespace std;

class RAIntegrator {
public:
    bool getGames(PsGames *result, string playlist);
    vector<string> getPlaylists();
    bool autoDetectCorePath(PsGame *game, string* core_name, string* core_path);
private:
    bool isValidPlaylist(string path);
    bool isJSONPlaylist(string path);
    void parseJSON(PsGames *result, string path);
    void parse6line(PsGames *result, string path);
};


