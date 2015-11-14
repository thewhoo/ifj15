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
#include "error.h"
#include "galloc.h"
#include "enums.h"

#define DEFAULT_STACK_SIZE 15
#define AUTORESIZE_COEF 2

TStack* stack_init()
{
    TStack *stack = gmalloc(sizeof(TStack));
    stack->data = gmalloc(DEFAULT_STACK_SIZE * sizeof(void *));

    stack->capacity = DEFAULT_STACK_SIZE;
    stack->used = 0;

    return stack;
}

TStack* stack_copy(TStack *s)
{
  TStack *sNew = gmalloc(sizeof(TStack));
  sNew->data = gmalloc(sizeof(void *) * s->capacity);

  for(int i = 0; i < s->used; i++)
    sNew->data[i] = s->data[i];

  return sNew;
}

void stack_free(TStack* stack)
{
    gfree(stack->data);
    gfree(stack);
}

int stack_push(TStack* stack, void* item)
{
    if(stack->capacity <= stack->used)
    {
        stack->data = grealloc(stack->data,
                 sizeof(void *) * stack->capacity * AUTORESIZE_COEF);
        stack->capacity = stack->capacity * AUTORESIZE_COEF;
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
