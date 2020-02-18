#include "environment.h"
#include "DirEntry.h"
#include <dirent.h>
#include <unistd.h>
#include <climits>

using namespace std;

// these strings are initialized by main.cpp.  once initialized they should not be modified.
// they are not declared in the header to prevent outside code from accidentally modifying them.

// passing a single arg on the debug command line instead of passing two args is optional.  two arg is still available.
//
// in single arg mode you pass the path to the root of the flash USB drive.  in this mode, as much as possible, files
// from the USB drive are used instead of files in the debug build environment.  this allows you to debug the files
// on the USB drive.  this also allows someone to send you the contents of a USB drive to debug a
// problem.  you will use the UI theme, the menu themes, the cover db files, the regional db file, the .prev file,
// the retroarch playlists, the rom files, the lang files, and the config.ini file, etc.

bool private_singleArgPassed = false;
string private_pathToUSBDrive;
string private_pathToGamesDir;
string private_pathToRegionalDBFile;
string private_pathToInternalDBFile;

//*******************************
// Environment:: One Liners
//*******************************

// Kind of redundant...
string Environment::getPathToUSBRoot() {
  return private_pathToUSBDrive;
}

string Environment::getPathToAutobleemDir() {
    return private_pathToUSBDrive + sep + "Autobleem";
}

string Environment::getPathToAppsDir() {
    return getPathToAutobleemDir() + sep + "Apps";
}

std::string Environment::getPathToRCDir() {
    return getPathToAutobleemDir() + sep + "rc";
}

string Environment::getPathToGamesDir() {
    return private_pathToGamesDir;
}

string Environment::getPathToMemCardsDir() {
    return private_pathToGamesDir + sep + "!MemCards";
}

string Environment::getPathToSaveStatesDir() {
    return private_pathToGamesDir + sep + "!SaveStates";
}

string Environment::getPathToSystemDir() {
    return getPathToAutobleemDir() + sep + "System";
}

string Environment::getPathToRetroarchDir() {
    return private_pathToUSBDrive + sep + "project_eris" + sep + "opt" + sep + "retroarch";
}

string Environment::getPathToRetroarchPlaylistsDir() {
    return getPathToRetroarchDir() + sep + "config" + sep + "retroarch" + sep + "playlists";
}

string Environment::getPathToRetroarchCoreFile() {
    return getPathToRetroarchDir() + sep + "config" + sep + "retroarch" + sep + "cores" + sep + "pcsx_rearmed_libretro.so";
}

string Environment::getPathToRomsDir() {
    return getPathToAutobleemDir + sep + "roms";
}

// includes the "regional.db" filename
string Environment::getPathToRegionalDBFile() {
    return private_pathToRegionalDBFile;
}

// includes the "internal.db" filename
string Environment::getPathToInternalDBFile() {
    return private_pathToInternalDBFile;
}

// for networking
// Bleemsync is no more so this needs retiring.
string Environment::getPathToBleemsyncDir() {
    return private_pathToUSBDrive + sep + "bleemsync";
}

string Environment::getPathToBleemsyncCFGDir() {
    return getPathToBleemsyncDir() + sep + "etc" + sep + "bleemsync" + sep + "CFG";
}

string Environment::getPathToBleemsyncWPADir() {
    return getPathToBleemsyncDir() + sep + "network" + sep + "etc" + sep + "wpa_supplicant";
}

string Environment::getPathToLogsDir() {
    return private_pathToUSBDrive + sep + "logs";
}

//*******************************
// Environment::getWorkingPath
// 1 arg: "usb:/Autobleem/bin/autobleem"
// 2 arg: autobleem-gui executable dir
// PSC: autobleem-gui executable dir
//*******************************
string Environment::getWorkingPath() {
#if defined(__x86_64__) || defined(_M_X64)
    if (private_singleArgPassed) {
        string path = private_pathToUSBDrive + sep + "Autobleem" + sep + "bin" + sep + "autobleem";
        return path;
    } else {
        char temp[PATH_MAX];
        return (getcwd(temp, sizeof(temp)) ? string(temp) : string(""));
    }
#else
    char temp[PATH_MAX];
        return (getcwd(temp, sizeof(temp)) ? string(temp) : string(""));
#endif
}

//*******************************
// Environment::getSonyPath
// 1 arg: "usb:/Autobleem/bin/autobleem/sony"
// 2 arg: autobleem-gui executable dir + sep + "sony"
// PSC: autobleem-gui executable dir
//*******************************
string Environment::getSonyPath() {
#if defined(__x86_64__) || defined(_M_X64)
    return getWorkingPath() + sep + "sony";
#else
    return "/usr" + sep + "sony" + sep + "share" + sep + "data";
#endif
}

//*******************************
// Environment::getSonyFontPath
// 1 arg: "usb:/Autobleem/bin/autobleem/sony/font"
// 2 arg: autobleem-gui executable dir + sep + "sony" + sep + "font"
// PSC: autobleem-gui executable dir
//*******************************
string Environment::getSonyFontPath() {
    return getSonyPath() + sep + "font";
}

#if 0
//*******************************
// Environment::getPathToWorkingPathFile
// return path to file in working path
// 1 arg: "usb:/Autobleem/bin/autobleem/filename"
// 2 arg: autobleem-gui executable dir/filename
// PSC: autobleem-gui executable dir/filename
//*******************************
string Environment::getPathToWorkingPathFile(const std::string &filename) {
    return getWorkingPath() + sep + filename;
}
#endif

//*******************************
// Environment::getPathToThemesDir
// 1 arg: "usb:/themes", 2 arg: "./themes"
// PSC: "/media/themes"
//*******************************
string Environment::getPathToThemesDir() {
#if defined(__x86_64__) || defined(_M_X64)
    if (private_singleArgPassed) {
        string path = private_pathToUSBDrive + sep + "themes";
        return path;
    } else {
        string path = getWorkingPath() + sep + "themes";
        return path;
    }
#else
    string path = "/media" + sep + "Autobleem" + sep + "themes";
    return path;
#endif
}

//*******************************
// Environment::getPathToCoversDBDir
// 1 arg: "usb:/Autobleem/bin/db", 2 arg: "../db"
// PSC: "../db"
//*******************************
string Environment::getPathToCoversDBDir() {
#if defined(__x86_64__) || defined(_M_X64)
    if (private_singleArgPassed) {
        string path = private_pathToUSBDrive + sep + "Autobleem" + sep + "bin" + sep + "db";
        return path;
    } else {
        string path = ".." + sep + "db";
        return path;
    }
#else
    string path =  ".." + sep + "db";
    return path;
#endif
}
