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

#include "enums.h"

//lex.h si vytvor sam..bude tam asi len deklaracia funckie get_token 
//vytvorim asi nejaky subor types.h kde bude ta struktura pre token
//
//v nejakej funkci set token type porovnas len s tymito hodnotami,
//toto pole daj len ako lokalnu premennu funkcie, ak sa to nebude zhodovat,
//tak nacitane slovo je len obycajny identifikator token_identifier
//tie token typy su v enums.h

#define KEYWORDS_COUNT 15

char *token_keywords[] = {"auto", "cin", "cout", "double", "else",
                          "for", "if", "int", "return", "string", 
                          "length", "substr", "concat", "find", "sort"};
