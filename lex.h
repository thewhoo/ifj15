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

#include "adt.h"

#ifndef LEX_H
#define LEX_H

#define STR_DEFAULT_SIZE 20
#define KEYWORDS_COUNT 15

typedef enum
{
	S_START,			//start
	S_ERROR,			//error

	S_DIV,				// /
	S_LBC,				// /*
	S_RBC,				// /* *
	S_LCOM,				// //
	S_LESS,				// <
	S_GREAT,			// >
	S_ASSIGN,			// =
	S_EXCM,				// !
	S_QUOT,				// "
	S_ESCAPE,			// esc sequence
	S_HEX_NUMBER,		// hex number

	S_INT,				// int
	S_DOT,				// int.
	S_DBL,				// double
	S_EXPO_E,			// base^e
	S_EXPO_M,			// base^e+-
	S_EXPO,				// base^e+-int

	S_IDENT,			// indentifier

} States;

void lex_init(FILE *f);

/**
  * @brief Returns one token back to the "tokens stream"
  *
  * If you return more tokens back, only last will be remembered
  *
  * @param token Pointer to token 
  */

char hex_to_ascii(char first, char second);

void unget_token(TToken *token);

TToken* get_token();

#endif // LEX_H
