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
#include <stdio.h>
#include <assert.h>
#include "vector.h"
#include "enums.h"

int initVector(TVector *v, int size)
{
  v->data = malloc(size * sizeof(int));
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
  newData = realloc(v->data, size * sizeof(int));
  if (newData == NULL)
    return E_ALLOC;

  v->data = newData;
  v->capacity = size;

  return E_OK;
}

int insertIntoVector(TVector *v, int value)
{
  // Vector is full, it must be extended
  int ret = E_OK;
  if (v->used >= v->capacity)
    ret = resizeVector(v, v->capacity * VECTOR_AUTORESIZE_COEF);
  if (ret != E_OK)
    return ret;

  // v->used will be the same as the first free index
  v->data[v->used] = value;
  (v->used)++;

  return E_OK;
}

int getFromVector(TVector *v, int index)
{
  // We should not access unallocated members
  assert(index < v->used);

  return v->data[index];
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

/*
 * @DEBUG_SECTION
 * These functions are for debugging purposes
 * They should be removed in the final version
 *
 */

void dumpVector(TVector *v)
{
  for(int i=0; i<v->used; i++)
    printf("%d: %d\n", i, v->data[i]);
}

void apVector(TVector *v, int count)
{
  for(int i=0; i<count; i++)
    v->data[i] = i;

  v->used = count;
}