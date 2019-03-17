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
    void patchHLEbios(string entry, string gamePath);
    string getValue(string entry, string gamePath, string property,bool internal);

private:
    void replaceInternal(string filePath, string property, string newline);


};


#endif //CBLEEMSYNC_CFGPROCESSOR_H
