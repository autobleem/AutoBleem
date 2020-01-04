//
// Created by screemer on 2019-07-25.
//

#include "ra_integrator.h"
#include "fstream"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../util.h"
#include <json.h>
#include "../engine/cfgprocessor.h"
#include "../DirEntry.h"
#include "../environment.h"

using namespace nlohmann;

//********************
// RAIntegrator::getInstance()
//********************
std::shared_ptr<RAIntegrator> RAIntegrator::getInstance() {
    static std::shared_ptr<RAIntegrator> singleInstance{new RAIntegrator};
    // we need the environment paths to be inited before the singleton starts reading data
    // so don't read data until getInstance is called the first time
    // the environment paths are inited in main.cpp
    static bool firstTime {true};
    if (firstTime) {
        singleInstance->initCoreInfo();
        singleInstance->readGamesFromAllPlaylists();
        firstTime = false;
    }
    return singleInstance;
}

//********************
// RAIntegrator::~RAIntegrator()
//********************
RAIntegrator::~RAIntegrator() {
    for (CoreInfoPtr ci: cores) {
        ci.reset();
    }
}

//********************
// RAIntegrator::isValidPlaylist
//********************
bool RAIntegrator::isValidPlaylist(string path) {
    // check file extension
    if (ReturnLowerCase(DirEntry::getFileExtension(path)) != "lpl") {
        cout << "Extension is not .lpl" << endl;
        return false;
    }
    // check if not empty
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    if (in.tellg() <= 0) {
        cout << "Playlist looks like empty file" << endl;
        return false;
    }

    return true;
}

//********************
// RAIntegrator::playlistNameToIndex
//********************
// returns true for success, and the index if successful
std::tuple<bool,int> RAIntegrator::playlistNameToIndex(const string& name) {
    auto it = find_if(begin(playlistInfos), end(playlistInfos),
                      [&] (RAPlaylistInfo& info) { return name == info.displayName; } );
    if (it != end(playlistInfos)) {
        return make_tuple(true, it - begin(playlistInfos));
    } else
        return make_tuple(false, -1);
}

//********************
// RAIntegrator::readGamesFromPlaylistFile()
//********************
PsGames RAIntegrator::readGamesFromPlaylistFile(const std::string& path) {
    cout << "Parsing Playlist: " << path << endl;
    PsGames psGames;
    if (isJSONPlaylist(path)) {
        psGames = parseJSON(path);
    } else {
        psGames = parse6line(path);
    }
    cout << "Games found: " << psGames.size() << endl;
    return psGames;
}

//********************
// RAIntegrator::findFavoritesPlaylistPath
//********************
// returns "" if not found
string RAIntegrator::findFavoritesPlaylistPath() {
    string defaultPath {Env::getPathToRetroarchDir() + sep + "content_favorites.lpl"};
    if (DirEntry::exists(defaultPath))
        return defaultPath;
    else
        return "";
}

//********************
// RAIntegrator::findHistoryPlaylistPath
//********************
// returns "" if not found
string RAIntegrator::findHistoryPlaylistPath() {
    string defaultPath {Env::getPathToRetroarchDir() + sep + "content_history.lpl"};
    if (DirEntry::exists(defaultPath))
        return defaultPath;
    else
        return "";
}

//********************
// RAIntegrator::reloadFavorites
//********************
// after running RA favorites may have been added or removed
void RAIntegrator::reloadFavorites() {
    string favPath = findFavoritesPlaylistPath();
    if (favPath != "") {
        auto favGames = readGamesFromPlaylistFile(favPath);

        // when RA adds a game to favorites for some reason the crc32 and db_name fields are empty.
        // we need the db_path to compute the path to the boxart.
        // find the original game in the playlists and copy the db_name to the favorite entry for the same game
        for (auto & favGame : favGames) {
            for (auto const & info : playlistInfos) {
                if (info.displayName != favoritesDisplayName) {
                    auto it = find_if(begin(info.psGames), end(info.psGames), [&] (PsGamePtr game) { return (game->image_path == favGame->image_path); } );
                    if (it != end(info.psGames)) {
                        favGame->db_name = (*it)->db_name;
                        break;
                    }
                }
            }
        }

        // remove any games in the favorites playlist that no longer exist
        auto it = remove_if(begin(favGames), end(favGames), [&] (PsGamePtr &game)
        { return game->title == "" || game->db_name == ""; });
        favGames.erase(it, end(favGames));

        std::tuple<bool,int> restuple = playlistNameToIndex(favoritesDisplayName);
        bool found = std::get<0>(restuple);
        int index = std::get<1>(restuple);
        if (found) {
            // the prior favorites is in the list.  modify the existing entry
            RAPlaylistInfo & fav = playlistInfos[index];
            fav.path = favPath;
            fav.psGames = favGames;
        } else {
            // the favorites list didn't exist before and isn't in the list.  add a new entry.
            playlistInfos.emplace_back(favoritesDisplayName, favPath, favGames);
        }
    }
}

