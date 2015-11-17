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
#include <string.h>
#include "stack.h"
#include "galloc.h"
#include "expr.h"
#include "lex.h"
#include "enums.h"
#include "error.h"
#include "adt.h"
#include "ial.h"

#define RULE_COUNTER 13
#define EQ 0
#define LO 1
#define HI 2
#define ER 3

void expression(TVariable* variable_toilet, Tins_list* list_toilet);
void generate_code(TToken* tok);
int expr_end(TToken* tok);
int token_to_pos(int n);
int stack_lower_prio(int x_type, int y_type);
char *type_to_text(int type);
void stack_flush();
void type_checker(TToken* var1, TToken* var2);

/* TODO Zpracovani funkci  */

const char prece_table[RULE_COUNTER][RULE_COUNTER] = {
/* Sloupce představují vstupní tokeny, řádky představují tokeny na zásobníku */
/* Rozšiřovat vlevo a dole!! */
/*			 	+		-		*		/		(		)		id		<		>		<=		>=		==		!=		*/
/*	+	*/	{	HI,	HI,	LO,	LO,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	-	*/	{	HI,	HI,	LO,	LO,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	*	*/	{	HI,	HI,	HI,	HI,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	/	*/	{	HI,	HI,	HI,	HI,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	(	*/	{	LO,	LO,	LO,	LO,	LO,	EQ,	LO,	LO,	LO,	LO,	LO,	LO,	LO	},
/*	)	*/	{	HI,	HI,	HI,	HI,	ER,	HI,	ER,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	id	*/	{	HI,	HI,	HI,	HI,	ER,	HI,	ER,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	<	*/	{	LO,	LO,	LO,	LO,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	>	*/	{	LO,	LO,	LO,	LO,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	<=	*/	{	LO,	LO,	LO,	LO,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	>=	*/	{	LO,	LO,	LO,	LO,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	==	*/	{	LO,	LO,	LO,	LO,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
/*	!=	*/	{	LO,	LO,	LO,	LO,	LO,	HI,	LO,	HI,	HI,	HI,	HI,	HI,	HI	},
};
TStack* expr_stack; /* INFIX->POSTFIX stack*/
TStack* gene_stack; /* Stack for instruction generating */

void expression(TVariable* variable_toilet, Tins_list* list_toilet)
{
	TToken* token_act; /* Token from lexer */
	TToken* token_top; /* Token from expr_stack */

	expr_stack = stack_init();
	gene_stack = stack_init();
	token_act = get_token();

	while (expr_end(token_act)) {
		switch (token_act->type) {
			case TOKEN_INT_VALUE:
			case TOKEN_DOUBLE_VALUE:
			case TOKEN_IDENTIFIER:
				/* TODO kontrola existence promennych a konstant, pripadne jejich tvorba */
				/* TODO nejedna se o funkci?! */
				generate_code(token_act);
				break;
			case TOKEN_LROUND_BRACKET:
				stack_push(expr_stack, token_act);
				break;
			case TOKEN_RROUND_BRACKET:
				token_top = stack_top(expr_stack);
				while (token_top->type != TOKEN_LROUND_BRACKET) {
					generate_code(token_top);
					stack_pop(expr_stack);
					token_top = stack_top(expr_stack);
				}
				stack_pop(expr_stack);
				break;
			case TOKEN_MUL:
			case TOKEN_DIV:
			case TOKEN_ADD:
			case TOKEN_SUB:
			case TOKEN_EQUAL:
			case TOKEN_NOT_EQUAL:
			case TOKEN_GREATER:
			case TOKEN_GREATER_EQUAL:
			case TOKEN_LESS:
			case TOKEN_LESS_EQUAL:
				token_top = stack_top(expr_stack);
				if (stack_empty(expr_stack) || stack_lower_prio(token_act->type, token_top->type)) {
					stack_push(expr_stack, token_act);
				} else {
					while (!stack_empty(expr_stack) && !stack_lower_prio(token_act->type, token_top->type)) {
						generate_code(token_top);
						stack_pop(expr_stack);
						token_top = stack_top(expr_stack);
					}
					stack_push(expr_stack, token_act);
				}
				break;
		}
		token_act = get_token();
	}

	stack_flush(expr_stack);
int navrat =token_act->type;
  token_act->type = TOKEN_EOF; /* Killing token */
	generate_code(token_act);

token_act->type = navrat;
unget_token(token_act);
	stack_free(expr_stack);
	stack_free(gene_stack);
}

void generate_code(TToken* tok)
{
	 TToken* var1;
	 TToken* var2;

	if (tok->type == TOKEN_EOF) {
		tok = stack_top(gene_stack); /* INS_RET */
		stack_pop(gene_stack);
		printf("INS_RET %s\n", tok->data);
		return;
	}

	if (tok->type >= 34) {
		var1 = stack_top(gene_stack);
		stack_pop(gene_stack);
		var2 = stack_top(gene_stack);
		stack_pop(gene_stack);
		type_checker(var1, var2);
		switch (tok->type) {
			/* TODO tvorba instrukci */
			case 34:
				printf("INS_MUL XX, %s, %s\n", var2->data, var1->data);
				break;
			case 35:
				printf("INS_DIV XX, %s, %s\n", var2->data, var1->data);
				break;
			case 36:
				printf("INS_ADD XX, %s, %s\n", var2->data, var1->data);
				break;
			case 37:
				printf("INS_SUB XX, %s, %s\n", var2->data, var1->data);
				break;
			case 38:
				printf("INS_EQ XX, %s, %s\n", var2->data, var1->data);
				break;
			case 39:
				printf("INS_NEQ XX, %s, %s\n", var2->data, var1->data);
				break;
			case 40:
				printf("INS_GREATER XX, %s, %s\n", var2->data, var1->data);
				break;
			case 41:
				printf("INS_GREATEQ XX, %s, %s\n", var2->data, var1->data);
				break;
			case 42:
				printf("INS_LESSER XX, %s, %s\n", var2->data, var1->data);
				break;
			case 43:
				printf("INS_LESSEQ XX, %s, %s\n", var2->data, var1->data);
				break;
		  }
		  strcat(var2->data, var1->data);
		  stack_push(gene_stack, var2);
	 } else {
		  stack_push(gene_stack, tok);
	 }
}

int expr_end(TToken* tok)
{
	int stoper= 1;

	if (tok->type == TOKEN_EOF) {
		stoper = 0;
	}
	if (tok->type == TOKEN_SEMICOLON) {
		stoper = 0;
	}

	return stoper;
}

/* vrchol zásobníku menší prioritu než vstupní token */
/* x_type vstupní token, y_type vrchol zásobníku*/
int stack_lower_prio(int x_type, int y_type)
{
	int x;
	int y;
	int val;

	x = token_to_pos(x_type);
	y = token_to_pos(y_type);
	switch (prece_table[y][x]) {
		case HI:
		case EQ:
			val = 0;
			break;
		case LO:
			val = 1;
			break;
		case ER:
			exit_error(3);
	 }

	return val;
}

/* hledání typu tokenu v precedenční tabulce */
int token_to_pos(int n)
{
	switch (n) {
		case TOKEN_ADD:
			return oper_add;
		case TOKEN_SUB:
			return oper_sub;
		case TOKEN_MUL:
			return oper_mul;
		case TOKEN_DIV:
			return oper_div;
		case TOKEN_LROUND_BRACKET:
			return oper_lr_bracket;
		case TOKEN_RROUND_BRACKET:
			return oper_rr_bracket;
		case TOKEN_IDENTIFIER:
			return oper_id;
		case TOKEN_LESS:
			return oper_less;
		case TOKEN_GREATER:
			return oper_greater;
		case TOKEN_LESS_EQUAL:
			return oper_less_e;
		case TOKEN_GREATER_EQUAL:
			return oper_greater_e;
		case TOKEN_EQUAL:
			return oper_equal;
		case TOKEN_NOT_EQUAL:
			return oper_not_equal;
	}

	return 99; /* TODO vraceni 99? */
}

/* vysypání zbytků ze zásobníku */
void stack_flush()
{
	 TToken* tok;

	 while (!stack_empty(expr_stack)) {
		  tok = stack_top(expr_stack);
		  generate_code(tok);
		  stack_pop(expr_stack);
	 }
}

void type_checker(TToken* var1, TToken* var2)
{
	int problem = 0;;

	if (var1->type == 99) problem = 1;
	if (var2->type == 99) problem = 1;

	if (problem) {
		exit_error(4);
	}
}

