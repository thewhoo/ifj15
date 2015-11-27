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
 * @param str string
 * @param substr substring
 * @return index of match, from 0, -1 if not found
 */
int find(TVariable* s, TVariable* search);

/**
 * sort chars in string, ascending
 *
 * @param *str ptr to string
 * @return *ptr to sorted string
 */
char * sort(TVariable *var);

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
 * @brief vytvori a inicializuje hash tabulku 
 * @param size velkost tabulky 
 * @return vrati ukazatel na tabulku alebo null
 */
htab_t *htab_init(unsigned int size);

/**
 * @brief copy constructor 
 * @param htab pointer to original hash table
 * @return pointer to copied hash table
 */
htab_t *htab_copy(htab_t *old_tab);

/**
 * @brief vyhlada prvok v tabulke 
 * @param tab hash tabulka
 * @param key prvok, ktory hladame
 * @return ukazatel na hladany prvok tabulky, ak nie je tak null         
 */
struct htab_listitem* htab_lookup(htab_t* tab, const char* key);

/**
 * @brief vlozi novy prvok do tabulky 
 * @param tab hash tabulka
 * @param key kluc noveho prvku
 * @return ukazatel na novy prvok, alebo null 
 */
struct htab_listitem* htab_insert(htab_t* tab, const char* key);

/**
 * @brief zavola funckiu pre kazdy prvok hash tabulky 
 * @param tab hash tabulka
 * @param function funkcia, ktoru chceme volat
 */
void htab_foreach(
        htab_t *tab, 
        void function(const char* key, struct s_variable *var));

/**
 * @brief odstrani prvok z tabulky 
 * @param tab hash tabulka
 * @param key dany prvok
 */
void htab_remove(htab_t* tab, const char* key);

/**
 * @brief zmaze vsetky prvky tabulky 
 * @param tab hash tabulka
 */
void htab_clear(htab_t *tab);

/**
 * @brief uvolni celu tabulku z pamate
 * @param tab hash tabulka
 */
void htab_free(htab_t *tab);

/**
 * @brief vypis max min a priemernej dlzky jedneho zoznamu v hash tabulke
 * @param tab hash tabulka
 */
void htab_statistics(htab_t* tab);



#endif //IAL_H