//********************
// RAIntegrator::reloadHistory
//********************
// after running RA history may have been added or removed
void RAIntegrator::reloadHistory() {
    string histPath = findHistoryPlaylistPath();
    if (histPath != "") {
        auto histGames = readGamesFromPlaylistFile(histPath);

        // when RA adds a game to history for some reason the title, crc32, and db_name fields are empty.
        // we need the title and db_path to compute the path to the boxart.
        // find the original game in the playlists and copy the title and db_name to the history entry for the same game
        for (auto & histGame : histGames) {
            for (auto const & info : playlistInfos) {
                if (info.displayName != historyDisplayName) {
                    auto it = find_if(begin(info.psGames), end(info.psGames), [&] (PsGamePtr game) { return (game->image_path == histGame->image_path); } );
                    if (it != end(info.psGames)) {
                        histGame->title = (*it)->title;
                        histGame->db_name = (*it)->db_name;
                        break;
                    }
                }
            }
        }

        // remove any games in the history playlist that no longer exist
        auto it = remove_if(begin(histGames), end(histGames), [&] (PsGamePtr &game)
        { return game->title == "" || game->db_name == ""; });
        histGames.erase(it, end(histGames));

        std::tuple<bool,int> restuple = playlistNameToIndex(historyDisplayName);
        bool found = std::get<0>(restuple);
        int index = std::get<1>(restuple);
        if (found) {
            // the prior history is in the list.  modify the existing entry
            RAPlaylistInfo & hist = playlistInfos[index];
            hist.path = histPath;
            hist.psGames = histGames;
        } else {
            // the history list didn't exist before and isn't in the list.  add a new entry.
            playlistInfos.emplace_back(historyDisplayName, histPath, histGames);
        }
    }
}

//********************
// RAIntegrator::readGamesFromAllPlaylists
//********************
// reads all the playlist info into RAPlaylistInfos
void RAIntegrator::readGamesFromAllPlaylists() {
    assert(playlistInfos.size() == 0);
    if (playlistInfos.size() != 0) {
        reloadFavorites();  // playlists already read.  only need to update favorites in case changed in RA.
        reloadHistory();    // playlists already read.  only need to update history in case changed in RA.
        return;
    }

    string path = Env::getPathToRetroarchPlaylistsDir();
    cout << "Checking playlists path" << path << endl;

    if (DirEntry::exists(path)) {
        vector<DirEntry> entries = DirEntry::diru_FilesOnly(path);
        cout << "Total Playlists:" << entries.size() << endl;
        vector<string> playlistNames;
        for (const DirEntry &entry:entries) {
            if (DirEntry::getFileNameWithoutExtension(entry.name) == "AutoBleem")
                continue;
            playlistNames.emplace_back(entry.name);
        }

        // sort the playlist names and if any favorites or history add them at the end
        sort(begin(playlistNames), end(playlistNames));

        for (auto & playlistName : playlistNames) {
            cout << "Playlist: " << playlistName << endl;
            string path = Env::getPathToRetroarchPlaylistsDir() + sep + playlistName;
            if (isValidPlaylist(path)) {
                PsGames games = readGamesFromPlaylistFile(path);
                string nameOnly = DirEntry::getFileNameWithoutExtension(playlistName);
                if (games.size() > 0)
                    playlistInfos.emplace_back(nameOnly, path, games);
                else
                    cout << "Playlist has no games: " << playlistName << endl;
            }
            else
                cout << "Invalid Playlist: " << playlistName << endl;
        }
        reloadFavorites();  // since it isn't already in the list, reloadFavorites() will add favorites at the end
        reloadHistory();    // since it isn't already in the list, reloadHistory() will add history at the end
    }
}

//********************
// RAIntegrator::getPlaylists
//********************
vector<string> RAIntegrator::getPlaylists() {
    vector<string> temp;
    for (auto& info : playlistInfos)
        temp.emplace_back(info.displayName);

    return temp;
}

//********************
// RAIntegrator::getGames
//********************
PsGames RAIntegrator::getGames(string playlist) {
    PsGames games;
    std::tuple<bool,int> restuple = playlistNameToIndex(playlist);
    bool found = std::get<0>(restuple);
    int index = std::get<1>(restuple);
    if (found)
        games = playlistInfos[index].psGames;
    return games;
}

//********************
// RAIntegrator::getGamesNumber
//********************
int RAIntegrator::getGamesNumber(string playlist) {
    std::tuple<bool,int> restuple = playlistNameToIndex(playlist);
    bool found = std::get<0>(restuple);
    int index = std::get<1>(restuple);
    if (found)
        return playlistInfos[index].psGames.size();
    else
        return 0;
}

