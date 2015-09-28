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

#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
  int *data;
  int capacity;
  int used;
} TVector;

int initVector(TVector *v, int size);
int insertIntoVector(TVector *v, int value);
int getFromVector(TVector *v, int index);
void deleteFromVector(TVector *v, int index, int count);
void freeVector(TVector *v);

// Debug functions below
void dumpVector(TVector *v);
void apVector(TVector *v, int count);

#endif
