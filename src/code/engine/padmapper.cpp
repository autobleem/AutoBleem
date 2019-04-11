//
// Created by screemer on 2019-03-21.
//

#include "padmapper.h"
#include "../util.h"

#define DIR_UP    1
#define DIR_DOWN  2
#define DIR_LEFT  3
#define DIR_RIGHT 4
#define DIR_NONE  5


void PadMapper::init() {
    buttonNames.clear();
    buttonNames[PCS_BTN_CIRCLE] = "circle";
    buttonNames[PCS_BTN_SQUARE] = "square";
    buttonNames[PCS_BTN_TRIANGLE] = "triangle";
    buttonNames[PCS_BTN_CROSS] = "cross";
    buttonNames[PCS_BTN_START] = "start";
    buttonNames[PCS_BTN_SELECT] = "select";
    buttonNames[PCS_BTN_L1] = "l1";
    buttonNames[PCS_BTN_L2] = "l2";
    buttonNames[PCS_BTN_R1] = "r1";
    buttonNames[PCS_BTN_R2] = "r2";

    defaultConfig = new Inifile();
    defaultConfig->values["cross"]    ="2";
    defaultConfig->values["circle"]   ="1";
    defaultConfig->values["square"]   ="3";
    defaultConfig->values["triangle"] ="0";
    defaultConfig->values["start"]    ="9";
    defaultConfig->values["select"]    ="8";
    defaultConfig->values["l1"]        ="6";
    defaultConfig->values["l2"]    ="4";
    defaultConfig->values["r1"]    ="7";
    defaultConfig->values["r2"]    ="5";
    defaultConfig->values["dpad"]    ="analogue";
    defaultConfig->values["analogue1"]    ="na";
    defaultConfig->values["analogue2"]    ="na";
    defaultConfig->values["dpaddeadzone"]    ="32000";

    configs.clear();
    // now load all mappings
    string path = Util::getWorkingPath() + "/gpmapping";
    for (DirEntry entry:Util::diru(path)) {
        if (Util::getFileExtension(entry.name) == "ini") {

            Inifile *mapping = new Inifile();
            mapping->load(path + "/" + entry.name);
            cout << "New Pad Config: " << mapping->section << endl;
            configs[mapping->section] = mapping;
        }
    }


}

bool PadMapper::isDirection(SDL_Event *e,  int dir) {
    // find pad name

   SDL_JoystickID id;

    if (e->type==SDL_JOYHATMOTION)
    {
        id = e->jhat.which;
    } else
    {
        id =  e->jaxis.which;
    }
    SDL_Joystick * joy = SDL_JoystickFromInstanceID(id);
    const char* joyname = SDL_JoystickName(joy);

    Inifile *config = configs[joyname];
    if (config == nullptr) {
       config = defaultConfig;
    }

    int deadZone = atoi(config->values["dpaddeadzone"].c_str());

    int axis;
    if ((dir == DIR_DOWN) || (dir == DIR_UP)) axis = 1;
    else axis = 0;

    if (config->values["dpad"] == "analogue") {

        if (dir!=DIR_NONE) {
            if (e->type == SDL_JOYAXISMOTION) {
                if (e->jaxis.axis == axis) {
                    if ((dir == DIR_UP) || (dir == DIR_LEFT)) {
                        return (e->jaxis.value < -deadZone);
                    } else {
                        return (e->jaxis.value > deadZone);
                    }
                }
            }
        } else
        {
            return (e->jaxis.value>=-deadZone) && (e->jaxis.value<=deadZone);
        }
    } else {
        if (e->type == SDL_JOYHATMOTION) {
            switch (dir)
            {
                case DIR_UP:
                    return (e->jhat.value == SDL_HAT_UP);
                case DIR_DOWN:
                    return (e->jhat.value == SDL_HAT_DOWN);
                case DIR_LEFT:
                    return (e->jhat.value == SDL_HAT_LEFT);
                case DIR_RIGHT:
                    return (e->jhat.value == SDL_HAT_RIGHT);
                case DIR_NONE:
                    return (e->jhat.value == SDL_HAT_CENTERED);
            }


        }
    }
    return false;
}

bool PadMapper::isCenter(SDL_Event *event)
{
    return isDirection(event, DIR_NONE);
}
bool PadMapper::isUp(SDL_Event *event) {
   return isDirection(event,DIR_UP);
}

bool PadMapper::isDown(SDL_Event *event)
{
    return isDirection(event,DIR_DOWN);
}

bool PadMapper::isLeft(SDL_Event *event)
{
    return isDirection(event,DIR_LEFT);
}
bool PadMapper::isRight(SDL_Event *event)
{
    return isDirection(event,DIR_RIGHT);
}

int PadMapper::translateButton(int button, SDL_Event *e) {
    SDL_Joystick * joy = SDL_JoystickFromInstanceID(e->jbutton.which);
    const char* joyname = SDL_JoystickName(joy);
    string buttonName = buttonNames[button];
    // fetch mapping
    Inifile *config = configs[joyname];
    if (config == nullptr) {
        config = defaultConfig;
    }
    int newBtnCode = atoi(config->values[buttonName].c_str());
    return newBtnCode;
}