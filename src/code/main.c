/* 
 * File:   main.c
 * Author: screemer
 *
 * Created on 11 Dec 2018, 20:37
 */

#include <stdio.h>
#include <stdlib.h>

#include "database.h"
#include "filesystem.h"
/*
 * 
 */
int main(int argc, char** argv) {

    if (argc<3)
    {
        fprintf(stderr, "USAGE: bleemsync dbfilename.db /path/to/games\n");
        return (-1);
    }
    if (create_database(argv[1])!=0)
    {
         fprintf(stderr, "Error creating db structure\n");
         return -1;
    };
    
    if (scan_directory_folders(argv[2])!=0)
    {
         fprintf(stderr, "Error scanning folders\n");
         return -1;
    }
    
    save_database(argv[1]);
    
    return (EXIT_SUCCESS);
}

