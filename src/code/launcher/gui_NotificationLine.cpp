#include "gui_NotificationLine.h"
#include "gui_launcher.h"

using namespace std;

//*******************************
// NotificationLine::setText
//*******************************
void NotificationLine::setText(string _text, bool _timed, long _timeLimit, const SDL_Color & _textColor, TTF_Font *_font) {
    text = _text;
    timed = _timed;
    notificationTime = SDL_GetTicks();  // tick count when setText called
    if (notificationTime == 0)  // if by chance it's 0.  0 flags that the timeLimit has been reached and to turn off the display
        ++notificationTime;
    timeLimit = _timeLimit;
    textColor = _textColor;
    font = _font;
};

//*******************************
// NotificationLine::setText
//*******************************
void NotificationLine::setText(string _text, bool _timed, long _timeLimit) {
    setText(_text, _timed, _timeLimit, textColor, font);
};

//*******************************
// NotificationLine::tickTock
//*******************************
void NotificationLine::tickTock(SDL_Renderer * renderer) {
    if (timed) {
        if (notificationTime != 0) {
            long currentTimeTicks = SDL_GetTicks();
            if (currentTimeTicks - notificationTime > timeLimit) // if time limit reached
                notificationTime = 0;   // turn off the display
        }
        if (notificationTime != 0)
            GuiLauncher::renderText(renderer, x, y, text, textColor, font, true, true);
    } else // not timed - keep display on
        GuiLauncher::renderText(renderer, x, y, text, textColor, font, true, true);
}

//*******************************
// NotificationLines::createAndSetDefaults
//*******************************
void NotificationLines::createAndSetDefaults(int count, int x_start, int y_start, TTF_Font * font, int fontHeight, int separationBetweenLines) {
    for (int line=0; line < count; ++line) {
        NotificationLine notificationLine;
        notificationLine.font = font;
        notificationLine.textColor = brightWhite;
        notificationLine.x = x_start;
        notificationLine.y = y_start + (line * (fontHeight + separationBetweenLines));
        if (line == 0)
            notificationLine.timed = false; // keep line 0 displayed
        else {
            notificationLine.timed = true;
            notificationLine.timeLimit = DefaultShowingTimeout;
        }
        lines.push_back(notificationLine);
    }
}

//*******************************
// NotificationLines::tickTock
//*******************************
void NotificationLines::tickTock(SDL_Renderer * renderer) {
    for (auto & line : lines)
        line.tickTock(renderer);
}
