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
    void initializeWifi();

    virtual std::string getTitle() override { return "-=" + _("-=" + _("Edit Network SSID and Password (ssid.cfg)") + "=-") + "=-"; }
    virtual std::string getStatusLine() override;   // returns the status line at the bottom

    std::string ssid, password;
    std::string cfgPath, runPath;

    void doCircle_Pressed() override;
//    void doSquare_Pressed() override;
//    void doTriangle_Pressed() override;
    void doCross_Pressed() override;

    void doEnter() { doCross_Pressed(); }
    void doEscape() { doCircle_Pressed(); }
//    void doDelete() { doSquare_Pressed(); }
};
