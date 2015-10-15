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
#include "error.h"
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

void deleteFromVector(TVector *v, int index)
{
  assert(index >= 0 && index < v->used);

  // Remove value by replacing it with NULL
  v->data[index] = NULL;
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
			deleteFromVector(v, i);
			break;
		}
	}
}

int gcInit()
{
	if (initVector(&mem, INITIAL_SIZE) != E_OK)
		exit_error(E_ALLOC);
	
	mem.initialized = true;
	
	return E_OK;
}

void gcDestroy()
{
	if (mem.initialized == false)
		return;

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
		exit_error(E_ALLOC);

	insertIntoVector(&mem, ptr);
	return ptr;
}

void *grealloc(void *ptr, int size)
{
	assert(mem.initialized);

	void *newPtr;
	newPtr = realloc(ptr, size);
	if (newPtr == NULL)
		exit_error(E_ALLOC);

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
