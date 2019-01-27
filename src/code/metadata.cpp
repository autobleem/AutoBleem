//
// Created by screemer on 2018-12-16.
//

#include "metadata.h"
#include "database.h"

static const char *jDatabases[] = {"../db/coversU.db", "../db/coversP.db", "../db/coversJ.db"};

bool Metadata::lookup(string serial) {
    for (int i = 0; i < 3; i++) {
        Database *db = new Database();
        if (db->connect(jDatabases[i])) {
            if (db->querySerial(serial, this)) {
                db->disconnect();
                delete(db);
                switch(i)
                {
                    case 0:
                        this->lastRegion="U";
                        break;
                    case 1:
                        this->lastRegion="P";
                        break;
                    case 2:
                        this->lastRegion="J";
                        break;
                    default:
                        this->lastRegion="U";
                        break;
                }
                return true;
            }
        };
        db->disconnect();
        delete(db);
    }

    return false;
}

bool Metadata::lookup2(string title) {
    for (int i = 0; i < 3; i++) {
        Database *db = new Database();
        if (db->connect(jDatabases[i])) {
            if (db->queryTitle(title, this)) {
                db->disconnect();
                delete(db);
                switch(i)
                {
                    case 0:
                        this->lastRegion="U";
                        break;
                    case 1:
                        this->lastRegion="P";
                        break;
                    case 2:
                        this->lastRegion="J";
                        break;
                    default:
                        this->lastRegion="U";
                        break;
                }
                return true;
            }
        };
        db->disconnect();
        delete(db);
    }

    return false;
}