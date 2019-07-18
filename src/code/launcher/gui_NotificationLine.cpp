#include "gui_NotificationLine.h"
#include "gui_launcher.h"

using namespace std;

//*******************************
// NotificationLine::setText
//*******************************
void NotificationLine::setText(string _text, long _timeLimitInMilliSeconds, const SDL_Color & _textColor, FontSize _fontSize) {
    text = _text;
    timed = (_timeLimitInMilliSeconds != 0);
    notificationTime = SDL_GetTicks();  // tick count when setText called
    if (notificationTime == 0)  // if by chance it's 0.  0 flags that the timeLimit has been reached and to turn off the display
        ++notificationTime;
    timeLimit = _timeLimitInMilliSeconds;
    textColor = _textColor;
    fontSize = _fontSize;
};

//*******************************
// NotificationLine::setText
//*******************************
void NotificationLine::setText(string _text, long _timeLimitInMilliSeconds) {
    setText(_text, _timeLimitInMilliSeconds, textColor, fontSize);
};

//*******************************
// NotificationLine::tickTock
//*******************************
void NotificationLine::tickTock() {
    auto gui = Gui::getInstance();

    if (timed) {
        if (notificationTime != 0) {
            long currentTimeTicks = SDL_GetTicks();
            if (currentTimeTicks - notificationTime > timeLimit) // if time limit reached
                notificationTime = 0;   // turn off the display
        }
        if (notificationTime != 0)
            GuiLauncher::renderText(x, y, text, textColor, gui->getFont(fontSize), true, true);
    } else // not timed - keep display on
        GuiLauncher::renderText(x, y, text, textColor, gui->getFont(fontSize), true, true);
}

//*******************************
// NotificationLines::createAndSetDefaults
//*******************************
void NotificationLines::createAndSetDefaults(int count, int x_start, int y_start, FontSize fontSize, int fontHeight, int separationBetweenLines) {
    for (int line=0; line < count; ++line) {
        NotificationLine notificationLine;
        notificationLine.fontSize = fontSize;
        notificationLine.textColor = brightWhite;
        notificationLine.x = x_start;
        notificationLine.y = y_start + (line * (fontHeight + separationBetweenLines));
        notificationLine.timed = true;
        notificationLine.timeLimit = DefaultShowingTimeout;

        lines.push_back(notificationLine);
    }
}

//*******************************
// NotificationLines::tickTock
//*******************************
void NotificationLines::tickTock() {
    for (auto & line : lines)
        line.tickTock();
}
