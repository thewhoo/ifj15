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
 * GNU General Public License v2.0
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "string.h"
#include "error.h"
#include "enums.h"
#include "galloc.h"

int initString(TString *v, int size)
{
  v->data = gmalloc(size * sizeof(char));
  if (v->data == NULL)
    return E_ALLOC;

  v->data[0] = 0;
  v->capacity = size;
  v->used = 0;

  return E_OK;
}

int resizeString(TString *v, int size)
{
  // Cannot shrink used data
  assert(size > v->used);

  void *newData;

  // Prevent memory leak by using a temporary pointer
  newData = grealloc(v->data, size * sizeof(char));
  if (newData == NULL)
    return E_ALLOC;

  v->data = newData;
  v->capacity = size;

  return E_OK;
}

int insertIntoString(TString *v, char value)
{
  // String is full, it must be extended
  int ret = E_OK;
  if (v->used >= v->capacity)
    ret = resizeString(v, v->capacity * VECTOR_AUTORESIZE_COEF);
  if (ret != E_OK)
    return ret;

  // v->used will be the same as the first free index
  v->data[v->used] = value;
  (v->used)++;

  return E_OK;
}

char getFromString(TString *v, int index)
{
  // We should not access unallocated members
  assert(index < v->used);

  return v->data[index];
}

void deleteFromString(TString *v, int index, int count)
{
  assert(index >= 0 && count > 0 && count <= v->used - index);

  // Remove n values by replacing them with values located further in the array
  for(int i = index; i < (v->used - count); i++)
    v->data[i] = v->data[i+count];

  v->used -= count;
}

void freeString(TString *v)
{
  gfree(v->data);
}
