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

#include <stdlib.h>
#include "ial.h"
#include "stack.h"
#include "shared.h"
#include "galloc.h"


struct TGlobal G;

void global_init()
{
  G.g_constTab = htab_init(HTAB_SIZE);
	G.g_globalTab = htab_init(HTAB_SIZE);
	G.g_frameStack = stack_init();
  G.g_return = gmalloc(sizeof(TVariable));
  G.g_return->var_type = 0;
  G.g_return->name = "GLOBAL_RET";
  G.g_return->initialized = 0;
  G.g_return->constant = 1;
}
