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

#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "galloc.h"

void exit_error(int err)
{
    switch(err)
    {
        case(E_LEX):
            fprintf(stderr, "IFJ15 Error: lexical error!\n");
            break;
        case(E_SYNTAX):
            fprintf(stderr, "IFJ15 Error: syntax error!\n");
            break;
        case(E_SEMANTIC_DEF):
            fprintf(stderr, "IFJ15 Error: Some semantic error!\n");
            break;
        case(E_SEMANTIC_TYPES):
            fprintf(stderr, "IFJ15 Error: Semantic type error!\n");
            break;
        case(E_AUTO_TYPE):
            fprintf(stderr, "IFJ15 Error: No idea about type for 'auto' variable!\n");
            break;
        case(E_SEMANTIC_OTHERS):
            fprintf(stderr, "IFJ15 Error: Some semantic error!\n");
            break;
        case(E_READ_NUMBER):
            fprintf(stderr, "IFJ15 Error: Cin: Fail to read number from stdin!\n");
            break;
        case(E_UNINITIALIZED):
            fprintf(stderr, "IFJ15 Error: Operation with unitialized var or missing return at the end of function!\n");
            break;
        case(E_ZERO_DIVISION):
            fprintf(stderr, "IFJ15 Math error: Zero division!\n");
            break;
        case(E_RUNTIME_OTHERS):
            fprintf(stderr, "IFJ15 Error: Interpret: runtime failure\n");
            break;
        case(E_INTERNAL):
            fprintf(stderr, "IFJ15 Error: Something went wrong!\n");
            break;
        case(E_ALLOC):
            fprintf(stderr, "IFJ15 Error: Memory allocation failed!\n");
            err = E_INTERNAL;
            break;

    }

    //gcDestroy();
    exit(err);
}
