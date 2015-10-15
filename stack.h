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

#ifndef STACK_H
#define STACK_H

#define DEFAULT_STACK_SIZE 15

typedef struct Stack
{
    void **data; 
    int capacity;
    int used;
} TStack;

/**
  * @brief  Init stack
  *
  * Deallocate string
  *
  * @param *v Pointer to the string
  */
int stack_init(TStack* stack);
void stack_free(TStack* stack);
void stack_pop(TStack* stack);
int stack_push(TStack* stack, void* item);
void* stack_top(TStack* stack);
int stack_empty(TStack* stack);

#endif //STACk_H
