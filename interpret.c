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

#include "interpret.h"
#include "error.h"
#include "adt.h"
#include "ial.h"

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
    
    fprint(stderr,"Var not found, should not happen!\n");
    exit_error(99);
}

void math_ins(char type, TVariable *dest, TVariable *src1, TVariable *src2)
{
    double a, b;

    if(!src1->initialized || !src2->initialized)
        exit_error(E_UNITIALIZED);

    if(src1->var_type == TYPE_DOUBLE)
        a = src1->data.d;
    else
        a = src1->data.i;

    if(src2->var_type == TYPE_DOUBLE)
        b = src2->data.d;
    else
        b = src2->data.i;


    switch(type):
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

    TList_item *ins;
    active_frame = stack_top(gStack);

    while(ins != NULL)
    {
        switch(ins->ins_type):
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
                ret_int = length(get_var(ins->addr2));
                tmp_var = get_var(ins->addr1);
                tmp_var->var_type = TYPE_INT;
                tmp_var->data.i = ret_int;
                tmp_var->initialized = 1;
                break;
            case(INS_SUBSTR):
                
                break;
            case(INS_CONCAT):
                ret_str = concat(get_var(ins->addr2), get_var(ins->addr3));
                tmp_var = get_var(ins->addr1)
                tmp_var->var_type = TYPE_STRING;
                tmp_var->data.str = ret_str;
                tmp_var->initialized = 1;
                break;
            case(INS_FIND):
                ret_int = find(get_var(ins->addr2), get_var(ins->addr3));
                tmp_var = get_var(ins->addr1);
                tmp_var->var_type = TYPE_INT;
                tmp_var->data.i = ret_int;
                tmp_var->initialized = 1;
                break;
            case(INS_SORT):

                break;
            case(INS_CIN):

                break;
            case(INS_COUT):

                break;           

        }
    }
}

void interpret()
{
    // expect ptr to global symb table
    htab_t *global_tab;

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
    

    stack_push(&gStack, func_main_frame);
    
    interpret_loop(func_main->ins_list)
}
