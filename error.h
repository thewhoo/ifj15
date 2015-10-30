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
  * @file error.h
  * @brief Error states library
  *
  * Include this file to report errors in program
  *  
  */

#ifndef ERROR_H
#define ERROR_H

enum error_codes
{
  E_OK = 0,     //0  
  E_LEX = 1,    //1
  E_SYNTAX = 2,
  E_SEMANTIC_DEF = 3,
  E_SEMANTIC_TYPES = 4,
  E_AUTO_TYPE = 5,
  E_SEMANTIC_OTHERS = 6,
  E_READ_NUMBER = 7,
  E_UNINITIALIZED = 8,
  E_ZERO_DIVISION = 9,
  E_RUNTIME_OTHERS = 10,
  E_INTERNAL = 99, //return values defined in project description

  //local errors
  E_ALLOC

};


/**
  * @brief Writes specific error message, clean up allocated memory and exit
  *
  * @param err error code
  * @return void
  */
void exit_error(int err);

#endif //ERROR_H
