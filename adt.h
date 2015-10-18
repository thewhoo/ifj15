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

/**
  * @file types.h
  * @brief abstract data types for interpret
  *
  */

#ifndef TYPES_H
#define TYPES_H

enum vars
{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE STRING
};

typedef struct var
{
    int var_type;
    char *name;
    int initialized;
    union 
    {
        int i;
        double d;
        char *str;
    }data;
}TVar;

typedef struct func
{
    char *name;
    int return_type;
    int defined;   
//something for arguments..list? struct? 
//ptr to local symbol table?
}TFunction;


typedef struct token
{
    int type;
    char *data;

}TToken;

#endif //TYPES_H
