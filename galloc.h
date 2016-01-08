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

/**
  * @file galloc.h
  * @brief Garbage collector library
  *
  * Include this file to use the garbage collector
  *
  * @note The garbage collector must be initialized prior to use by calling
  * gcInit()
  */

#ifndef GALLOC_H
#define GALLOC_H

/**
  * @brief Initialize garbage collector
  *
  * Initializes the garbage collector.
  * This function must be called before anything else
  * can be used, ideally at the beginning of the program.
  *
  * @warning This function must be called before any other library calls can be used!
  * @return error Error code
  */
int gcInit();

/**
  * @brief Destroy garbage collector
  *
  * Free all memory allocated using @c gcalloc() and @c gcrealloc()
  * and finally all the memory used by the actual collector. This
  * function must be called at the end of the program.
  *
  * @warning This function must be called before the program exits, otherwise
  * you're going to cause a big memory leak!
  */
void gcDestroy();

/**
  * @brief Allocate memory
  *
  * Allocate memory using the garbage collector. Same
  * functionality as @c malloc().
  *
  * @param size Number of bytes to allocate
  * @return void* Pointer to allocated memory
  */
void *gmalloc(int size);

/**
  * @brief Reallocate memory
  *
  * Reallocate memory using the garbage collector. Same
  * funcionality as @c realloc().
  *
  * @param *ptr Pointer to currently allocated memory
  * @param size Number of bytes to newly allocate
  * @return void* Pointer to newly allocated memory
  */
void *grealloc(void *ptr, int size);

/**
  * @brief Free memory
  *
  * Manually free memory using the garbage collector. This
  * will stop the garbage collector from tracking this memory
  * location and free the memory. Same functionality as @c free().
  *
  * @param *ptr Pointer to memory
  */
void gfree(void *ptr);

#endif //GALLOC_H
