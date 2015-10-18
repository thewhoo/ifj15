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

#include "galloc.h"
#include "ilist.h"

Tins_list* list_init()
{
    Tins_list* list = gmalloc(sizeof(Tins_list));
    list->first = NULL;
    list->act = NULL;
    list->last = NULL;

    return list;
}

void list_free(Tins_list *list)
{
    TList_item *tmp;
    while(list->first != NULL)
    {
        tmp = list->first;
        list->first = list->first->next;
        gfree(tmp);
    }
    gfree(list);
} 

void list_insert(Tins_list *list, TList_item *item)
{
    item->next = NULL;
    if(list->first == NULL)
        list->first = item;
    else
        list->last->next = item;
    list->last = item;
}

void list_first(Tins_list *list)
{
    list->act = list->first;
}

void list_next(Tins_list *list)
{
    if(list->act != NULL)
        list->act = list->act->next;
}
