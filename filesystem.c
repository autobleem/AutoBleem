#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"
#include "filesystem.h"
#include "database.h"

t_game_data game_data[MAX_GAMES];
int games;
bool all_correct;

void process_game_ini(char * path) {
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int * fp = fopen(path, "r");
    if (fp == NULL) {
        return; // error opening file
    }

    game_data[games].game_ini_found = true;

    while ((read = getline(&line, &len, fp)) != -1) {
        trimwhitespace(line, strlen(line), line);
        if (strlen(line) == 0) {
            continue; // skip empty lines
        }

        char * linelc = malloc(strlen(line));
        linelc[0] = 0;
        strcpy(linelc, line);
        for (int i = 0; i < strlen(linelc); i++)
            linelc[i] = tolower(line[i]);
        if (strcmp(linelc, "[game]") == 0) {
            // ignore this
        } else {
            // this is something we need to process
            char * pos;
            // find "=" character;


            linelc[0] = 0;
            strcpy(linelc, line);
            for (int i = 0; i < strlen(linelc); i++)
                linelc[i] = tolower(line[i]);

            pos = strstr(linelc, "=");
            if (pos == NULL) {
                // this line is faulty
                continue;
            }
            pos[0] = 0; // remove it
            pos = strstr(line, "=");
            pos++;


            // fill data in
            if (strcmp(linelc, "title") == 0) {
                strcpy(game_data[games].title, pos);
                fprintf(stderr, "Game Title: %s\n", game_data[games].title);
            }

            if (strcmp(linelc, "publisher") == 0) {
                strcpy(game_data[games].publisher, pos);

            }

            if (strcmp(linelc, "players") == 0) {
                if (is_integer_name(pos)) {
                    game_data[games].players = atoi(pos);

                } else {
                    game_data[games].game_ini_valid = false;
                    return;
                }
            }

            if (strcmp(linelc, "year") == 0) {
                if (is_integer_name(pos)) {
                    game_data[games].year = atoi(pos);

                } else {
                    game_data[games].game_ini_valid = false;
                    return;
                }
            }

            if (strcmp(linelc, "discs") == 0) {

                char *pt;
                pt = strtok(pos, ",");
                while (pt != NULL) {
                    strcpy(game_data[games].discs[game_data[games].total_discs].diskname, pt);
                    game_data[games].total_discs++;
                    pt = strtok(NULL, ",");
                }


            }
        }
        free(linelc);

    }





    fclose(fp);
    if (line)
        free(line);

    // now validate if we have all data
    if (strlen(game_data[games].title) == 0) {
        sprintf(game_data[games].title, "Unknown Game %d", games + 1);
    }

    if (strlen(game_data[games].publisher) == 0) {
        sprintf(game_data[games].publisher, "BleemSync");
    }

    if (game_data[games].players == 0) {
        game_data[games].players = 1;
    }

    if (game_data[games].year == 0) {
        game_data[games].year = 2018;
    }

    if (game_data[games].total_discs == 0) {
        game_data[games].game_ini_valid = false;
        return; // can not continue without disc images
    }


    game_data[games].game_ini_valid = true;
}

bool final_validate() {
    bool result = true;

    t_game_data data = game_data[games];
    fprintf(stderr, "-----------------\n");
    fprintf(stderr, "Final validation:\n");
    fprintf(stderr, "-----------------\n");
    fprintf(stderr, "Game folder id: %d\n", data.folder_id);
    fprintf(stderr, "Game:%s\n", data.title);
    fprintf(stderr, "Players:%d\n", data.players);
    fprintf(stderr, "Publisher:%s\n", data.publisher);
    fprintf(stderr, "Year:%d\n", data.year);
    fprintf(stderr, "GameData found:%d\n", data.game_data_found);
    if (!data.game_data_found) result = false;
    fprintf(stderr, "Game.ini found:%d\n", data.game_ini_found);
    if (!data.game_ini_found) result = false;
    fprintf(stderr, "Game.ini valid:%d\n", data.game_ini_valid);
    if (!data.game_ini_valid) result = false;
    fprintf(stderr, "PNG found:%d\n", data.image_found);
    if (!data.image_found) result = false;
    fprintf(stderr, "LIC found:%d\n", data.lic_found);
    if (!data.lic_found) result = false;
    fprintf(stderr, "pcsx.cfg found:%d\n", data.pcsx_cfg_found);
    if (!data.pcsx_cfg_found) result = false;
    fprintf(stderr, "TotalDiscs:%d\n", data.total_discs);
    for (int i = 0; i < data.total_discs; i++) {
        fprintf(stderr, "  Disc:%d name:%s\n", i, data.discs[i].diskname);
        if (strlen(data.discs[i].diskname) == 0) result = false;
        fprintf(stderr, "  CUE found: %d\n", data.discs[i].cue_found);
        if (!data.discs[i].cue_found) result = false;
        fprintf(stderr, "  BIN found: %d\n", data.discs[i].bin_found);
        if (!data.discs[i].bin_found) result = false;
    }

    //   data.game_ini_valid;
    if (result) {
        fprintf(stderr, "-------OK-------\n");
    } else {
        fprintf(stderr, "------FAIL------\n");
    }
    return result;
}

