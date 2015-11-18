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
  TOKEN_AUTO = 0,   //0
  TOKEN_CIN,        //1
  TOKEN_COUT,       //2
  TOKEN_DOUBLE,     //3
  TOKEN_ELSE,       //4
  TOKEN_FOR,        //5
  TOKEN_IF,         //6
  TOKEN_INT,        //7
  TOKEN_RETURN,     //8
  TOKEN_STRING,     //9

  TOKEN_LENGTH,     //10
  TOKEN_SUBSTR,     //11
  TOKEN_CONCAT,     //12
  TOKEN_FIND,       //13
  TOKEN_SORT,       //14

//built-in functions
  TOKEN_BF_LENGTH,  //15
  TOKEN_BF_SUBSTR,  //16
  TOKEN_BF_CONCAT,  //17
  TOKEN_BF_FIND,    //18
  TOKEN_BF_SORT,    //19

//other tokens
  TOKEN_IDENTIFIER,     //20    //IDENTIFIER, NOT KEYWORD
  TOKEN_EOF,            //21
  TOKEN_STRING_VALUE,   //22    // "some text in quotation marks"
  TOKEN_INT_VALUE,      //23    // some integer
  TOKEN_DOUBLE_VALUE,   //24    // double value
  TOKEN_ASSIGN,         //25    // =        e.g.(a = 3;)

  TOKEN_SEMICOLON,      //26    // ;
  TOKEN_COMMA,          //27    // ,
  TOKEN_COUT_BRACKET,   //28    // <<
  TOKEN_CIN_BRACKET,    //29    // >>
  TOKEN_LROUND_BRACKET, //30    // (
  TOKEN_RROUND_BRACKET, //31    // )
  TOKEN_LCURLY_BRACKET, //32    // {
  TOKEN_RCURLY_BRACKET, //33    // }

//math 
  TOKEN_MUL,            //34    // *
  TOKEN_DIV,            //35    // /
  TOKEN_ADD,            //36    // +
  TOKEN_SUB,            //37    // -

//comparing
  TOKEN_EQUAL,          //38    // ==
  TOKEN_NOT_EQUAL,      //39    // !=
  TOKEN_GREATER,        //40    // >
  TOKEN_GREATER_EQUAL,  //41    // >=
  TOKEN_LESS,           //42    // < 
  TOKEN_LESS_EQUAL      //43    // <= 
};

#endif
