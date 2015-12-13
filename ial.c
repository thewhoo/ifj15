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
#include "galloc.h"
#include "adt.h"

/* -------------- KMP find--------------------*/
int find(TVariable *s, TVariable *search)
{
    char* str = s->data.str;
    char* substr = search->data.str;

    int str_len = strlen(str);
    int subs_len = strlen(substr);
    int r = -1;

    //init fail vector
    int *fail;
    fail = gmalloc(sizeof(int)*subs_len);

    //fill fail vector
    fail[0] = r;
    for(int i = 1; i < subs_len; i++)
    {
        r = fail[i - 1];
        while ((r > -1) && (substr[r] != substr[i-1]))
            r = fail[r];
        fail[i] = r + 1;
    }

    //kmp searching
    int str_ind = 0;
    int subs_ind = 0;

    while((str_ind < str_len) && (subs_ind < subs_len))
    {
        if ((subs_ind == -1) || (str[str_ind] == substr[subs_ind]))
        {
            str_ind++;
            subs_ind++;
        }
        else //chars do not match (or beggining of pattern)
            subs_ind = fail[subs_ind];
    }
    
    gfree(fail);
    
    if (subs_ind >= subs_len)
        return (str_ind - subs_len); 
    else
        return (-1); //-1 for ifj    
}
    

/* ----------------heap sort-------------------*/
/**
  * Reorganize array to get heap (sorted binary tree)
  * Move root item to right place
  */
void siftDown(char *str, int left, int right)
{
    int root, child, tmp;
    bool cont; //continue  condition 

    root = left;
    child = 2*root + 1;
    tmp = str[root];
    cont = child <= right; 

    while (cont)
    {
        if (child < right) //have both children, choose higher
            if (str[child] < str[child+1])
                child++;

            if (tmp >= str[child]) //already higher in root
                cont = false;
            else
            {
                str[root] = str[child];
                root = child;
                child = 2*root + 1;
                cont = child <= right; //cont if have some child
            }
    }

    str[root] = tmp; 
}

char *sort(TVariable *var)
{
    char *orig_str = var->data.str;
    int s_len = strlen(orig_str);
    char *str = gmalloc(s_len + 1);
    strcpy(str, orig_str);   

    int right = s_len - 1;
    int left = s_len / 2 - 1;

    //create heap
    for (int i = left; i >= 0; i--)
        siftDown(str, i, right);

    //sort
    for (; right > 0; right--)
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
    unsigned int hash = 0;
    int c;
    //sdbm
    while((c = *str++))
        hash = c + (hash << 6) +(hash << 16) - hash;

    return hash % htab_size;
}

htab_t *htab_init(unsigned int size)
{
    htab_t *tab;
    tab = gmalloc(sizeof(htab_t) + sizeof(struct htab_listitem)*size);

    tab->htab_size = size;
    for(unsigned int i = 0; i<size; i++)
        tab->list[i] = NULL;

    return tab;
}

htab_t* htab_copy(htab_t* old_tab)
{
    if(old_tab == NULL)
        return NULL;

    TVariable *var_copy;
    htab_item *old_item;
    htab_item *new_item;
    // create new hash tab
    htab_t *new_tab = htab_init(old_tab->htab_size);

    for(unsigned int i = 0; i < old_tab->htab_size; i++)
        for(old_item=old_tab->list[i]; old_item!=NULL; old_item=old_item->next)
        {
            // insert new empty item in hash tab
            new_item = htab_insert(new_tab, old_item->key);
            new_item->data_type = old_item->data_type;
            // copy variable 
            var_copy = gmalloc(sizeof(TVariable));
            memcpy(var_copy, old_item->data.variable, sizeof(TVariable));   
            // insert pointer to new var in new htab item
            new_item->data.variable = var_copy;
            new_item->next = NULL;
        }

    return new_tab;
}

struct htab_listitem* htab_lookup(htab_t* tab, const char* key)
{
    struct htab_listitem* item;
    unsigned int index;
    
    index = hash_function(key, tab->htab_size);
    
    if(tab->list[index] == NULL) //list is empty
    {
        return NULL;
    }
    else //search for key, return ptr
    {   
        item = tab->list[index];
        while(1)
        {
            if(strcmp(item->key, key) == 0)
                return item;    
            if(item->next == NULL)
                return NULL;
            item = item->next;
        }
    }
    
    return NULL; 
}

struct htab_listitem* htab_insert(htab_t* tab, const char* key)
{
    struct htab_listitem* item;
    unsigned int index;
    
    index = hash_function(key, tab->htab_size);
    
    if(tab->list[index] == NULL) //insert first for empty list
    {
        tab->list[index] = gmalloc(sizeof(struct htab_listitem));
        item = tab->list[index];
    }
    else 
    {   
        item = tab->list[index];
        while(item->next != NULL)
            item = item->next;

        item->next = gmalloc(sizeof(struct htab_listitem));
        item = item->next;
    }

    item->key = key;

    item->next = NULL;

    return item; 
}



void htab_foreach(
        htab_t *tab,
        void function(const char* key, struct s_variable *var))
{
    struct htab_listitem *item;

    for(unsigned int i = 0; i < tab->htab_size; i++)
        for(item = tab->list[i]; item != NULL; item = item->next)
            function(item->key, item->data.variable);
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
   
    if(item == tab->list[index]) //deleting first item
        tab->list[index] = item->next;
    else //deleting next/last item
        prev_item->next = item->next;

    //gfree((char*)item->key);
    gfree(item);
}

void htab_clear(htab_t *tab)
{
    struct htab_listitem *tmp;

    for(unsigned int i = 0; i < tab->htab_size; i++)
        while(tab->list[i] != NULL)
        {
            tmp = tab->list[i];
            tab->list[i] = tab->list[i]->next;
            if(tmp->data_type == TYPE_VARIABLE)
            {   //free string data
                if(tmp->data.variable->var_type == TYPE_STRING)
                    gfree(tmp->data.variable->data.str);
                gfree((void*)tmp->data.variable);
            }
            //gfree((void*)tmp->key);
            gfree((void*)tmp);
        }       
}

void htab_free(htab_t *tab)
{
    if (tab != NULL)
    {
        htab_clear(tab); //free items
        gfree(tab);      //free tab
    }
}

void htab_statistics(htab_t* tab)
{
    unsigned int max = 0;    
    unsigned int min = 1000000000;    
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
    printf("Average length: %u\nLongest list: %u\nShortest list: %u\n", avg, max, min);
}
