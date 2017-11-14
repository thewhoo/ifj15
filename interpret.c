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
 * GNU General Public License v2.0
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
    exit_error(E_INTERNAL);
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
                dest->data.d = a + b;
            else
                dest->data.i = (int)(a + b);
            break;
        case('-'):
            if(dest->var_type == TYPE_DOUBLE)
                dest->data.d = a - b;
            else
                dest->data.i = (int)(a - b);
            break;
        case('*'):
            if(dest->var_type == TYPE_DOUBLE)
                dest->data.d = a * b;
            else
                dest->data.i = (int)(a * b);
            break;
        case('/'):
            if(b == 0)
                exit_error(E_ZERO_DIVISION);

            if(dest->var_type == TYPE_DOUBLE)
                dest->data.d = a/b;
            else
                dest->data.i = (int)(a/b);
            break;
    }

    dest->initialized = 1;
}

void compare_ins(int type, TVariable* dest, TVariable *src1, TVariable* src2)
{
    int result;
    double a, b;

    if(!src1->initialized || !src2->initialized)
        exit_error(E_UNINITIALIZED);

    if(src1->var_type == TYPE_DOUBLE)
        a = src1->data.d;
    else
        a = src1->data.i;  //if src is string, some random values ar in a and b

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
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (result == 0) ? 1 : 0;
                else
                    dest->data.d = (result == 0) ? 1 : 0;
                break;
            case(INS_NEQ):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (result != 0) ? 1 : 0;
                else
                    dest->data.d = (result != 0) ? 1 : 0;
                break;
            case(INS_GREATER):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (result > 0) ? 1 : 0;
                else
                    dest->data.d = (result > 0) ? 1 : 0;
                break;
            case(INS_GREATEQ):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (result >= 0) ? 1 : 0;
                else
                    dest->data.d = (result >= 0) ? 1 : 0;
                break;
            case(INS_LESSER):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (result < 0) ? 1 : 0;
                else
                    dest->data.d = (result < 0) ? 1 : 0;
                break;
            case(INS_LESSEQ):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (result <= 0) ? 1 : 0;
                else
                    dest->data.d = (result <= 0) ? 1 : 0;
                break;
        }
    }
    else if((src1->var_type != TYPE_STRING) && (src2->var_type != TYPE_STRING))
    {
        switch(type)
        {
            case(INS_EQ):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (a == b) ? 1 : 0;
                else
                    dest->data.d = (a == b) ? 1 : 0;
                break;
            case(INS_NEQ):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (a != b) ? 1 : 0;
                else
                    dest->data.d = (a != b) ? 1 : 0;
                break;
            case(INS_GREATER):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (a > b) ? 1 : 0;
                else
                    dest->data.d = (a > b) ? 1 : 0;
                break;
            case(INS_GREATEQ):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (a >= b) ? 1 : 0;
                else
                    dest->data.d = (a >= b) ? 1 : 0;
                break;
            case(INS_LESSER):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (a < b) ? 1 : 0;
                else
                    dest->data.d = (a < b) ? 1 : 0;
                break;
            case(INS_LESSEQ):
                if(dest->var_type == TYPE_INT)
                    dest->data.i = (a <= b) ? 1 : 0;
                else
                    dest->data.d = (a <= b) ? 1 : 0;
                break;
        }
    }
    else
        exit_error(E_SEMANTIC_TYPES);

    dest->initialized = 1;
}


