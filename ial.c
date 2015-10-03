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
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "ial.h"

/* ----------------heap sort-------------------*/

void siftDown(char *str, int left, int right)
{
    int i, j, tmp;
    bool cont; //continue   

    i = left;
    j = 2*i;
    tmp = str[i];
    cont = j <= right;

    while (cont)
    {
        if (j < right)
            if (str[j] < str[j+1])
                j++;

            if (tmp >= str[j])
                cont = false;
            else
            {
                str[i] = str[j];
                i = j;
                j = 2 * i;
                cont = j <= right;
            }
    }

    str[i] = tmp;
}

char *sort(char * str)
{
    int right = strlen(str) - 1;
    int left = right / 2;

    for (int i = left; i >= 0; i--)
        siftDown(str, i, right);

    for ( ; right >= 1; right--)
    {
        str[0] ^= str[right]; //xor swap
        str[right] ^= str[0];
        str[0] ^= str[right];
        siftDown(str, 0, right-1);
    }
    
    return str;    
}

/* ---------------hash table-------------------*/

unsigned int hash_function(const char *str, unsigned htab_size)
{
    unsigned int h = 0;
    const unsigned char *p;
    for(p = (const unsigned char*)str; *p != '\0'; p++)
        h = 65599*h + *p;
    return h % htab_size;
}

htab_t *htab_init(unsigned int size)
{
    htab_t *tab;
    tab = malloc(sizeof(htab_t) + sizeof(struct htab_listitem)*size);

    if (tab == NULL)
        return NULL;

    tab->htab_size = size;
    for(unsigned int i = 0; i<size; i++)
        tab->list[i] = NULL;

    return tab;
}

struct htab_listitem* htab_lookup(htab_t* tab, const char* key)
{
    struct htab_listitem* item;
    unsigned int index;
    
    index = hash_function(key, tab->htab_size);
    
    if(tab->list[index] == NULL) //ak je zoznam prazdny, pridanie na zaciatok
    {
        tab->list[index] = malloc(sizeof(struct htab_listitem));
        item = tab->list[index];
    }
    else //inak hladame key, ak je, vrati pointer, inak prida dalsi prvok
    {   
        item = tab->list[index];
        while(1) //vieme, ze tab->list[index] != NULL
        {
            if(strcmp(item->key, key) == 0)
                return item;    
            if(item->next == NULL)
                break;
            item = item->next;
        }

        item->next = malloc(sizeof(struct htab_listitem));
        item = item->next;
    }

    if(item == NULL)
        return NULL;
    
    item->key = malloc(strlen(key) + 1);
    if(item->key == NULL)
        return NULL;

    strcpy((char*)item->key, key);
    item->data = 0;
    item->next = NULL;

    return item; 
}

void htab_foreach(htab_t *tab, void function(const char* key, int value))
{
    struct htab_listitem *item;

    for(unsigned int i = 0; i < tab->htab_size; i++)
        for(item = tab->list[i]; item != NULL; item = item->next)
            function(item->key, item->data);
}

void htab_remove(htab_t* tab, const char* key)
{
    struct htab_listitem* item;
    struct htab_listitem* prev_item = NULL;
    unsigned int index;

    index = hash_function(key, tab->htab_size);
    item = tab->list[index];
   
    while(item != NULL && (strcmp(key, item->key) != 0))
    {
        prev_item = item;
        item = item->next;
    }
   
    if(item == NULL)
        return;
   
    if(item == tab->list[index]) //ak mazeme prvy prvok
        tab->list[index] = item->next;
    else //prostredny/posledny prvok
        prev_item->next = item->next;

    free((char*)item->key);
    free(item);
}

void htab_clear(htab_t *tab)
{
    struct htab_listitem *tmp;

    for(unsigned int i = 0; i < tab->htab_size; i++)
        while(tab->list[i] != NULL)
        {
            tmp = tab->list[i];
            tab->list[i] = tab->list[i]->next;
            free((char*)tmp->key);
            free(tmp);
        }       
}

void htab_free(htab_t *tab)
{
    if (tab != NULL)
    {
        htab_clear(tab);
        free(tab); 
    }
}

void htab_statistics(htab_t* tab)
{
    unsigned int max = 0;    
    unsigned int min = 1000000000; //magic constant..    
    unsigned int avg = 0;    
    unsigned int length = 0;    
    struct htab_listitem* item;

    for (unsigned int i = 0; i < tab->htab_size; i++)
    {
        for(item = tab->list[i]; item != NULL; item = item->next)
            length++;

        avg += length;
        if(length > max)
            max = length;
        if(length < min)
            min = length;
        length = 0;        
    }
    avg = avg/(tab->htab_size);
    printf("Priemerna dlzka zoznamu: %u\nNajdlhsi zoznam: %u\nNajkratsi zoznam: %u\n", avg, max, min);
}
