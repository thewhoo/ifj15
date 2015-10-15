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

#include <stdlib.h>

#include "stack.h"
#include "enums.h"
#include "galloc.h"

int stack_init(TStack *stack)
{
    stack->data = gmalloc(DEFAULT_STACK_SIZE * sizeof(void *));
    if(stack->data == NULL)
         return E_ALLOC;   
    
    stack->capacity = DEFAULT_STACK_SIZE;
    stack->used = 0;

    return 0;
}

void stack_free(TStack* stack)
{
    gfree(stack->data);    
}

int stack_push(TStack *stack, void* item)
{
    void **new_data;

    if(stack->capacity == (stack->used + 1))
    {
        new_data = grealloc(stack->data, 
                        sizeof(void *) * stack->capacity*VECTOR_AUTORESIZE_COEF);    
        if(new_data == NULL)
            return E_ALLOC;
        stack->data = new_data;
        stack->capacity = stack->capacity * VECTOR_AUTORESIZE_COEF;
    }

    stack->data[stack->used] = item;
    (stack->used)++;             

    return 0;
}

void stack_pop(TStack *stack)
{
    if(stack->used > 0)
        (stack->used)--;
}

void* stack_top(TStack *stack)
{
    if(stack->used > 0)
        return stack->data[stack->used - 1];
    else
        return NULL; 
}

int stack_empty(TStack *stack)
{
    return (stack->used == 0);
}
