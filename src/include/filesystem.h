/* 
 * File:   filesystem.h
 * Author: screemer
 *
 * Created on 11 dec 2018, 21:49
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define MAX_GAMES 500


int scan_directory_folders(char * path);
int save_database(char * fileName);

#ifdef __cplusplus
}
#endif

#endif /* FILESYSTEM_H */

