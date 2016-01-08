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

/**
  * @file ilist.h
  * @brief Instruction List library
  *
  */

#ifndef ILIST_H
#define ILIST_H

typedef struct listitem
{
    int ins_type;
    void *addr1;
    void *addr2;
    void *addr3;
    struct listitem *next;
}TList_item;

typedef struct s_list
{
    TList_item *first;
    TList_item *act;
    TList_item *last;
}Tins_list;

/**
  * @brief Initialize empty list of instructions
  * 
  * @return list Pointer to new list
  */
Tins_list* list_init();
/**
  * @brief Deallocate list
  * 
  * @param list Pointer to list
  */
void list_free(Tins_list *list);
/**
  * @brief Insert new instruction item at the end of ins list
  * 
  * @param list Pointer to list
  * @param item Pointer to filled instruction 
  */
void list_insert(Tins_list *list, TList_item *item);
/**
  * @brief Set activity to first item in list
  * 
  * @param list Pointer to list
  */
void list_first(Tins_list *list);
/**
  * @brief Set activity to next instruction
  * 
  * Nothing happens when empty list
  * 
  * @param list Pointer to list
  */
void list_next(Tins_list *list);






#endif //ILIST_H
