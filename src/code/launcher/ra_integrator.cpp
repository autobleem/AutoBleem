//
// Created by screemer on 2019-07-25.
//

#include "ra_integrator.h"
#include "fstream"
#include <cstdio>
#include <iostream>
#include "../util.h"
#include "../nlohmann/json.h"

using namespace nlohmann;

bool RAIntegrator::isValidPlaylist(string path) {
    // check file extension
    if (ReturnLowerCase(Util::getFileExtension(path)) != "lpl") {
        return false;
    }
    // check if not empty
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    if (in.tellg() <= 0) {
        return false;
    }

    return true;
}

bool RAIntegrator::isJSONPlaylist(string path) {
    std::ifstream in(path, std::ifstream::binary);
    string line;
    getline(in, line);
    trim(line);
    if (line.empty()) {
        return false;
    }
    if (line == "{")
        return true;
    return false;
}

void RAIntegrator::parseJSON(PsGames *result, string path) {
    std::ifstream in(path, std::ifstream::binary);
    json j;
    in >> j;

    int id = 0;

    json array = j["items"];

    for (json::iterator it = array.begin(); it != array.end(); ++it) {
        PsGamePtr game{new PsGame};
        game->gameId = id++;
        game->title = (*it)["label"];
        game->publisher = "";
        game->year = 0;
        game->players = 0;
        game->folder = "";
        game->ssFolder = "";
        game->base = "";
        game->memcard = "";
        game->cds = 0;
        game->locked = true;
        game->hd = false;
        game->favorite = false;
        game->foreign = true;
        game->core_name = (*it)["core_name"];
        game->core_path = (*it)["core_path"];
        game->db_name =  (*it)["db_name"];
        game->image_path =  (*it)["path"];
        result->push_back(game);
        id++;
    }


}

void RAIntegrator::parse6line(PsGames *result, string path) {

}

bool RAIntegrator::getGames(PsGames *result, string playlist) {
    string path = string(RA_FOLDER) + Util::separator() + "playlists" + Util::separator() + playlist;
    if (isJSONPlaylist(path)) {
        parseJSON(result, path);
    } else {
        parse6line(result, path);
    }
    return true;
}

vector<string> RAIntegrator::getPlaylists() {
    vector<string> result;
    if (!Util::exists(RA_FOLDER)) {
        return result;
    }

    string path = string(RA_FOLDER) + Util::separator() + "playlists";
    vector<DirEntry> entries = Util::diru(path);
    for (const DirEntry &entry:entries) {
        if (entry.isDir) continue;
        if (Util::getFileNameWithoutExtension(entry.name)=="AutoBleem") continue;
        if (isValidPlaylist(path + Util::separator() + entry.name)) {
            result.push_back(entry.name);
        }
    }
    return result;
}

