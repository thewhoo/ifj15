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
  * @file stack.h
  * @brief Stack library
  *
  * This library allows the use of the stack ADT with automatic
  * extension.
  *
  * @note You may print the string using printf("%s\n", 
  * s->data), but you MUST insert a binary 0 at the end of the string!
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
  * Initializes an empty stack on heap with default size.
  *
  * @param *v Poin
  */
int stack_init(TStack* stack);
void stack_free(TStack* stack);
void stack_pop(TStack* stack);
int stack_push(TStack* stack, void* item);
void* stack_top(TStack* stack);
int stack_empty(TStack* stack);

#endif //STACk_H
