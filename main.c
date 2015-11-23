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
#include <stdlib.h>

#include "error.h"
#include "galloc.h"
#include "interpret.h"
#include "lex.h"
#include "parser.h"
#include "shared.h"
#include "expr.h"

int main(int argc, char **argv)
{

    if (argc < 2)
        exit_error(E_INTERNAL);

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit_error(E_INTERNAL);

    gcInit();

    lex_init(fp);

    global_init();
    
    expr_init();

    parse();

    fclose(fp);

    interpret();

    gcDestroy();

    return 0;
}

