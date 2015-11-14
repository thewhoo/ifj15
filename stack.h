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
  */

#ifndef STACK_H
#define STACK_H

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
  * @return Pointer to initialized stack
  */
TStack* stack_init();

/**
  * @brief  deallocate memory
  *
  * @param *stack Pointer to the stack structure
  */
void stack_free(TStack* stack);

/**
  * @brief  Removes one item from top of the stack
  *
  * No effects on empty stack
  *
  * @param *stack Pointer to the stack structure
  * @warning POP do not return item from top of stack!
  */
void stack_pop(TStack* stack);
/**
  * @brief  Insert item on top of the stack
  *
  * @param *item Pointer to item
  * @param *stack Pointer to the stack structure
  */
int stack_push(TStack* stack, void* item);
/**
  * @brief  Returns item from top of the stack, DO NOT REMOVE IT
  *
  * @param *stack Pointer to the stack structure
  * @warning TOP do not delete item from top of stack!
  */
void* stack_top(TStack* stack);
/**
  * @brief  Check if is stack empty
  *
  * @param *stack Pointer to the stack structure
  */
int stack_empty(TStack* stack);

/**
  * @brief Copy (duplicate) a stack
  *
  * @param *stack Pointer to the stack which you want to duplicate
  * @return Pointer to the duplicate of the stack
  */
TStack *stack_copy(TStack *stack);

#endif //STACK_H
