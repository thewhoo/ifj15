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
  * @file string.h
  * @brief String library
  *
  * This library allows the use of dynamically sized strings with automatic
  * extension.
  *
  * @warning You may print the string using printf("%s\n", 
  * s->data), but you MUST insert a binary 0 at the end of the string!
  */

#ifndef VECTOR_H
#define VECTOR_H

/**
  * @brief TString structure
  *
  * Structure which holds the dynamic string.
  */
typedef struct
{
  char *data; /**< Points to the location of the string in memory. */
  int capacity; /**< Total capacity */
  int used; /**< Bytes used */
} TString;

/**
  * @brief String intialization function
  *
  * Initializes an empty string structure @p *v with an initial
  * size of @p size. Returns an @c error code (see enums.h).
  *
  * @param *v Pointer to the string struct
  * @param size Initial string size (you may use STR_AUTORESIZE_COEF)
  */
int initString(TString *v, int size);

/**
  * @brief Insert a new character into the string
  *
  * Inserts a new character into the string.
  *
  * @param *v Pointer to the string struct
  * @param value Char to insert
  */
int insertIntoString(TString *v, char value);

/**
  * @brief Get specific char from string
  * 
  * Safely retrieve a character from the string
  *
  * @param *v Pointer to the string struct
  * @param index Index of character to retrieve;
  */
int getFromString(TString *v, int index);

/**
  * @brief Delete characters from string
  *
  * Safely remove characters from the string
  *
  * @param *v Pointer to the string struct
  * @param index Index at which to start deletion
  * @param count Number of characters to delete
  */
void deleteFromString(TString *v, int index, int count);

/**
  * @brief Deallocate string
  *
  * Deallocate string
  *
  * @param *v Pointer to the string
  */
void freeString(TString *v);

#endif
