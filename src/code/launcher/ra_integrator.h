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
    std::string name;
    PsGames psGames;

    RAPlaylistInfo(const std::string& _name, const PsGames& games) : name(_name), psGames(games) { }
};
//using RAPlaylistInfos = std::vector<RAPlaylistInfo>;

//********************
// RAIntegrator
//********************
class RAIntegrator {
    RAIntegrator() { }  // only getInstance can call
public:
    ~RAIntegrator();

    CoreInfos cores;
    map<string,CoreInfoPtr> defaultCores;
    map<string,CoreInfoPtr> overrideCores;
    set<string> databases;

    std::vector<RAPlaylistInfo> playlistInfos;
    std::tuple<bool,int> playlistNameToIndex(const std::string& name);     // returns true for success, and the index if successful
    void readInAllData();    // reads all the playlist info into RAPlaylistInfos


    vector<string> getPlaylists() {
        vector<string> temp;
        for (auto& info : playlistInfos)
            temp.emplace_back(info.name);

        return temp;
    }

    PsGames getGames(string playlist) {
        PsGames games;
        auto [success, index] = playlistNameToIndex(playlist);
        if (success)
            games = playlistInfos[index].psGames;
        return games;
    }

    int getGamesNumber(string playlist) {
        auto [success, index] = playlistNameToIndex(playlist);
        if (success)
            return playlistInfos[index].psGames.size();
        else
            return 0;
    }

//    vector<string> getPlaylists();
//    bool getGames(PsGames *result, string playlist);
//    int getGamesNumber(string playlist);

    bool autoDetectCorePath(PsGamePtr game, string& core_name, string& core_path);
    bool findOverrideCore(PsGamePtr game, string& core_name, string& core_path);
    static string escapeName(string input);
    void initCoreInfo();

    static bool sortByMaxExtensions(const CoreInfoPtr &i, const CoreInfoPtr &j) { return i->extensions.size() > j->extensions.size(); };

    static std::shared_ptr<RAIntegrator> getInstance() {
        static std::shared_ptr<RAIntegrator> singleInstance{new RAIntegrator};
        // we need the environment paths to be inited before the singleton starts reading data
        // so don't read data until getInstance is called the first time
        // the environment paths are inited in main.cpp
        static bool firstTime {true};
        if (firstTime) {
            singleInstance->initCoreInfo();
            singleInstance->readInAllData();
            firstTime = false;
        }
        return singleInstance;
    }

private:
    bool isGameValid(PsGamePtr game);
    bool isValidPlaylist(string path);
    bool isJSONPlaylist(string path);
    PsGames parseJSON(string path);
    PsGames parse6line(string path);
    CoreInfoPtr parseCoreInfo(string file, string entry);
};


