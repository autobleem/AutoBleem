#include "gui_optionsMenuBase.h"
#include "../../lang.h"

using namespace std;

//*******************************
// void GuiOptionsMenuBase::init()
//*******************************
void GuiOptionsMenuBase::init()
{
    GuiMenuBase<OptionsInfo>::init();
    lang = Lang::getInstance();
}

//*******************************
// void GuiOptionsMenuBase::getBooleanSymbolText
//*******************************
std::string GuiOptionsMenuBase::getBooleanSymbolText(const OptionsInfo& info, const std::string& value) {
    if (info.choices[0] == "true")
    {
        // the boolean is reversed
        if (value == "true")
            return "|@Uncheck|";
        else
            return "|@Check|";
    }
    else {
        // boolean is normal
        if (value == "true")
            return "|@Check|";
        else
            return "|@Uncheck|";
    }
}

//*******************************
// void GuiOptionsMenuBase::getLineText
//*******************************
std::string GuiOptionsMenuBase::getLineText(const OptionsInfo& info) {
    std::string temp = lang->translate(info.descriptionToTranslate) + " ";
    auto value = gui->cfg.inifile.values[info.iniKey];
    if (info.keyIsBoolean) {
        temp += getBooleanSymbolText(info, value);
    }
    else {
        temp += value;  // append the current text value in the options list
    }
    return temp;
}

//*******************************
// void GuiOptionsMenuBase::renderLineIndexOnRow()
//*******************************
void GuiOptionsMenuBase::renderLineIndexOnRow(int index, int row) {
    string line = getLineText(lines[index]);
    int height = gui->renderTextLineOptions(line, row, offset, POS_LEFT);
}

//*******************************
// void GuiOptionsMenuBase::getPrevNextOption()
//*******************************
std::string GuiOptionsMenuBase::getPrevNextOption(OptionsInfo& info, const std::string & current, bool next) {
    const vector<string>& list = info.choices;
    // find current position
    int pos = 0;
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == current) {
            pos = i;
            break;
        }
    }
    if (next) {
        pos++;
        if (pos >= list.size()) {
            pos = list.size() - 1;
        }
    } else {
        pos--;
        if (pos < 0) pos = 0;
    }

    return list[pos];
}

//*******************************
// void GuiOptionsMenuBase::doPrevNextOption()
//*******************************
string GuiOptionsMenuBase::doPrevNextOption(OptionsInfo& info, bool next) {
    string nextValue = getPrevNextOption(info, gui->cfg.inifile.values[info.iniKey], next);
    gui->cfg.inifile.values[info.iniKey] = nextValue;
    return nextValue;
}

//*******************************
// void GuiOptionsMenuBase::doPrevNextOption()
//*******************************
std::string GuiOptionsMenuBase::doPrevNextOption(bool next) {
    if (lines.size() > 0 && selected >= 0 && selected < lines.size())
        return doPrevNextOption(lines[selected], next);
    else
        return "";
}

//*******************************
// void GuiOptionsMenuBase::doOptionIndex()
//*******************************
string GuiOptionsMenuBase::doOptionIndex(uint index) {
    if (validSelectedIndex()) {
        auto & choices = lines[selected].choices;
        if (choices.size() > 0 && index >= 0 && index < choices.size()) {
            string nextValue = choices[index];
            gui->cfg.inifile.values[lines[selected].iniKey] = nextValue;
            return nextValue;
        } else
            return "";  // index is not within range
    }
    else
        return "";
}

//*******************************
// void GuiOptionsMenuBase::doFirstOption()
//*******************************
string GuiOptionsMenuBase::doFirstOption() {
    return doOptionIndex(0);
}

//*******************************
// void GuiOptionsMenuBase::doLastOption()
//*******************************
string GuiOptionsMenuBase::doLastOption() {
    if (validSelectedIndex()) {
        auto & choices = lines[selected].choices;
        if (choices.size() > 0) {
            return doOptionIndex(choices.size()-1);
        } else
            return "";  // index is not within range
    }
    else
        return "";
}