void map_params(htab_t *tab, TStack* decl_params)
{
    htab_item *param;
    TVariable *dest, *src;

    for(int i=0; i < decl_params->used; i++)
    {
        param = htab_insert(tab, ((TVariable*)decl_params->data[i])->name);
        param->data_type = TYPE_VARIABLE;
        dest = gmalloc(sizeof(TVariable));
        dest->name = ((TVariable*)decl_params->data[i])->name;
        dest->constant = 0;
        dest->initialized = 1;
        dest->var_type = ((TVariable*)decl_params->data[i])->var_type;
        param->data.variable = dest;
        src = fparams_stack->data[i];
        if(dest->var_type == TYPE_INT)
        {
            if(src->var_type == TYPE_DOUBLE)
                dest->data.i = (int)src->data.d;
            else
                dest->data.i = src->data.i;
        }
        else if (dest->var_type == TYPE_DOUBLE)
        {
            if(src->var_type == TYPE_DOUBLE)
                dest->data.d = src->data.d;
            else
                dest->data.d = (double)src->data.i;
        }
        else
        {
            dest->data.str = gmalloc(strlen(src->data.str) + 1);
            strcpy(dest->data.str, src->data.str);
        }
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
    int ret_int, int1, int2;
    char* ret_str;
    TVariable *var1, *var2, *var3;
    TFunction *func;
    htab_item *item;
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
            //compare instructions
            case(INS_EQ):
            case(INS_NEQ):
            case(INS_GREATER):
            case(INS_GREATEQ):
            case(INS_LESSER):
            case(INS_LESSEQ):
                compare_ins(ins->ins_type, get_var(ins->addr1),
                            get_var(ins->addr2), get_var(ins->addr3));
                break;
            //push to active frame new tab for new nested block
            case(INS_PUSH_TAB):
                int1 = ((TFunction*)ins->addr1)->var_count;
                int1 = int1 + (int1 >> 1) + 1;
                new_tab = htab_init(int1);
                stack_push(active_frame, new_tab);
                break;

            //new var defined - check if isn't already in htable
            //e.g. var definition in loops
            case(INS_PUSH_VAR):
                new_tab = stack_top(active_frame);
                var1 = ((TVariable*)ins->addr1);
                item = htab_lookup(new_tab, var1->name);
                if(item == NULL)
                {
                    item = htab_insert(new_tab, var1->name);
                    item->data_type = TYPE_VARIABLE;
                    var2 = gmalloc(sizeof(TVariable));
                    item->data.variable = var2;
                    memcpy(var2, ins->addr1, sizeof(TVariable));

                }
                else if(item->data.variable->var_type == TYPE_STRING)
                {
                    gfree(item->data.variable->data.str);
                }
                item->data.variable->initialized = 0;
                break;

            //leaving nested block, clean up
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

            //push var as parameter for following function call
            case(INS_PUSH_PARAM):
                var1 = get_var(ins->addr1);
                if(!var1->initialized)
                    exit_error(E_UNINITIALIZED);
                stack_push(fparams_stack, var1);
                break;

            //call function - create new frame, map parameters to function
            //local vars
            case(INS_CALL):
                stack_push(gStack, active_frame);
                stack_push(gStack, ins);

                func = (TFunction*)((htab_item*)ins->addr1)->data.function;
                ins = func->ins_list->first;
                #ifdef DEBUG_MODE
                print_instructions(ins);
                #endif
                active_frame = stack_init();
                int1 = func->var_count;
                int1 = int1 + (int1 >> 1) + 1;
                new_tab = htab_init(int1);
                //new_tab = htab_init(HTAB_SIZE);
                stack_push(active_frame, new_tab);
                map_params(new_tab, func->params_stack);
                //after break, continue with new isntr, we want to begin with first one
                continue;

            //return from function - clean function frame, when leaving main
            //func, return 0!
            case(INS_RET):
                if(stack_empty(gStack))  //end of main func
                {
                    //gcDestroy();
                    exit(0);
                }

                clean_active_frame();
                ins = (TList_item*) stack_top(gStack);
                stack_pop(gStack);
                active_frame = (TStack*) stack_top(gStack);
                stack_pop(gStack);
                break;

            case(INS_ASSIGN):
                var1 = get_var(ins->addr1);
                var2 = get_var(ins->addr2);
                if(!var2->initialized)
                    exit_error(E_UNINITIALIZED);
                if(var1->var_type == TYPE_INT)
                {
                    if(var2->var_type == TYPE_DOUBLE)
                        var1->data.i = (int)var2->data.d;
                    else
                        var1->data.i = var2->data.i;
                }
                else if (var1->var_type == TYPE_DOUBLE)
                {
                    if(var2->var_type == TYPE_DOUBLE)
                        var1->data.d = var2->data.d;
                    else
                        var1->data.d = (double)var2->data.i;
                }
                else
                {
                    if(var1->initialized)
                        gfree(var1->data.str);

                    var1->data.str = gmalloc(strlen(var2->data.str) + 1);
                    strcpy(var1->data.str, var2->data.str);
                }
                var1->initialized = 1;
                break;

            //built-in
            case(INS_LENGTH):
                var2 = get_var(ins->addr2);
                if(!var2->initialized)
                    exit_error(E_UNINITIALIZED);
                ret_int = length(var2);
                var1 = get_var(ins->addr1);
                if(var1->var_type == TYPE_INT)
                    var1->data.i = ret_int;
                else
                    var1->data.d = (double)ret_int;
                var1->initialized = 1;
                break;

            case(INS_SUBSTR):
                var1 = (TVariable*)fparams_stack->data[0];
                var2 = (TVariable*)fparams_stack->data[1];
                var3 = (TVariable*)fparams_stack->data[2];
                if(!var1->initialized || !var2->initialized || !var3->initialized)
                    exit_error(E_UNINITIALIZED);
                stack_clear(fparams_stack);
                if(var2->var_type == TYPE_INT)
                    int1 = var2->data.i;
                else
                    int1 = (int)var2->data.d;
                if(var3->var_type == TYPE_INT)
                    int2 = (int)var3->data.i;
                else
                    int2 = (int)var3->data.d;
                ret_str = substr(var1->data.str, int1, int2);
                var3 = get_var(ins->addr1);
                //free old string in var
                if(var3->initialized)
                    gfree(var3->data.str);
                var3->data.str = ret_str;
                var3->initialized = 1;
                break;

            case(INS_CONCAT):
                var2 = get_var(ins->addr2);
                var3 = get_var(ins->addr3);
                if(!var2->initialized || !var3->initialized)
                    exit_error(E_UNINITIALIZED);
                ret_str = concat(var2, var3);
                var1 = get_var(ins->addr1);
                //free old string in var
                if(var1->initialized)
                    gfree(var1->data.str);
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
                if(var1->var_type == TYPE_INT)
                    var1->data.i = ret_int;
                else
                    var1->data.d = (double)ret_int;
                var1->initialized = 1;
                break;

            case(INS_SORT):
                var2 = get_var(ins->addr2);
                if(!var2->initialized)
                    exit_error(E_UNINITIALIZED);
                ret_str = sort(var2);
                var1 = get_var(ins->addr1);
                //free old string in var
                if(var1->initialized)
                    gfree(var1->data.str);
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
    exit_error(E_UNINITIALIZED); //called function's body is empty
}

void interpret()
{
    //init global stack for interpret
    gStack = stack_init();
    fparams_stack = stack_init();

    //find main function in global symbol table
    htab_item *func_main = htab_lookup(G.g_globalTab, "main");
    if(func_main == NULL)
        exit_error(E_SEMANTIC_DEF);

    //create symbol table for main, push to active frame
    int vars = func_main->data.function->var_count;
    vars = vars + (vars >> 1) + 1;
    htab_t *main_tab = htab_init(vars);
    TStack *func_main_frame = stack_init();
    stack_push(func_main_frame, main_tab);
    active_frame = func_main_frame;

    interpret_loop(func_main->data.function->ins_list);
}
