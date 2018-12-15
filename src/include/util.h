/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   util.h
 * Author: screemer
 *
 * Created on 11 grudnia 2018, 23:12
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cstdlib>

class Util {
public:
    static const char *separator();

    static bool is_integer_name(char *input);

    static size_t trimwhitespace(char *out, size_t len, const char *str);

    static int strcicmp(char const *a, char const *b);

    static int copy_file(char *old_filename, char *new_filename);

    static int strpos(char *haystack, char *needle);

    static void strlower(char *dest, char *src);

    static void clearstr(char *str);
};




#endif /* UTIL_H */

