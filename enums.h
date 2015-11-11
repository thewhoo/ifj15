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

#ifndef ENUMS_H
#define ENUMS_H

#define VECTOR_AUTORESIZE_COEF 2

enum tokens
{
//keywords 
  TOKEN_AUTO = 0, 
  TOKEN_CIN,
  TOKEN_COUT,
  TOKEN_DOUBLE,
  TOKEN_ELSE,
  TOKEN_FOR,
  TOKEN_IF,
  TOKEN_INT,
  TOKEN_RETURN,
  TOKEN_STRING,

  TOKEN_LENGTH,
  TOKEN_SUBSTR,
  TOKEN_CONCAT,
  TOKEN_FIND,
  TOKEN_SORT,

//built-in functions
  TOKEN_BF_LENGTH,
  TOKEN_BF_SUBSTR,
  TOKEN_BF_CONCAT,
  TOKEN_BF_FIND,
  TOKEN_BF_SORT,  

//other tokens
  TOKEN_IDENTIFIER,         //IDENTIFIER, NOT KEYWORD
  TOKEN_EOF,
  TOKEN_STRING_VALUE,        // "some text in quotation marks"
  TOKEN_INT_VALUE,          // some integer
  TOKEN_DOUBLE_VALUE,       // double value
  TOKEN_ASSIGN,             // =        e.g.(a = 3;)

  TOKEN_SEMICOLON,          // ;
  TOKEN_COMMA,              // ,
  TOKEN_COUT_BRACKET,       // <<
  TOKEN_CIN_BRACKET,        // >>
  TOKEN_LROUND_BRACKET,     // (
  TOKEN_RROUND_BRACKET,     // )
  TOKEN_LCURLY_BRACKET,     // {
  TOKEN_RCURLY_BRACKET,     // }

//math 
  TOKEN_MUL,                // *
  TOKEN_DIV,                // /
  TOKEN_ADD,                // +
  TOKEN_SUB,                // -

//comparing
  TOKEN_EQUAL,              // ==
  TOKEN_NOT_EQUAL,          // !=
  TOKEN_GREATER,            // >
  TOKEN_GREATER_EQUAL,      // >=
  TOKEN_LESS,               // < 
  TOKEN_LESS_EQUAL          // <= 
};

#endif
