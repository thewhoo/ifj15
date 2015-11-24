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

#ifndef ADTYPES_H
#define ADTYPES_H

#include "ilist.h"
#include "stack.h"

enum vars
{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_PSEUDO,
    TYPE_AUTO
};

typedef struct s_variable
{
    int var_type;
    char *name;
    int initialized;
    int constant;
    union
    {
        int i;
        double d;
        char *str;
    }data;
} TVariable;

typedef struct s_function
{
    char *name;
    int return_type;
    int defined;
    Tins_list *ins_list;
    struct hash_tab *local_tab;
    TStack *params_stack;
    TVariable *return_var;
} TFunction;


typedef struct token
{
    int type;
    char *data;

} TToken;

/**
  * @brief Create TVar representation for local/constant
  *        symbol table from token.
  *
  * For int/double/string value - create TVar named by string value, set
  *                               as initialized, value and type is set,
  *                               stored in const table
  *
  * @param *token ptr to token from lexer
  * @return TVariable new initialized TVar
  */
TVariable* token_to_const(TToken* token);

/**
 * @brief Creates TFunction representation for global symbol table from token.
 *
 * Sets funciton name.
 * Function "defined" is FALSE (no idea if it's * definition/declaration,
 *                               set it manually later)
 * Instruction list for function is initialized and ready for use.
 * Local symbol table is initialized and ready.
 * Params stack is initialized and ready.
 *
 * @param *token ptr to token from lexer.
 * @return ptr to TFunction type
 */
TFunction* token_to_function(TToken* token);
#endif //ADTYPES_H
