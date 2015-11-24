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
#include <stdlib.h>

#include "adt.h"
#include "enums.h"
#include "galloc.h"
#include "ilist.h"
#include "ial.h"
#include "stack.h"
#include "shared.h"

//using prefix num_ / str_
#define PREFIX_LEN 4

TVariable* token_to_const(TToken *token)
{
    htab_item *tmp = NULL;

    if(token->type == TOKEN_STRING_VALUE)
        tmp = htab_lookup(G.g_constTabStr, token->data);
    else
        tmp = htab_lookup(G.g_constTabNum, token->data);
    
    if(tmp != NULL)
        return tmp->data.variable;


    TVariable* var = gmalloc(sizeof(TVariable));

    var->initialized = 1;
    var->constant = 1;
    var->name = token->data;

    if(token->type == TOKEN_STRING_VALUE)
    {
        var->var_type = TYPE_STRING;
        var->data.str = token->data;
        tmp = htab_insert(G.g_constTabStr, var->name);
    }
    else if(token->type == TOKEN_DOUBLE_VALUE)
    {
        var->var_type = TYPE_DOUBLE;
        var->data.d = strtod(token->data, NULL);
        tmp = htab_insert(G.g_constTabNum, var->name);
    }
    else if(token->type == TOKEN_INT_VALUE)
    {
        var->var_type = TYPE_INT;
        var->data.i = strtol(token->data, NULL, 10);
        tmp = htab_insert(G.g_constTabNum, var->name);
    }
    
    tmp->data.variable = var;

    return var;
}

TFunction* token_to_function(TToken *token)
{
    TFunction* func = gmalloc(sizeof(TFunction));

    func->name = gmalloc(strlen(token->data) + 1);
    strcpy(func->name, token->data);
    func->defined = 0;
    func->ins_list = list_init();
    func->local_tab = htab_init(HTAB_SIZE);
    func->params_stack = stack_init();


    return func;
}
