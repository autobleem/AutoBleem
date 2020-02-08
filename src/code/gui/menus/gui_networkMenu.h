#pragma once

#include "gui_stringMenu.h"
#include <vector>
#include <string>
#include "../../lang.h"


//********************
// GuiNetworkMenu
//********************
class GuiNetworkMenu : public GuiStringMenu {
public:
    GuiNetworkMenu(SDL_Shared<SDL_Renderer> _renderer) : GuiStringMenu(_renderer) {}

    void init() override;
    void fill();

    void readCfgFile();
    void writeCfgFile();
    static void initializeWifi();   // we currently can't tell if it was successful or not

    virtual std::string getTitle() override { return "-=" + _("-=" + _("Edit Network SSID and Password (ssid.cfg)") + "=-") + "=-"; }
    virtual std::string getStatusLine() override;   // returns the status line at the bottom

    std::string ssid, password;
    bool displayAsterisksInsteadOfPassword = true;   // this is a password.  display *****

    static std::string getCfgPath() { return Env::getPathToBleemsyncCFGDir() + sep + "ssid.cfg"; }
    static std::string getRunPath() { return Env::getPathToApps() + sep + "bleemsync_networking" + sep + "run.sh"; }
    static std::string getWpaSupplicantPath()
        { return Env::getPathToBleemsyncDir() + sep + "network/etc/wpa_supplicant/wpa_supplicant.conf"; }

    static std::string getSSID();
    static std::string getSSIDfrom_ssidcfg();
    static std::string getSSIDfrom_wpa_supplicant();

    void doCircle_Pressed() override;
    void doCross_Pressed() override;

    void doEnter() { doCross_Pressed(); }
    void doEscape() { doCircle_Pressed(); }
};
