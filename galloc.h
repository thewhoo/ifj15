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

/**
  * @file galloc.h
  * @brief Garbage collector library
  *
  * Include this file to use the garbage collector
  *
  * @note The garbage collector must be initialized prior to use by calling
  * gInit()
  */

#ifndef GALLOC_H
#define GALLOC_H

void gcInit();
void gcDestroy();
void *gmalloc(int size);
void *grealloc(void *ptr, int size);
void gfree(void *ptr);

#endif //GALLOC_H
