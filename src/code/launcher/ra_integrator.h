//
// Created by screemer on 2019-07-25.
//

#pragma once

#include <cstring>
#include <map>
#include <set>
#include "../main.h"
#include "ps_game.h"

using namespace std;

class CoreInfo
{
public:

    string name;
    vector<string> extensions;
    vector<string> databases;
    string core_path;
};


using CoreInfoPtr = std::shared_ptr<CoreInfo>;
using CoreInfos = std::vector<CoreInfoPtr>;

class RAIntegrator {
public:
    CoreInfos cores;
    map<string,CoreInfoPtr> defaultCores;
    set<string> databases;

    bool getGames(PsGames *result, string playlist);
    vector<string> getPlaylists();
    void autoDetectCorePath(PsGamePtr game, string& core_name, string& core_path);
    string escapeName(string input);
    void initCoreInfo();

    static bool sortByMaxExtensions(const CoreInfoPtr &i, const CoreInfoPtr &j) { return i->extensions.size() > j->extensions.size(); };

private:
    bool isGameValid(PsGamePtr game);
    bool isValidPlaylist(string path);
    bool isJSONPlaylist(string path);
    void parseJSON(PsGames *result, string path);
    void parse6line(PsGames *result, string path);
    CoreInfoPtr parseInfo(string file, string entry);
};


