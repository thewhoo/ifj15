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

#include <stdlib.h>
#include "ial.h"
#include "stack.h"
#include "shared.h"
#include "galloc.h"


struct TGlobal G;

void global_init()
{
	G.g_constTabStr = htab_init(HTAB_SIZE);
	G.g_constTabNum = htab_init(HTAB_SIZE);
	G.g_globalTab = htab_init(HTAB_SIZE);
	G.g_frameStack = stack_init();
}