void read_and_validate(char * folder, char * path) {
    fprintf(stderr, "Processing folder: %s\n", folder);

    int id = atoi(folder);
    game_data[games].folder_id = id;
    char fullPath[2048];
    fullPath[0] = 0; // clear this
    strcat(fullPath, path);
    strcat(fullPath, separator());
    strcat(fullPath, folder);
    strcat(fullPath, separator());
    strcat(fullPath, "GameData");
    strcat(fullPath, separator());
    DIR *dir = opendir(fullPath);

    // check if gamedata is valid dir
    if (dir != NULL) {
        closedir(dir);
        game_data[games].game_data_found = true;
    } else {
        game_data[games].game_data_found = false;
        return; // no point to continue
    }
    // now try to read Game.ini
    char game_ini[2048];
    game_ini[0] = 0;
    strcat(game_ini, fullPath);
    strcat(game_ini, "Game.ini");


    process_game_ini(game_ini);

    if (!game_data[games].game_ini_valid) {
        // invalid game init 
        return;
    }

    // validate if discs cue/bin are available
    for (int i = 0; i < game_data[games].total_discs; i++) {
        DIR *dir = opendir(fullPath);
        if (dir != NULL) {
            struct dirent *entry = readdir(dir);
            while (entry != NULL) {

                if (strcicmp(entry->d_name, "pcsx.cfg") == 0) {
                    game_data[games].pcsx_cfg_found = true;

                }

                if (strlen(entry->d_name) > 4) {
                    const char *last_four = &entry->d_name[strlen(entry->d_name) - 4];

                    if (i == 0) {
                        if (strcicmp(".lic", last_four) == 0) {
                            game_data[games].lic_found = true;

                        }

                        if (strcicmp(".png", last_four) == 0) {
                            game_data[games].image_found = true;

                        }
                    }


                    if (strcicmp(".bin", last_four) == 0) {
                        // this is bin file compare to disc

                        if (strncmp(game_data[games].discs[i].diskname, entry->d_name, strlen(game_data[games].discs[i].diskname)) == 0) {

                            game_data[games].discs[i].bin_found = true;


                        }
                    }
                    if (strcicmp(".cue", last_four) == 0) {
                        // this is bin file compare to disc
                        if (strncmp(entry->d_name, game_data[games].discs[i].diskname, strlen(game_data[games].discs[i].diskname)) == 0) {
                            game_data[games].discs[i].cue_found = true;


                        }
                    }
                }
                entry = readdir(dir);
            }
        }




        if (dir != NULL) {
            closedir(dir);
        }

    }
    char cwd[1024];
    if (getcwd(cwd, sizeof (cwd)) != NULL) {

    }

    char png_path[1024];
    png_path[0] = 0;
    strcpy(png_path, cwd);
    strcpy(png_path, separator);
    strcpy(png_path, "default.png");
    char lic_path[1024];
    lic_path[0] = 0;
    strcpy(lic_path, cwd);
    strcpy(lic_path, separator);
    strcpy(lic_path, "default.lic");
    char cfg_path[1024];
    cfg_path[0] = 0;
    strcpy(cfg_path, cwd);
    strcpy(cfg_path, separator);
    strcpy(cfg_path, "pcsx.cfg");



    // recover missing files
    if (!game_data[games].pcsx_cfg_found) {
        // copy default
        char pcsx_cfg[2048];
        pcsx_cfg[0] = 0;
        strcat(pcsx_cfg, fullPath);
        strcat(pcsx_cfg, "pcsx.cfg");
        copy_file(cfg_path, pcsx_cfg);
        game_data[games].pcsx_cfg_found = true;
        fprintf(stderr, "Copy default pcsx.cfg\n");

    }

    if (!game_data[games].image_found) {
        // copy default
        char png_file[2048];
        png_file[0] = 0;
        strcat(png_file, fullPath);
        strcat(png_file, game_data[games].discs[0].diskname);
        strcat(png_file, ".png");
        copy_file("default.png", png_file);
        game_data[games].image_found = true;
        fprintf(stderr, "Copy default image\n");

    }

    if (!game_data[games].lic_found) {
        // copy default
        char lic_file[2048];
        lic_file[0] = 0;
        strcat(lic_file, fullPath);
        strcat(lic_file, game_data[games].discs[0].diskname);
        strcat(lic_file, ".lic");
        copy_file("default.lic", lic_file);
        game_data[games].lic_found = true;
        fprintf(stderr, "Copy default licence\n");

    }

    game_data[games].game_correct = final_validate();
    
}

int cmpfunc (const void * a, const void * b) {
    t_game_data * aObj=a;
    t_game_data * bObj=b;
    
   return aObj->folder_id - bObj->folder_id;
}

int save_database(char * fileName)
{
    for (int i=0;i<games;i++)
    {
        t_game_data data = game_data[i];
        insert_game_record(fileName,data);
    }
}

int scan_directory_folders(char * path) {
    // init values
    all_correct = false; // update this before loading to db 
    games = 0;
    memset(&game_data, 0, sizeof game_data);

    fprintf(stderr, "Scanning and validating game data\n");
    DIR *dir = opendir(path);
    if (dir != NULL) {
        struct dirent *entry = readdir(dir);


        while (entry != NULL) {
            if (entry->d_type == DT_DIR)
                if (is_integer_name(entry->d_name)) {
                    // found folder with game
                    read_and_validate(entry->d_name, path);
                    games++;
                }


            entry = readdir(dir);
        }

        closedir(dir);
        
        // sort games by id;
        qsort(game_data, games, sizeof(t_game_data), cmpfunc);
        
        // validate all game id's sequential
        all_correct = true;
        fprintf(stderr,"Total games: %d\n\n",games);
        
        for (int i=0;i<games;i++)
        {
            
            if (i != game_data[i].folder_id-1 )
            {
                all_correct = false;
                fprintf(stderr,"Incorect sequence of folders\n\n");
                return -1;
            }
        }

        
        
        fprintf(stderr, "Everything looks ok\n");
    } else {
        fprintf(stderr, "Invalid directory\n");
    }
    return 0;
}