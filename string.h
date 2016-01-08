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
  * @file string.h
  * @brief String library
  *
  * This library allows the use of dynamically sized strings with automatic
  * extension.
  *
  * @note You may print the string using printf("%s\n", 
  * s->data), but you MUST insert a binary 0 at the end of the string!
  */

#ifndef STRING_H
#define STRING_H

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
  * @return error Error code
  */
int initString(TString *v, int size);

/**
  * @brief Insert a new character into the string
  *
  * Inserts a new character into the string.
  *
  * @param *v Pointer to the string struct
  * @param value Char to inserti
  * @return error Error code
  */
int insertIntoString(TString *v, char value);

/**
  * @brief Get specific char from string
  * 
  * Safely retrieve a character from the string
  *
  * @param *v Pointer to the string struct
  * @param index Index of character to retrieve;i
  * @return char Character on given index
  */
char getFromString(TString *v, int index);

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
