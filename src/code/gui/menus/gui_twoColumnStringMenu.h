#pragma once

#include "gui_menuBase.h"


//*******************************
// struct TwoColumnsOfText
//*******************************
struct TwoColumnsOfText {
    std::string line_L;
    std::string line_R;
    TwoColumnsOfText(std::string left, std::string right) : line_L(left), line_R(right) {}
};

//*******************************
// class GuiTwoColumnStringMenu
//*******************************
class GuiTwoColumnStringMenu : public GuiMenuBase<TwoColumnsOfText> {
public:
    GuiTwoColumnStringMenu(SDL_Shared<SDL_Renderer> _renderer) : GuiMenuBase(_renderer) {}

    int xoffset_L = 0;
    int xoffset_R = 500;

    virtual std::string getTitle() override { return GuiMenuBase::getTitle(); }
    virtual std::string getStatusLine() override { return GuiMenuBase::getStatusLine(); }

    virtual void renderLineIndexOnRow(int index, int row) override {
        gui->renderTextLineToColumns(lines[index].line_L, lines[index].line_R, xoffset_L, xoffset_R, row, offset, font);
    }
};
