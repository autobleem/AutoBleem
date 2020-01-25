#pragma once

#include "gui_menuBase.h"

//*******************************
// struct OptionsInfo
//*******************************
struct OptionsInfo {
    int id;                                 // ex: CFG_THEME
    std::string descriptionToTranslate;     // ex: _("AutoBleem Theme:")
    std::string iniKey;                     // ex: "theme"
    bool keyIsBoolean;                      // if it's false/true we substitute the switch icons
    std::vector<std::string> choices;

    OptionsInfo(int _id = 0, std::string _descriptionToTranslate = std::string(),
            std::string _iniKey = std::string(), bool _keyIsBoolean = false, const std::vector<std::string> & _choices = std::vector<std::string>())
        : id(_id), descriptionToTranslate(_descriptionToTranslate),
        iniKey(_iniKey), keyIsBoolean(_keyIsBoolean), choices(_choices) {}
};

//*******************************
// class GuiOptionsMenuBase
//*******************************
class GuiOptionsMenuBase : public GuiMenuBase<OptionsInfo> {
public:
    GuiOptionsMenuBase(SDL_Shared<SDL_Renderer> _renderer) : GuiMenuBase(_renderer) {}
    virtual void init();

    std::shared_ptr<Lang> lang;             // so we can translate OptionsInfo.descriptionToTranslate

    virtual std::string getBooleanSymbolText(const OptionsInfo& info, const std::string& value);
    virtual std::string getLineText(const OptionsInfo& info);

    virtual void renderLineIndexOnRow(int index, int row);

    bool validSelectedIndex();  // returns true if the selected line index is a valid index
    uint getChoicesSize();      // returns the number of choices on the selected line (0 if the selected index is invalid).

    virtual uint getCurrentOptionIndex(OptionsInfo& info, const std::string & current);
    virtual std::string getPrevNextOption(OptionsInfo& info, const std::string & current, bool next);
    virtual std::string doPrevNextOption(OptionsInfo& info, bool next);
    virtual std::string doPrevNextOption(bool next);

    virtual std::string doOptionIndex(uint index);
    virtual std::string doFirstOption();
    virtual std::string doLastOption();

    int computeAmountTomoveBy(uint totalSize);
    virtual void doL1_Pressed();
    virtual void doR1_Pressed();

    virtual void doL2_Pressed() { doFirstOption(); }
    virtual void doR2_Pressed() { doLastOption(); }
    virtual void doHome() { doFirstOption(); }
    virtual void doEnd() { doLastOption(); }

};
