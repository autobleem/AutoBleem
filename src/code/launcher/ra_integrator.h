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

//********************
// CoreInfo
//********************
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

//********************
// RAPlaylistInfo
//********************
struct RAPlaylistInfo {
    std::string displayName;
    std::string path;
    PsGames psGames;

    RAPlaylistInfo(const std::string& _displayName, const std::string& _path)
        : displayName(_displayName), path(_path) { }

    RAPlaylistInfo(const std::string& _displayName, const std::string& _path, const PsGames& games)
        : RAPlaylistInfo(_displayName, _path)
        { psGames = games; }
};

//********************
// RAIntegrator
//********************
class RAIntegrator {
    RAIntegrator() { }  // only getInstance can call
public:
    // don't call the read playlist routine until the evironment paths are setup in main.cpp
    static std::shared_ptr<RAIntegrator> getInstance(); // shared singleton
    ~RAIntegrator();

    CoreInfos cores;
    map<string,CoreInfoPtr> defaultCores;
    map<string,CoreInfoPtr> overrideCores;
    set<string> databases;

    std::vector<RAPlaylistInfo> playlistInfos;
    //std::string favoritesFileName;
    std::string favoritesDisplayName { "Favorites" };
    std::tuple<bool,int> playlistNameToIndex(const std::string& name);  // returns true if name found, and the index

    PsGames readGamesFromPlaylistFile(const std::string& path);         // read one lpl file
    void readGamesFromAllPlaylists();                                   // reads all the playlist info into playlistInfos

    std::string findFavoritesPlaylistPath();                            // returns "" if not found
    void reloadFavorites(); // after running RA, favorites may have been added or removed

    vector<string> getPlaylists();
    PsGames getGames(string playlist);
    int getGamesNumber(string playlist);

    bool autoDetectCorePath(PsGamePtr game, string& core_name, string& core_path);
    bool findOverrideCore(PsGamePtr game, string& core_name, string& core_path);
    static string escapeName(string input);
    void initCoreInfo();

    static bool sortByMaxExtensions(const CoreInfoPtr &i, const CoreInfoPtr &j) { return i->extensions.size() > j->extensions.size(); };

private:
    bool isGameValid(PsGamePtr game);
    bool isValidPlaylist(string path);
    bool isJSONPlaylist(string path);
    PsGames parseJSON(string path);
    PsGames parse6line(string path);
    CoreInfoPtr parseCoreInfo(string file, string entry);
};


