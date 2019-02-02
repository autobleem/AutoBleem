//
// Created by screemer on 2018-12-16.
//

#include "main.h"

using namespace std;

#ifndef CBLEEMSYNC_METADATA_H
#define CBLEEMSYNC_METADATA_H


class Metadata {
public:
    string title;
    string publisher;
    int year;
    int players;
    char *bytes = NULL;
    int dataSize = 0;
    bool valid = false;

    string lastRegion="U";

    bool lookupBySerial(string serial);
    bool lookupByTitle(string title);
    void clean()
    {
        if (bytes!=NULL)
        {
            free(bytes);
            bytes=NULL;
        }
    }

};


#endif //CBLEEMSYNC_METADATA_H
