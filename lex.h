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

#include "adt.h"

#ifndef LEX_H
#define LEX_H

#define STR_DEFAULT_SIZE 20
#define KEYWORDS_COUNT 15


void lex_init(FILE *f);

/**
  * @brief Returns one token back to the "tokens stream"
  *
  * If you return more tokens back, only last will be remembered
  *
  * @param token Pointer to token
  */

void unget_token(TToken *tok);

TToken* get_token();

#endif // LEX_H
