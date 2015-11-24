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

#ifndef SHARED_H
#define SHARED_H

#include "ial.h"
#include "stack.h"

struct TGlobal
{
  // Global ConstTab for strings and numbers
  htab_t *g_constTabStr;
  htab_t *g_constTabNum;

  // Global GlobalTab
  htab_t *g_globalTab;
  
  // Global ExprTab
  htab_t *g_exprTab;

  // Global FrameStack
  TStack *g_frameStack;
};

extern struct TGlobal G;

void global_init();

#endif // SHARED_H
