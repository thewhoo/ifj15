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
  char *data;
  int capacity;
  int used;
} TString;

int initString(TString *v, int size);
int insertIntoString(TString *v, char value);
int getFromString(TString *v, int index);
void deleteFromString(TString *v, int index, int count);
void freeString(TString *v);

#endif
