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

/**
  * @file expr.h
  * @brief Expression library
  *
  * Tadlenc vìc je pro vyhodnocování výrazù
  *
  */

#ifndef EXPR_H
#define EXPR_H

typedef enum {
	 oper_add,
	 oper_sub,
	 oper_mul,
	 oper_div,
	 oper_lr_bracket,
	 oper_rr_bracket,
	 oper_id,
	 oper_less,
	 oper_greater,
	 oper_less_e,
	 oper_greater_e,
	 oper_equal,
	 oper_not_equal
} ex_operator;

/**
  * @brief Funkce pro vyhodnocovani vyrazu
  *
  *  Bude mit dva parametry, navratovou promennou a aktualniho instrukcniho listu
  */
void expression();

#endif //EXPR_H
