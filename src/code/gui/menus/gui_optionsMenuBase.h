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

    OptionsInfo(int _id, std::string _descriptionToTranslate, std::string _iniKey, bool _keyIsBoolean)
        : id(_id), descriptionToTranslate(_descriptionToTranslate), iniKey(_iniKey), keyIsBoolean(_keyIsBoolean) {}
};

//*******************************
// class GuiOptionsMenuBase
//*******************************
class GuiOptionsMenuBase : public GuiMenuBase<OptionsInfo> {
public:
    GuiOptionsMenuBase(SDL_Shared<SDL_Renderer> _renderer) : GuiMenuBase(_renderer) {}

    virtual int getVerticalSize() { return 0; }
};
