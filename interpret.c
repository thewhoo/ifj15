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
#include <stdio.h>

#include "interpret.h"
#include "galloc.h"
#include "builtin.h"
#include "error.h"
#include "adt.h"
#include "ial.h"
#include "stack.h"


TStack *gStack;
TStack *active_frame;

TVariable* get_var(char *address)
{
    htab_item *item = NULL;
    // what abou constants???
    for(int i = active_frame->used - 1; i>=0; i--)
    {
        item = htab_lookup((htab_t *)active_frame->data[i], address);
        if(item != NULL)
            return item->data.variable;
    }
    
    fprintf(stderr,"Var not found, should not happen!\n");
    exit_error(99);
    return NULL;
}

void math_ins(char type, TVariable *dest, TVariable *src1, TVariable *src2)
{
    double a, b;

    if(!src1->initialized || !src2->initialized)
        exit_error(E_UNINITIALIZED);

    if(src1->var_type == TYPE_DOUBLE)
        a = src1->data.d;
    else
        a = src1->data.i;

    if(src2->var_type == TYPE_DOUBLE)
        b = src2->data.d;
    else
        b = src2->data.i;


    switch(type)
    {
        case('+'):
            if(dest->var_type == TYPE_DOUBLE)
                dest->data.d = a+b;
            else
                dest->data.i = a+b;       
            break;
        case('-'):
            if(dest->var_type == TYPE_DOUBLE)
                dest->data.d = a-b;
            else
                dest->data.i = a-b;        
            break;
        case('*'):
            if(dest->var_type == TYPE_DOUBLE)
                dest->data.d = a*b;
            else
                dest->data.i = a*b;       
            break;
        case('/'):
            if(b == 0)
                exit_error(E_ZERO_DIVISION);

            if(dest->var_type == TYPE_DOUBLE)
                dest->data.d = a/b;
            else
                dest->data.i = a/b;       
            break;
    }
    dest->initialized = 1;
}

void interpret_loop(Tins_list *ins_list)
{
    int ret_int;
    char* ret_str;
    char* str;
    TVariable *var1, *var2, *var3;

    TList_item *ins = ins_list->first;
    active_frame = stack_top(gStack);

    while(ins != NULL)
    {
        switch(ins->ins_type)
        {
            //math
            case(INS_ADD):
                math_ins('+', get_var(ins->addr1), get_var(ins->addr2),
                        get_var(ins->addr3));
                break;
            case(INS_SUB):
                math_ins('-', get_var(ins->addr1), get_var(ins->addr2),
                        get_var(ins->addr3));
                break;
            case(INS_MUL):
                math_ins('*', get_var(ins->addr1), get_var(ins->addr2),
                        get_var(ins->addr3));
                break;
            case(INS_DIV):
                math_ins('/', get_var(ins->addr1), get_var(ins->addr2),
                        get_var(ins->addr3));
                break;
            //built-in
            case(INS_LENGTH):
                //CHECK RETURN VAR TYPE? 
                var2 = get_var(ins->addr2);
                if(!var2->initialized)
                    exit_error(E_UNINITIALIZED);
                ret_int = length(var2);
                var1 = get_var(ins->addr1);
                var1->var_type = TYPE_INT;
                var1->data.i = ret_int;
                var1->initialized = 1;
                break;
            case(INS_SUBSTR):
                //3 agrumenty, 1 return..neda sa ako 1 instrukcia..hm.. 
                break;
            case(INS_CONCAT):
                var2 = get_var(ins->addr2);
                var3 = get_var(ins->addr3);
                if(!var2->initialized || !var3->initialized)
                    exit_error(E_UNINITIALIZED);
                ret_str = concat(var2, var3);
                var1 = get_var(ins->addr1);
                var1->var_type = TYPE_STRING;
                var1->data.str = ret_str;
                var1->initialized = 1;
                break;
            case(INS_FIND):
                var2 = get_var(ins->addr2);
                var3 = get_var(ins->addr3);
                if(!var2->initialized || !var3->initialized)
                    exit_error(E_UNINITIALIZED);
                ret_int = find(var2, var3);
                var1 = get_var(ins->addr1);
                var1->var_type = TYPE_INT;
                var1->data.i = ret_int;
                var1->initialized = 1;
                break;
            case(INS_SORT):
                //create copy of string, sort do not affect original string
                var2 = get_var(ins->addr2);
                if(var2->var_type != TYPE_STRING)
                    exit_error(E_SEMANTIC_TYPES);
                
                str = gmalloc(strlen(var2->data.str) + 1);
                strcpy(str, var2->data.str);
                ret_str = sort(str);
                gfree(str);
                var1 = get_var(ins->addr1);
                var1->var_type = TYPE_STRING;
                var1->data.str = ret_str;
                var1->initialized = 1;
                break;
            case(INS_CIN):
                cin(get_var(ins->addr1));
                //ADD CIN FOT INT AND DOUBLE"""""""""""""""""""""""
               break;
            case(INS_COUT):
                var1 = get_var(ins->addr1);
                if(!var1->initialized)
                    exit_error(E_UNINITIALIZED);
                cout(var1);
                break;           

        }
        ins = ins->next;
        if(ins == NULL)
            exit_error(E_RUNTIME_OTHERS);
    }
}

void interpret()
{

htab_t *global_tab = NULL;
    //init global stack for interpret
    gStack = stack_init();    
    //find main function in global symbol table
    htab_item *func_main = htab_lookup(global_tab, "main");
    if(func_main == NULL)
        exit_error(3);
    
    //copy of main symbol table
    htab_t *main_tab = htab_copy(func_main->data.function->local_tab);
    TStack *func_main_frame = stack_init();
    stack_push(func_main_frame, main_tab);
    

    stack_push(gStack, func_main_frame);
    
    interpret_loop(func_main->data.function->ins_list);
}