//********************
// RAIntegrator::findOverrideCore
//********************
bool RAIntegrator::findOverrideCore(PsGamePtr game, string &core_name, string &core_path) {
    string dbName = DirEntry::getFileNameWithoutExtension(game->db_name);

    lcase(dbName);    trim(dbName);
    map<string, CoreInfoPtr>::const_iterator pos = overrideCores.find(dbName);
    if (pos == overrideCores.end()) {
        core_name = "DETECT";
        core_path = "DETECT";
        return false;
    }
    core_name = pos->second->name;
    core_path = pos->second->core_path;
    return true;
}

//********************
// RAIntegrator::isJSONPlaylist
//********************
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

//********************
// RAIntegrator::parseJSON
//********************
PsGames RAIntegrator::parseJSON(string path) {
    std::ifstream in(path, std::ifstream::binary);
    json j;
    in >> j;

    int id = 0;
    PsGames psGames;

    json array = j["items"];

    for (const auto & item : array) {
        PsGamePtr game{new PsGame};
        game->gameId = id++;
        game->title = item["label"];
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
        game->core_name = item["core_name"];
        game->core_path = item["core_path"];
        game->db_name = item["db_name"];
        game->image_path = item["path"];

#if defined(__x86_64__) || defined(_M_X64)
        // if you are running in the debugging environment then /media might be shared drive /media/sf_G_DRIVE etc
        if (game->image_path.substr(0, 6) == "/media")
            game->image_path.replace(0, 6, Env::getPathToUSBRoot());
        if (game->core_path.substr(0, 6) == "/media")
            game->core_path.replace(0, 6, Env::getPathToUSBRoot());
#endif

        if ((game->core_path == "DETECT") || (game->core_name == "DETECT")) {
            autoDetectCorePath(game, game->core_name, game->core_path);
        }

        if (!DirEntry::exists(game->core_path)) {
            autoDetectCorePath(game, game->core_name, game->core_path);
        }
        if (isGameValid(game)) {
            psGames.emplace_back(game);
        } else {
            cout << "Game invalid: title = '" << game->title << "'" <<  endl;
        }
    }
    in.close();
    return psGames;
}

//********************
// RAIntegrator::parse6line
// this routine is called when the playlist file is NOT a json file
//********************
PsGames RAIntegrator::parse6line(string path) {
    PsGames psGames;
    std::ifstream in(path, ifstream::binary);

    string game_path = "";
    string label = "";
    string core_path = "";
    string core_name = "";
    string crc = "";
    string db_name = "";

    int id = 0;
    while (!in.eof()) {

        getline(in, game_path);
        if (in.eof()) break;
        getline(in, label);
        if (in.eof()) break;
        getline(in, core_path);
        if (in.eof()) break;
        getline(in, core_name);
        if (in.eof()) break;
        getline(in, crc);
        if (in.eof()) break;
        getline(in, db_name);

        PsGamePtr game{new PsGame};
        game->gameId = id++;
        game->title = label;
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
        game->core_name = core_name;
        game->core_path = core_path;
        game->db_name = db_name;
        game->image_path = game_path;
        if ((core_path == "DETECT") || (core_name == "DETECT")) {
            bool coreFound = autoDetectCorePath(game, core_name, core_path);
            if (!coreFound) continue;
            game->core_name = core_name;
            game->core_path = core_path;
        }
        if (!DirEntry::exists(game->core_path)) {
            bool coreFound = autoDetectCorePath(game, core_name, core_path);
            if (!coreFound) continue;
            game->core_name = core_name;
            game->core_path = core_path;

        }
        if (isGameValid(game)) {
            psGames.emplace_back(game);
        }
    }
    in.close();
    return psGames;
}

//********************
// RAIntegrator::autoDetectCorePath
//********************
bool RAIntegrator::autoDetectCorePath(PsGamePtr game, string &core_name, string &core_path) {
    if (findOverrideCore(game, core_name, core_path)) {
        return true;
    }
    string dbName = DirEntry::getFileNameWithoutExtension(game->db_name);
    map<string, CoreInfoPtr>::const_iterator pos = defaultCores.find(dbName);
    if (pos == defaultCores.end()) {
        core_name = "DETECT";
        core_path = "DETECT";
        return false;
    }
    core_name = pos->second->name;
    core_path = pos->second->core_path;
    return true;
}

