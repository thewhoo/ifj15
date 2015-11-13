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
#include "shared.h"
#include "adt.h"
#include "galloc.h"
#include "error.h"

#define INITIAL_SIZE 20
#define AUTORESIZE_COEF 2

void initConstTab(TConstTab *t)
{
	int size = sizeof(TVariable)*INITIAL_SIZE;

	t->data = gmalloc(size);
	t->size = size;
	t->used = 0;
}

void initGlobalTab(TGlobalTab *t)
{
	int size = sizeof(TFunction)*INITIAL_SIZE;

	t->data = gmalloc(size);
	t->size = size;
	t->used = 0;
}

void insertIntoGlobalTab(TGlobalTab *t, TFunction *f)
{
	if(t->used == t->size)
	{
		void *tmp = grealloc(t->data, sizeof(TFunction) * t->size * AUTORESIZE_COEF);
		if(tmp != NULL)
			t->data = tmp;
		else
			exit_error(E_ALLOC);
	}

	t->data[t->used] = f;
	(t->used)++;
}

void insertIntoConstTab(TConstTab *t, TVariable *v)
{
	if(t->used == t->size)
	{
		void *tmp = grealloc(t->data, sizeof(TVariable) * t->size * AUTORESIZE_COEF);
		if(tmp != NULL)
			t->data = tmp;
		else
			exit_error(E_ALLOC);
	}

	t->data[t->used] = v;
	(t->used)++;
}
