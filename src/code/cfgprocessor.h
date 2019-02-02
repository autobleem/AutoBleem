//
// Created by screemer on 2019-01-15.
//

#ifndef CBLEEMSYNC_CFGPROCESSOR_H
#define CBLEEMSYNC_CFGPROCESSOR_H

#include "config.h"
class CfgProcessor {
public:
    Config config;
    void replace(string entry, string gamePath,  string property, string newline);

private:
    void replaceInternal(string filePath, string property, string newline);


};


#endif //CBLEEMSYNC_CFGPROCESSOR_H
