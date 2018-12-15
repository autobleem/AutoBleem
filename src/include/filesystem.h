/* 
 * File:   filesystem.h
 * Author: screemer
 *
 * Created on 11 dec 2018, 21:49
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "database.h"
    
#define MAX_GAMES 500


int scan_directory_folders(char * path);

int save_database(Database *db);



#endif /* FILESYSTEM_H */

