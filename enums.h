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

enum error_codes
{
  E_OK,     //0  
  E_LEX,    //1
  E_SYNTAX,
  E_SEMANTIC_DEF,
  E_SEMANTIC_TYPES,
  E_AUTO_TYPE,
  E_SEMANTIC_OTHERS,
  E_READ_NUMBER,
  E_UNITIALIZED,
  E_ZERO_DIVISION,
  E_RUNTIME_OTHERS,
  E_INTERNAL = 99, //return values defined in project description

  //local 
  E_ALLOC

};

typedef enum tokens
{
//keywords - add main funciotn as keyword? 
  TOKEN_AUTO,
  TOKEN_CIN,
  TOKEN_COUT,
  TOKEN_DOUBLE,
  TOKEN_ELSE,
  TOKEN_FOR,
  TOKEN_IF,
  TOKEN_INT,
  TOKEN_RETURN,
  TOKEN_STRING,

//built-in functions
  TOKEN_BF_LENGTH,
  TOKEN_BF_SUBSTR,
  TOKEN_BF_CONCAT,
  TOKEN_BF_FIND,
  TOKEN_BF_SORT,  

//other tokens
  TOKEN_IDENTIFIER,         //IDENTIFIER, NOT KEYWORD
  TOKEN_EOF,
  TOKEN_TEXT,               // "some text in quotation marks"
  TOKEN_SEMICOLON,          // ;
  TOKEN_COMMA,              // ,
  TOKEN_COUT_BRACKET,       // <<
  TOKEN_CIN_BRACKET,        // >>
  TOKEN_LROUND_BRACKET,     // (
  TOKEN_RROUND_BRACKET,     // )
  TOKEN_LCURLY_BRACKET,     // {
  TOKEN_RCURLYBRACKET,      // }

//math 
  TOKEN_MUL,                // *
  TOKEN_DIV,                // /
  TOKEN_ADD,                // +
  TOKEN_SUB,                // -

//comparing
  TOKEN_EQUAL,              // =
  TOKEN_NOT_EQUAL,          // !=
  TOKEN_GREATER,            // >
  TOKEN_GREATER_EQUAL,      // >=
  TOKEN_LESS,               // < 
  TOKEN_LESS_EQUAL          // <= 



};

#endif
