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

#ifndef SHARED_H
#define SHARED_H

#include "adt.h"
#include "galloc.h"

// TODO: documentation, motherfucker!
// Structure for global table of constants
typedef struct s_consttab
{
	TVariable **data;
	int size;
	int used;

} TConstTab;

// TODO: documentation, motherfucker!
// Structure for global table of global symbols (a.k.a. functions)
typedef struct s_globals
{
	TFunction **data;
	int size;
	int used;

} TGlobalTab;

// Global ConstTab
extern TConstTab *ConstTab;

// Global GlobalTab
extern TGlobalTab *GlobalTab;

// Functions
void initConstTab();
void insertIntoConstTab();
void initGlobalTab();
void insertIntoGlobalTab();

#endif // SHARED_H
