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
  * @file builtin.h
  * @brief Implementation of built-in functions used by interpret
  * 
  *
  */

#include "adt.h"

/**
  * @brief Returns string length
  *
  * @param str ptr to string variable
  * @return int length of string
  */
int length(TVariable *str);

/**
  * @brief Concatenation of two strings
  *
  * @param *str1 ptr to string variable
  * @param *str2 ptr to string variable
  * @return ptr to new string
  */
char* concat(TVariable* str1, TVariable* str2);

/**
  * @brief C++ std::cout simulation, prints only one variable
  *
  * @param output ptr to variable with int/double/string value
  */
void cout(TVariable* output);

/**
  * @brief C++ std::cin simulation, reads only one variable
  *
  * @param in ptr to variable with int/double/string value
  */
void cin(TVariable *in);

/**
  * @brief Returns substring from index i, n chars long in string s
  *
  * C++ std::basic_string::substr simulation
  * @param *s ptr to string
  * @param i index of substring beggining
  * @param n length of substring
  * @return ptr to new string, substring of string s
  */
char* substr(char *s, int i, int n);
