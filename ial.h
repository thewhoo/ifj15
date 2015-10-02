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

#ifndef IAL_H
#define IAL_H

#define HTAB_SIZE 24593 //prime number, change later if needed 
  
struct htab_listitem //used in IJC, change items if needed
{
    const char *key;
    unsigned int data;
    struct htab_listitem *next;
};

typedef struct hash_tab
{
    unsigned int htab_size;
    struct htab_listitem* list[];
}htab_t;

/** used in IJC, now just private function
 * @brief sbdm hash funkcia
 * @param str retazec, na vzpocitanie hash cisla
 * @param htab_size velkost hash tabulky
 * @return index do hash tabulky
 *
unsigned int hash_function(const char *str, unsigned htab_size);
*/

/**
 * @brief vytvori a inicializuje hash tabulku 
 * @param size velkost tabulky 
 * @return vrati ukazatel na tabulku alebo null
 */
htab_t *htab_init(unsigned int size);

/**
 * @brief vyhlada prvok v tabulke, ak tam nie je tak ho prida 
 * @param tab hash tabulka
 * @param key prvok, ktory hladame
 * @return ukazatel na hladany prvok tabulky, ak sa nepodari pridat, tak null         
 */
struct htab_listitem* htab_lookup(htab_t* tab, const char* key);

/**
 * @brief zavola funckiu pre kazdy prvok hash tabulky 
 * @param tab hash tabulka
 * @param function funkcia, ktoru chceme volat
 */
void htab_foreach(htab_t *tab, void function(const char* key, int value));

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
