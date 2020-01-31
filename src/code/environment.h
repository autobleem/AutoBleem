#pragma once

#include <string>

struct Environment {
    static std::string getPathToUSBRoot();
    static std::string getPathToApps();
    static std::string getPathToGamesDir();
    static std::string getPathToMemCardsDir();
    static std::string getPathToSaveStatesDir();
    static std::string getPathToSystemDir();
    static std::string getPathToRetroarchDir();
    static std::string getPathToRetroarchPlaylistsDir();
    static std::string getPathToRetroarchCoreFile();
    static std::string getPathToRomsDir();
    static std::string getPathToRegionalDBFile();   // includes the "regional.db" filename
    static std::string getPathToInternalDBFile();   // includes the "internal.db" filename

    // for networking
    static std::string getPathToBleemsyncDir();
    static std::string getPathToBleemsyncCFGDir();

    static std::string getWorkingPath();  // 1 arg: "usb:/Autobleem/bin/autobleem", 2 arg: autobleem-gui executable dir
    static std::string getSonyPath();  // 1 arg: "usb:/Autobleem/bin/autobleem/sony", 2 arg: "" + sep + "sony"
    static std::string getSonyFontPath();  // 1 arg: "usb:/Autobleem/bin/autobleem/sony", 2 arg: "" + sep + "sony"
//    static std::string getPathToWorkingPathFile(const std::string &filename);   // return path to file in working path

    static std::string getPathToThemesDir();        // "usb:/themes" or "./themes"
    static std::string getPathToCoversDBDir();   // "usb:/Autobleem/bin/db" or "../db"
};

using Env = Environment;
