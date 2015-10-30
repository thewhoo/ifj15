/*
 * Course IFJ @ FIT VUT Brno, 2015
 * IFJ15 Interpreter Project
 *
 * Authors:
 * Lukas Osadsky  - xosads00
 * Pavol Plaskon  - xplask00
 * Pavel Pospisil - xpospi88
 * Matej Postolka - xposto02
 *
 * Unless otherwise stated, all code is licensed under a
 * Creative Commons Attribution-ShareAlike 4.0 International License
 *
 */

#include <string.h>

#include "error.h"
#include "adt.h"

int length(TVariable *str)
{
    int size = 0;

    if(str->var_type != TYPE_STRING)
        exit_error(E_SEMANTIC_TYPES);

    size = strlen(str->data.str);
    return size;
}

char* concat(TVariable* str1, TVariable* str2)
{
    if(str1->var_type != TYPE_STRING || str2->var_type != TYPE_STRING)
        exit_error(E_SEMANTIC_TYPES);

    char* s1 = str1->data.str;
    char* s2 = str2->data.str;
    char* new_str = gmalloc(strlen(s1) + strlen(s2) + 1);
    
    strcpy(new_str, s1);
    strcat(new_str, s2);

    return new_str;
}