//********************
// RAIntegrator::initCoreInfo
//********************
void RAIntegrator::initCoreInfo() {
    cout << "Building core list" << endl;
    if (!DirEntry::exists(Env::getPathToRetroarchDir())) {
        cout << "Retroarch Not Found" << endl;
        return;
    }
    cores.clear();
    databases.clear();
    defaultCores.clear();
    string infoFolder = Env::getPathToRetroarchDir() + sep + "info/";
    cout << "Scanning: " << infoFolder << endl;
    vector<DirEntry> entries = DirEntry::diru_FilesOnly(infoFolder);
    cout << "Found files:" << entries.size() << endl;
    for (const DirEntry &entry:entries) {
        if (DirEntry::getFileExtension(entry.name) == "info") {
            string fullPath = infoFolder + sep + entry.name;

            CoreInfoPtr ci = parseCoreInfo(fullPath, entry.name);
            cores.push_back(ci);
        }
    }
    sort(cores.begin(), cores.end(), sortByMaxExtensions); // why not

    for (const string &dbname:databases) {
        bool nextDb = false;

        for (CoreInfoPtr ciPtr:cores) {
            for (const string &db:ciPtr->databases) {
                if (dbname == db) {
                    defaultCores.insert(std::pair<string, CoreInfoPtr>(db, ciPtr));
                    nextDb = true;
                }
                if (nextDb) continue;
            }
            if (nextDb) continue;
        }

        map<string, CoreInfoPtr>::const_iterator pos = defaultCores.find(dbname);

        if (pos == defaultCores.end()) {

            continue;
        }

        cout << "Mapping DB: " << dbname << "  Core: " << pos->second->name << endl;
    }

    overrideCores.clear();
    ifstream in(Env::getWorkingPath() + sep + "coreOverride.cfg");
    string line;
    while (getline(in, line)) {
        string db_name = line.substr(0, line.find("="));
        string value = line.substr(line.find("=") + 1);
        cout << "Custom Core Override: " << db_name << "    core: " << value << endl;

        for (CoreInfoPtr ciPtr:cores) {
            if (ciPtr->name.find(value) != string::npos) {
                lcase(db_name);
                trim(db_name);
                overrideCores.insert(std::pair<string, CoreInfoPtr>(db_name, ciPtr));
                cout << "Found: " << db_name << "    core: " << ciPtr->name << " " << ciPtr->core_path << endl;
            }
        }

    }
    in.close();

}

//********************
// RAIntegrator::escapeName
//********************
string RAIntegrator::escapeName(string text) {
    return DirEntry::replaceTheseCharsWithThisChar(text, "&*/:`<>?\\|", '_');
}

//********************
// RAIntegrator::isGameValid
//********************
bool RAIntegrator::isGameValid(PsGamePtr game) {
    if (!DirEntry::exists(game->core_path)) {
        return false;
    }
    string path = game->image_path;
    if (path.find("#") != string::npos) {
        int pos = path.find("#");
        string check = path.substr(0, pos);
        if (!DirEntry::exists(check)) {

            return false;
        }
    } else {
        if (!DirEntry::exists(path)) {

            return false;
        }
    }
    return true;
}

//********************
// RAIntegrator::parseCoreInfo
//********************
CoreInfoPtr RAIntegrator::parseCoreInfo(string file, string entry) {
    ifstream in(file);
    string line;

    cout << "Parsing " << endl;
    CoreInfoPtr coreInfoPtr{new CoreInfo};
    coreInfoPtr->core_path = Env::getPathToRetroarchDir() + sep + "cores/" + DirEntry::getFileNameWithoutExtension(entry) + ".so";
    coreInfoPtr->extensions.clear();
    cout << "CorePath: " << coreInfoPtr->core_path << endl;
    while (getline(in, line)) {
        string lcaseline = line;
        lcase(lcaseline);

        if (lcaseline.rfind("display_name", 0) == 0) {
            string value = line.substr(lcaseline.find("=") + 1);
            value.erase(remove(value.begin(), value.end(), '\"'), value.end());
            trim(value);
            coreInfoPtr->name = value;
            cout << "CoreName: " << coreInfoPtr->name << endl;
        }
        if (lcaseline.rfind("supported_extensions", 0) == 0) {
            string value = line.substr(lcaseline.find("=") + 1);
            value.erase(remove(value.begin(), value.end(), '\"'), value.end());
            trim(value);

            coreInfoPtr->extensions.clear();
            std::stringstream check1(value);

            string intermediate;

            while (getline(check1, intermediate, '|')) {
                coreInfoPtr->extensions.push_back(intermediate);

            }
        }
        if (lcaseline.rfind("database", 0) == 0) {
            string value = line.substr(lcaseline.find("=") + 1);
            value.erase(remove(value.begin(), value.end(), '\"'), value.end());
            trim(value);

            coreInfoPtr->databases.clear();
            std::stringstream check1(value);

            string intermediate;

            while (getline(check1, intermediate, '|')) {
                coreInfoPtr->databases.push_back(intermediate);
                databases.insert(intermediate);
            }
        }

    }
    in.close();
    return coreInfoPtr;
}