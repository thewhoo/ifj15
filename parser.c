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

 #include <stdio.h>
 #include "parser.h"
 #include "adt.h"
 #include "lex.h"
 #include "galloc.h"
 #include "enums.h"
 // Just a skeleton ATM, I will do something when I
 // figure out how the fucking LL tables work

 void parse()
 {
     TToken* token;

     do
     {
         token = get_token();
         printf("Token type: %d\nToken data: %s\n", token->type, token->data);

     }while(token->type != TOKEN_EOF);
 
 }
