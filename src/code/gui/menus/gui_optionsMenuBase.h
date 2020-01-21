#pragma once

#include "gui_menuBase.h"

//*******************************
// struct OptionsInfo
//*******************************
struct OptionsInfo {
    int id;                                 // ex: CFG_THEME
    std::string descriptionToTranslate;     // ex: _("AutoBleem Theme:")
    std::string iniKey;                     // ex: "theme"
    bool keyIsBoolean;
    std::vector<std::string> options;

    OptionsInfo(int _id, std::string _descriptionToTranslate,
            std::string _iniKey, bool _keyIsBoolean, const std::vector<std::string> & _options = std::vector<std::string>())
        : id(_id), descriptionToTranslate(_descriptionToTranslate),
        iniKey(_iniKey), keyIsBoolean(_keyIsBoolean), options(_options) {}
};

//*******************************
// class GuiOptionsMenuBase
//*******************************
class GuiOptionsMenuBase : public GuiMenuBase<OptionsInfo> {
public:
    GuiOptionsMenuBase(SDL_Shared<SDL_Renderer> _renderer) : GuiMenuBase(_renderer) {}
    virtual void init();

    std::shared_ptr<Lang> lang;

    std::string getLineText(const OptionsInfo& info) {
        std::string temp = lang->translate(info.descriptionToTranslate) + " ";
        auto value = gui->cfg.inifile.values[info.iniKey];
        if (info.keyIsBoolean) {
            // append the off/on or on/off icon
            if (info.options[0] == "true")   // if the boolean is reversed
                if (value == "true") temp += "|@Uncheck|"; else temp += "|@Check|";
            else
                if (value == "true") temp += "|@Check|"; else temp += "|@Uncheck|";
        }
        else
            temp += value;  // append the current text value in the config.ini
        return temp;
    }

    virtual void renderLineIndexOnRow(int index, int row);
    virtual int getVerticalSize() { return 0; }
};
