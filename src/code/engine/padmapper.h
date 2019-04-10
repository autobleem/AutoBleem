//
// Created by screemer on 2019-03-21.
//

#ifndef AUTOBLEEM_GUI_PADMAPPER_H
#define AUTOBLEEM_GUI_PADMAPPER_H

#include "../main.h"
#include "inifile.h"


using namespace std;

#define PCS_BTN_L2       4
#define PCS_BTN_R2       5
#define PCS_BTN_L1       6
#define PCS_BTN_R1       7
#define PCS_BTN_START    9
#define PCS_BTN_SQUARE   3
#define PCS_BTN_TRIANGLE 0
#define PCS_BTN_CROSS    2
#define PCS_BTN_CIRCLE   1
#define PCS_BTN_SELECT   8

class PadMapper {
public:
    // loads all mapping files
    void init();

    map<string,Inifile*> configs;
    map<int, string> buttonNames;
    int translateButton(int button, string padname);






};


#endif //AUTOBLEEM_GUI_PADMAPPER_H
