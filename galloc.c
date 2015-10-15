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
#include <assert.h>
#include <stdbool.h>
#include "galloc.h"
#include "enums.h"
#define INITIAL_SIZE 30
#define AUTORESIZE_COEF 2

typedef struct Vector
{
	void **data;
	int capacity;
	int used;
	bool initialized;
} TVector;

// Forward declarations of private functions
int initVector();
int resizeVector();
int insertIntoVector();
void deleteFromVector();
void freeVector();

// This is global so it can be safely initialized
TVector mem;

int initVector(TVector *v, int size)
{
  v->data = malloc(size * sizeof(void*));
  if (v->data == NULL)
    return E_ALLOC;

  v->capacity = size;
  v->used = 0;

  return E_OK;
}

int resizeVector(TVector *v, int size)
{
  // Cannot shrink used data
  assert(size > v->used);

  void *newData;

  // Prevent memory leak by using a temporary pointer
  newData = realloc(v->data, size * sizeof(void*));
  if (newData == NULL)
    return E_ALLOC;

  v->data = newData;
  v->capacity = size;

  return E_OK;
}

int insertIntoVector(TVector *v, void *value)
{
  // Vector is full, automatically extend
  int ret = E_OK;
  if (v->used >= v->capacity)
    ret = resizeVector(v, v->capacity * AUTORESIZE_COEF);
  if (ret != E_OK)
    return ret;

  // v->used will be the same as the first free index
  v->data[v->used] = value;
  (v->used)++;

  return E_OK;
}

void deleteFromVector(TVector *v, int index, int count)
{
  assert(index >= 0 && count > 0 && count <= v->used - index);

  // Remove n values by replacing them with values located further in the array
  for(int i = index; i < (v->used - count); i++)
    v->data[i] = v->data[i+count];

  v->used -= count;
}

void freeVector(TVector *v)
{
	free(v->data);
}

void searchAndRemove(TVector *v, void *value)
{
	// If address is matched, remove it from vector
	for (int i=0; i < v->used; i++)
	{
		if (value == v->data[i])
		{
			deleteFromVector(v, i, 1);
			break;
		}
	}
}

int gcInit()
{
	if (initVector(&mem, INITIAL_SIZE) != E_OK)
		return E_ALLOC;
	
	mem.initialized = true;
	
	return E_OK;
}

void gcDestroy()
{
	for (int i=0; i<mem.used; i++)
	{
		free(mem.data[i]);
	}

	free(mem.data);

	mem.initialized = false;
}

void *gmalloc(int size)
{
	assert(mem.initialized);

	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL)
		return NULL;

	insertIntoVector(&mem, ptr);
	return ptr;
}

void *grealloc(void *ptr, int size)
{
	assert(mem.initialized);

	void *newPtr;
	newPtr = realloc(ptr, size);
	if (newPtr == NULL)
		return NULL;

	if (newPtr != ptr)
	{
		searchAndRemove(&mem, ptr);
		insertIntoVector(&mem, newPtr);
	}

	return newPtr;
}

void gfree(void *ptr)
{
	searchAndRemove(&mem, ptr);
	free(ptr);
}
