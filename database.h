/* 
 * File:   database.h
 * Author: screemer
 *
 * Created on 11 dec 2018, 21:49
 */

#ifndef DATABASE_H
#define DATABASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite3.h"
    


int create_database(char * fileName);




#ifdef __cplusplus
}
#endif

#endif /* DATABASE_H */

