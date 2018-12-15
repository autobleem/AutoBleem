/* 
 * File:   database.h
 * Author: screemer
 *
 * Created on 11 dec 2018, 21:49
 */

#include <string>
#include "sqlite3.h"

#ifndef DATABASE_H
#define DATABASE_H

using namespace std;

typedef struct t_disc_entry {
    char diskname[1024];
    // verifications
    bool cue_found;
    bool bin_found;
} t_disc_entry;


typedef struct game_data {
    int folder_id;
    char title[1024];
    char publisher[256];
    int players;
    int year;
    // discs
    t_disc_entry discs[6];
    int total_discs;

    // verifications
    bool game_data_found;
    bool pcsx_cfg_found;
    bool game_ini_found;
    bool game_ini_valid;
    bool image_found;
    bool lic_found;
    bool game_correct;

} t_game_data;

class Database {
public:
    bool connect(string fileName);

    void disconnect();

    bool createInitialDatabase();

    bool insertGame(int id, string title, string publisher, int players, int year);

    bool insertDisc(int id, int discNum, string discName);

private:
    sqlite3 *db;

    bool executeCreateStatement(char *sql, string tableName);

    bool executeStatement(char *sql, string outMsg, string errorMsg);
};






#endif /* DATABASE_H */

