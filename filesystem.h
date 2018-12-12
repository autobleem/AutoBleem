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
    
#ifndef _BOOL
typedef int bool;
#define true 1
#define false 0
#define _BOOL
#endif
    
typedef struct t_disc_entry
{
    char diskname[1024];
    // verifications
    bool cue_found;
    bool bin_found;
} t_disc_entry;

typedef struct game_data
{
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


int scan_directory_folders(char * path);
int save_database(char * fileName);

#ifdef __cplusplus
}
#endif

#endif /* FILESYSTEM_H */

