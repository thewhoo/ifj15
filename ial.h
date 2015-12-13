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
  * @file ial.h
  * @brief Functions required for IAL
  *
  * ADT - Hash table
  * built in functions - find, sort
  *
  */

#ifndef IAL_H
#define IAL_H

#include "adt.h"

/**
 * find substring in string
 *
 * @param *s ptr to target string 
 * @param *search ptr to patter
 * @return index of match, from 0, -1 if not found
 */
int find(TVariable* s, TVariable* search);

/**
 * sort chars in string, ascending
 *
 * @param *var ptr to string
 * @return *ptr to sorted string
 */
char* sort(TVariable *var);

/* -----------------Hash tab ------------------- */

#define HTAB_SIZE 97 //planetmath.org/goodhashtableprimes

enum data_types
{
    TYPE_VARIABLE,
    TYPE_FUNCTION,
};

typedef struct htab_listitem 
{
    const char *key;
    int data_type;
    union
    {
        struct s_variable *variable;
        struct s_function *function;
    }data;
    struct htab_listitem *next;
}htab_item;

typedef struct hash_tab
{
    unsigned int htab_size;
    struct htab_listitem* list[];
}htab_t;

/**
 * @brief create and init hash table
 * @param size size of hash table
 * @return ptr to new table
 */
htab_t *htab_init(unsigned int size);

/**
 * @brief copy constructor 
 * @param htab pointer to original hash table
 * @return pointer to copied hash table
 */
htab_t *htab_copy(htab_t *old_tab);

/**
 * @brief Search for item in hash table
 * @param tab hash table
 * @param key of searched item
 * @return ptr to item or NULL if not found
 */
struct htab_listitem* htab_lookup(htab_t* tab, const char* key);

/**
 * @brief insert new item to hash table
 * @param tab hash table
 * @param key of new item
 * @return ptr to new item 
 */
struct htab_listitem* htab_insert(htab_t* tab, const char* key);

/**
 * @brief call function for every table item 
 * @param tab hash table
 * @param function to be called
 */
void htab_foreach(
        htab_t *tab, 
        void function(const char* key, struct s_variable *var));

/**
 * @brief removes one item from table
 * @param tab hash table
 * @param key of item to be removed
 */
void htab_remove(htab_t* tab, const char* key);

/**
 * @brief Removes all items from table. 
 * @param tab hash table
 */
void htab_clear(htab_t *tab);

/**
 * @brief Frees hash table and all items
 * @param tab hash table
 */
void htab_free(htab_t *tab);

/**
 * @brief prints max min and average length of synonyms
 * @param tab hash table
 */
void htab_statistics(htab_t* tab);



#endif //IAL_H
