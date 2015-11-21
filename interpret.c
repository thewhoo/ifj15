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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adt.h"
#include "builtin.h"
#include "error.h"
#include "galloc.h"
#include "interpret.h"
#include "ial.h"
#include "stack.h"
#include "shared.h"

TStack *gStack;
TStack *active_frame;
TStack *fparams_stack;


TVariable* get_var(TVariable *var)
{
    if(var->constant)
        return var;

    htab_item *item = NULL;

    for(int i = active_frame->used - 1; i>=0; i--)
    {
        item = htab_lookup((htab_t *)active_frame->data[i], var->name);
        if(item != NULL)
            return item->data.variable;
    }

    fprintf(stderr,"Var not found, should not happen!\n");
    exit_error(99);
    return NULL;
}

void clean_active_frame()
{
    for(int i = active_frame->used - 1; i>=0; i--)
        htab_free((htab_t*)active_frame->data[i]);
    gfree(active_frame->data);
    gfree(active_frame);
}

void math_ins(char type, TVariable *dest, TVariable *src1, TVariable *src2)
{
    double a, b;
//Check if var is STRING????????
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

void compare_ins(int type, TVariable* dest, TVariable *src1, TVariable* src2)
{
    int result, a, b;

    if(!src1->initialized || !src2->initialized)
        exit_error(E_UNINITIALIZED);

    //Check for dest type?  double a = 3 > 4 ..but result of comapring must be
    //INT

    if(src1->var_type == TYPE_DOUBLE)
        a = src1->data.d;
    else
        a = src1->data.i;

    if(src2->var_type == TYPE_DOUBLE)
        b = src2->data.d;
    else
        b = src2->data.i;

    if((src1->var_type == TYPE_STRING) && (src2->var_type == TYPE_STRING))
    {
        result = strcmp(src1->data.str, src2->data.str);
        switch(type)
        {
            case(INS_EQ):
                dest->data.i = (result == 0) ? 1 : 0;
                break;
            case(INS_NEQ):
                dest->data.i = (result != 0) ? 1 : 0;
                break;
            case(INS_GREATER):
                dest->data.i = (result > 0) ? 1 : 0;
                break;
            case(INS_GREATEQ):
                dest->data.i = (result >= 0) ? 1 : 0;
                break;
            case(INS_LESSER):
                dest->data.i = (result < 0) ? 1 : 0;
                break;
            case(INS_LESSEQ):
                dest->data.i = (result <= 0) ? 1 : 0;
                break;
        }
    }
    else if((src1->var_type != TYPE_STRING) && (src2->var_type != TYPE_STRING))
    {
        switch(type)
        {
            case(INS_EQ):
                dest->data.i = (a == b) ? 1 : 0;
                break;
            case(INS_NEQ):
                dest->data.i = (a != b) ? 1 : 0;
                break;
            case(INS_GREATER):
                dest->data.i = (a > b) ? 1 : 0;
                break;
            case(INS_GREATEQ):
                dest->data.i = (a >= b) ? 1 : 0;
                break;
            case(INS_LESSER):
                dest->data.i = (a < b) ? 1 : 0;
                break;
            case(INS_LESSEQ):
                dest->data.i = (a <= b) ? 1 : 0;
                break;
        }
    }
    else
        exit_error(E_SEMANTIC_TYPES);

    dest->var_type = TYPE_INT;
    dest->initialized = 1;
}


void map_params(htab_t *tab, TStack* decl_params)
{
    if(decl_params->used != fparams_stack->used)
    {
        fprintf(stderr, "Wrong number of parameters!\n");
        exit_error(10);
    } //delete later

    htab_item *param;
    TVariable *dest, *src;

    for(int i=0; i<decl_params->used; i++)
    {
        param = htab_lookup(tab, ((TVariable*) decl_params->data[i])->name);
        dest = param->data.variable;
        src = fparams_stack->data[i];
        if(dest->var_type == TYPE_INT)
        {
            if(src->var_type == TYPE_DOUBLE)
                dest->data.i = src->data.d;
            else
                dest->data.i = src->data.i;
        }
        else if (dest->var_type == TYPE_DOUBLE)
        {
            if(src->var_type == TYPE_DOUBLE)
                dest->data.d = src->data.d;
            else
                dest->data.d = src->data.i;
        }
        else
            dest->data.str = src->data.str;
        dest->initialized = 1; 
    }

    stack_clear(fparams_stack);
}

#ifdef DEBUG_MODE
void print_instructions(TList_item *ins)
{
    TList_item *tmp = ins;
    printf("INTERPRET: intruction list:\n");
    while(tmp != NULL)
    {
        printf("%d\n", tmp->ins_type);
        tmp = tmp->next;
    }
    printf("End of list\n");
}
#endif

void interpret_loop(Tins_list *ins_list)
{
    int ret_int;
    char* ret_str;
    char* str;
    TVariable *var1, *var2, *var3;
    htab_item *func;
    htab_t *new_tab = NULL;

    TList_item *ins = ins_list->first;
    
    #ifdef DEBUG_MODE
    print_instructions(ins);
    #endif

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

            case(INS_EQ):
            case(INS_NEQ):
            case(INS_GREATER):
            case(INS_GREATEQ):
            case(INS_LESSER):
            case(INS_LESSEQ):
                compare_ins(ins->ins_type, get_var(ins->addr1),
                            get_var(ins->addr2), get_var(ins->addr3));
                break;
            case(INS_PUSH_TAB):
                new_tab = htab_copy(ins->addr1);
                stack_push(active_frame, new_tab);
                break;

            case(INS_POP_TAB):
                htab_free((htab_t*)stack_top(active_frame));
                stack_pop(active_frame);
                break;

            case(INS_JMP):
                ins = (TList_item *) ins->addr1;
                break;

            case(INS_CJMP):
                var1 = get_var(ins->addr1);
                if(!var1->data.i)
                    ins = (TList_item *) ins->addr2;
                break;

            case(INS_LAB):
                break;

            case(INS_PUSH):
                var1 = get_var(ins->addr1);
                if(!var1->initialized)
                    exit_error(E_UNINITIALIZED);
                stack_push(fparams_stack, var1);
                break;

            case(INS_CALL):
                stack_push(gStack, active_frame);
                stack_push(gStack, ins);

                func = (htab_item*)ins->addr1;
                ins = func->data.function->ins_list->first;
                #ifdef DEBUG_MODE
                print_instructions(ins);
                #endif
                active_frame = stack_init();
                new_tab = htab_copy(func->data.function->local_tab);
                stack_push(active_frame, new_tab);
                map_params(new_tab, func->data.function->params_stack);
                //map pushed f arguments to f parameters
                continue; //after break, continue with new isntr, we want to
                          //begin with first one
            case(INS_RET):
                if(stack_empty(gStack))  //end of main func
                {
                    gcDestroy();
                    exit(0);            //maybe some cleaning?
                }

                clean_active_frame();
                ins = (TList_item*) stack_top(gStack);
                stack_pop(gStack);
                active_frame = (TStack*) stack_top(gStack);
                stack_pop(gStack);
                //returned value assigned by INS_ASSIGN from "return" var?
                break;

            case(INS_ASSIGN):
                var1 = get_var(ins->addr1);
                var2 = get_var(ins->addr2);
                if(!var2->initialized)
                    exit_error(E_UNINITIALIZED);
                if(var1->var_type == TYPE_INT)
                {
                    if(var2->var_type == TYPE_DOUBLE)
                        var1->data.i = var2->data.d;
                    else
                        var1->data.i = var2->data.i;
                }
                else if (var1->var_type == TYPE_DOUBLE)
                {
                    if(var2->var_type == TYPE_DOUBLE)
                        var1->data.d = var2->data.d;
                    else
                        var1->data.d = var2->data.i;
                }
                else
                    var1->data.str = var2->data.str;

                var1->initialized = 1;
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
        {
            exit_error(E_UNINITIALIZED); //missing return at the end of function
        }

    }
}

void interpret()
{

    //init global stack for interpret
    gStack = stack_init();
    fparams_stack = stack_init();

    //find main function in global symbol table
    htab_item *func_main = htab_lookup(G.g_globalTab, "main");
    if(func_main == NULL)
        exit_error(3);

    //copy of main symbol table
    htab_t *main_tab = htab_copy(func_main->data.function->local_tab);
    TStack *func_main_frame = stack_init();
    stack_push(func_main_frame, main_tab);
    active_frame = func_main_frame;

    interpret_loop(func_main->data.function->ins_list);
}
