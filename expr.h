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

/**
  * @file expr.h
  * @brief Expression library
  */

#ifndef EXPR_H
#define EXPR_H

#include "adt.h"
#include "ilist.h"
#include <stdbool.h>

enum {
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
};

enum {
	EQ,
	LO,
	HI,
	ER
};

enum {
	not_function,
	internal_function,
	external_function
};

/**
  * @brief Inicializace modulu
  *
  * Inicializace zasobniku potrebnych pri praci modulu
  */
void expr_init();

/**
  * @brief Deinicializace modulu
  *
  * Uvolneni zasobniku potrebnych pri praci modulu
  */
void expr_destroy();

/**
  * @brief Zpracovani vyrazu
  *
  * Funkce pro zpracovani vyrazu, generuje instrukce a vraci rizeni
  *
  * @param variable_toilet Ukazatel na promennou pro prirazeni vysledku vyrazu
  * @param list_toilet Ukazatel na aktulani instrukcni list pro vlozeni vygenerovanych instrukci
  */
void expression(TVariable *ret_var, Tins_list *act_ins_list);

#endif //EXPR_H

