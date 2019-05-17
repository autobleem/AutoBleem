//
// Created by screemer on 2018-12-16.
//
#pragma once
#include "../main.h"

//******************
// Metadata
//******************
class Metadata {
public:
    std::string title;
    std::string publisher;
    int year;
    int players;
    char *bytes = NULL;
    int dataSize = 0;
    bool valid = false;

    std::string lastRegion="U";

    bool lookupBySerial(std::string serial);
    bool lookupByTitle(std::string title);
    void clean()
    {
        if (bytes!=NULL)
        {
            free(bytes);
            bytes=NULL;
        }
    }
};
