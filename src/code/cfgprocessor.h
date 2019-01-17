//
// Created by screemer on 2019-01-15.
//

#ifndef CBLEEMSYNC_CFGPROCESSOR_H
#define CBLEEMSYNC_CFGPROCESSOR_H

#include "config.h"
class CfgProcessor {
public:
    Config config;
    void process(string source, string destination, int region, bool japan, int soundFilter, bool highres, int clock);
};


#endif //CBLEEMSYNC_CFGPROCESSOR_H
