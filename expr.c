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
#include "stack.h"
#include "galloc.h"
#include "expr.h"
#include "lex.h"
#include "enums.h"
#include "error.h"
#include "ilist.h"
#include "adt.h"
#include "ial.h"
#include "interpret.h"
#include "shared.h"

#define RULE_COUNTER 13
#define EQ 0
#define LO 1
#define HI 2
#define ER 3
#define DEB_INFO 1
#define DEB_ERROR_PRINT 1

/* External functions */
void expression(TVariable *var_from_parser, Tins_list *ins_list_to_fill);
/* Internal functions */
void expr_init(TVariable *var_from_parser, Tins_list *ins_list_to_fill);
void infix_2_postfix();
void transfer_rest_of_expr_stack();
int stack_lower_prio(TToken *token_in, TToken *token_stack);
int tok_prece_pos(int n);
int its_function();
void function_elaboration();
void postfix_count_test();
int token_is_operator(TToken *tok);
int token_is_operand(TToken *tok);
void generate_code();
void operand_check(TToken *tok);
int ope_type_2_ins_type(int operator_type);
TVariable *find_var(TToken *tok);
void operand_type_checker(int ins_type, TVariable *var_1, TVariable *var_2);
int t_compare(TVariable *var, int type);
TList_item *create_ins(int type, TVariable *addr1, TVariable *addr2, TVariable *addr3);
void charlady();
/* Debug functions */
void stack_print(TStack *st);

/* Global variables */
const char prece_table[RULE_COUNTER][RULE_COUNTER] = {/* Sloupec tok_in, řádek tok_stack */
/*         +   -   *   /   (   )   id  <   >   <=  >=  ==  !=      */
/* +  */ { HI, HI, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* -  */ { HI, HI, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* *  */ { HI, HI, HI, HI, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* /  */ { HI, HI, HI, HI, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* (  */ { LO, LO, LO, LO, LO, EQ, LO, LO, LO, LO, LO, LO, LO },
/* )  */ { HI, HI, HI, HI, ER, HI, ER, HI, HI, HI, HI, HI, HI },
/* id */ { HI, HI, HI, HI, ER, HI, ER, HI, HI, HI, HI, HI, HI },
/* <  */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* >  */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* <= */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* >= */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* == */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* != */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
};
TStack *expr_stack;
TStack *gene_stack;
Tins_list *actual_ins_list;
TVariable *expr_var;
TStack *ins_stack;

/*
***	TODO
	Zpracovani funkci
	Zkontrolovat konstanty vracenych exit_error
	Proměnné hledat v aktuálním stacu, ne v globálu
	Uklízet po sobě

***	POZNAMKY
	inicializovane promenne?
	kontrola jestli funkce defined?
*/

void expression(TVariable *var_from_parser, Tins_list *ins_list_to_fill)
{
	if (DEB_INFO) {
		printf("EXPR_INFO START!\n");
		TToken *tok = get_token();
		printf("EXPR_INFO First token is %d %s\n", tok->type, tok->data);
		unget_token(tok);
	}

	expr_init(var_from_parser, ins_list_to_fill);
	if (its_function()) {
		function_elaboration();
	} else {
		infix_2_postfix();
		generate_code();
	}

	if (DEB_INFO) printf("EXPR_INFO STOP!\n");
}

void expr_init(TVariable *var_from_parser, Tins_list *ins_list_to_fill)
{
	expr_var = var_from_parser;
	actual_ins_list = ins_list_to_fill;
	expr_stack = stack_init();
	gene_stack = stack_init();
	ins_stack = stack_init();
}

void infix_2_postfix()
{
	TToken *tok_in;
	TToken *tok_stack;
	int bracket_counter;

	bracket_counter = 0;
	tok_in = get_token();
	while ((tok_in->type != TOKEN_SEMICOLON) && (bracket_counter >= 0)) {
		switch (tok_in->type) {
			case TOKEN_INT_VALUE:
			case TOKEN_DOUBLE_VALUE:
			case TOKEN_STRING_VALUE:
			case TOKEN_IDENTIFIER:
				stack_push(gene_stack, tok_in);
				break;
			case TOKEN_LROUND_BRACKET:
				stack_push(expr_stack, tok_in);
				bracket_counter++;
				break;
			case TOKEN_RROUND_BRACKET:
				tok_stack = stack_top(expr_stack);
				stack_pop(expr_stack);
				while (tok_stack->type != TOKEN_LROUND_BRACKET) {
					stack_push(gene_stack, tok_stack);
					tok_stack = stack_top(expr_stack);
					stack_pop(expr_stack);
				}
				bracket_counter--;
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
			tok_stack = stack_top(expr_stack);
				if (stack_empty(expr_stack) || stack_lower_prio(tok_in, tok_stack)) {
					stack_push(expr_stack, tok_in);
				} else {

					while (!stack_empty(expr_stack) && !stack_lower_prio(tok_in, tok_stack)) {
						stack_push(gene_stack, tok_stack);
						stack_pop(expr_stack);
						tok_stack = stack_top(expr_stack);
					}
					stack_push(expr_stack, tok_in);
				}
				break;
		}
		tok_in = get_token();
	}
	transfer_rest_of_expr_stack();
	if (DEB_INFO) {
		printf("EXPR_INFO expr_stack\n");
		stack_print(expr_stack);
		printf("EXPR_INFO gene_stack\n");
		stack_print(gene_stack);
		printf("EXPR_INFO Unget %d %s\n", tok_in->type, tok_in->data);
	}
	unget_token(tok_in);
}

void transfer_rest_of_expr_stack()
{
	while (!stack_empty(expr_stack)) {
		stack_push(gene_stack, stack_top(expr_stack));
		stack_pop(expr_stack);
	}
}

int stack_lower_prio(TToken *token_in, TToken *token_stack)
{
	int x;
	int y;
	int val;

	x = tok_prece_pos(token_in->type);
	y = tok_prece_pos(token_stack->type);
	switch (prece_table[y][x]) {
		case HI:
		case EQ:
			val = 0;
			break;
		case LO:
			val = 1;
			break;
		case ER:
			exit_error(E_SYNTAX);
	}

	return val;
}

int tok_prece_pos(int n)
{
	int val;

	switch (n) {
		case TOKEN_ADD:
			val = oper_add;
			break;
		case TOKEN_SUB:
			val = oper_sub;
			break;
		case TOKEN_MUL:
			val = oper_mul;
			break;
		case TOKEN_DIV:
			val = oper_div;
			break;
		case TOKEN_LROUND_BRACKET:
			val = oper_lr_bracket;
			break;
		case TOKEN_RROUND_BRACKET:
			val = oper_rr_bracket;
			break;
		case TOKEN_IDENTIFIER:
			val = oper_id;
			break;
		case TOKEN_LESS:
			val = oper_less;
			break;
		case TOKEN_GREATER:
			val = oper_greater;
			break;
		case TOKEN_LESS_EQUAL:
			val = oper_less_e;
			break;
		case TOKEN_GREATER_EQUAL:
			val = oper_greater_e;
			break;
		case TOKEN_EQUAL:
			val = oper_equal;
			break;
		case TOKEN_NOT_EQUAL:
			val = oper_not_equal;
			break;
	}

	return val;
}

int its_function()
{
	int yes_it_is;
	TToken *tok;
	htab_item *item;

	tok = get_token();
	switch (tok->type) {
		case TOKEN_IDENTIFIER:
			item = htab_lookup(G.g_globalTab, tok->data);
			if ((item == NULL) || (item->data.function == NULL) || (item->data.function->defined == 0)) {
				if (DEB_ERROR_PRINT) printf("EXPR_ERR! Funkce nenalezena\n");
				exit_error(E_SEMANTIC_DEF);
			} else {
				yes_it_is = 1;
			}
			break;
		case TOKEN_BF_LENGTH:
		case TOKEN_BF_SUBSTR:
		case TOKEN_BF_CONCAT:
		case TOKEN_BF_FIND:
		case TOKEN_BF_SORT:
			yes_it_is = 1;
			break;
		default:
			yes_it_is = 0;
	}
	unget_token(tok);

	return yes_it_is;
}

void function_elaboration()
{
	printf("EXPR Prijata funkce!\n");
}

void stack_print(TStack *st)
{
	TToken *tok;
	TStack *cache_stack;

	cache_stack = stack_init();
	while (!stack_empty(st)) {
		tok = stack_top(st);
		printf("EXPE_INFO Token %d %s\n", tok->type, tok->data);
		stack_push(cache_stack, tok);
		stack_pop(st);
	}
	while (!stack_empty(cache_stack)) {
		tok = stack_top(cache_stack);
		stack_push(st, tok);
		stack_pop(cache_stack);
	}
	stack_free(cache_stack);
}

void generate_code()
{
	TToken *tok;
	TList_item *actual_ins;
	TVariable *var_1;
	TVariable *var_2;
	TVariable *var_to_push;

	postfix_count_test();
	while (gene_stack->used > 1) {
		tok = stack_top(gene_stack);
		stack_pop(gene_stack);
		if (token_is_operand(tok)) {
			var_to_push = find_var(tok);
			stack_push(ins_stack, var_to_push);
		} else {
			var_1 = stack_top(ins_stack);
			stack_pop(ins_stack);
			var_2 = stack_top(ins_stack);
			stack_pop(ins_stack);
			operand_type_checker(tok->type, var_1, var_2);
			actual_ins = create_ins(ope_type_2_ins_type(tok->type), G.g_return, var_2, var_1);
			list_insert(actual_ins_list, actual_ins);
			stack_push(ins_stack, G.g_return);
		}
	}
	actual_ins = create_ins(INS_ASSIGN, expr_var, stack_top(ins_stack), NULL);
	list_insert(actual_ins_list, actual_ins);
}

TVariable *find_var(TToken *tok)
{
	TVariable *new_var;
	htab_item *h_item;

	if (tok->type == TOKEN_IDENTIFIER) {
		h_item = htab_lookup(G.g_globalTab, tok->data);
		if (h_item == NULL) {
			if (DEB_ERROR_PRINT) printf("EXPR_ERR! Nepouzitelna promenna\n");
			exit_error(E_SEMANTIC_DEF);
		} else {
			new_var = h_item->data.variable;
		}
	} else {
		h_item = htab_lookup(G.g_constTab, tok->data);
		if (h_item == NULL) {
			htab_item *new_const = htab_insert(G.g_constTab, tok->data);
			new_const->data.variable = token_to_const(tok);
			new_var = new_const->data.variable;
		} else {
			new_var = h_item->data.variable;
		}
	}

	return new_var;
}

void postfix_count_test()
{
	int operator_counter;
	int operand_counter;
	TToken *tok;
	TStack *cache_stack;

	operator_counter = 0;
	operand_counter = 0;
	while (!stack_empty(gene_stack)) {
		tok = stack_top(gene_stack);
		operator_counter += token_is_operator(tok);
		operand_counter += token_is_operand(tok);
		stack_pop(gene_stack);
		stack_push(expr_stack, tok);

	}
	cache_stack = gene_stack;
	gene_stack = expr_stack;	/* Yummy! Yummy! */
	expr_stack = cache_stack;
	if (++operator_counter != operand_counter) {
		if (DEB_ERROR_PRINT) printf("EXPR_ERR! Nesouhlasi pocet operatoru a operandu\n");
		exit_error(E_SEMANTIC_DEF);
	}
}

int token_is_operator(TToken *tok)
{
	switch (tok->type) {
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
			return 1;
			break;
	}

	return 0;
}

int token_is_operand(TToken *tok)
{
	switch (tok->type) {
		case TOKEN_IDENTIFIER:
		case TOKEN_STRING_VALUE:
		case TOKEN_INT_VALUE:
		case TOKEN_DOUBLE_VALUE:
			return 1;
			break;
	}

	return 0;
}

int ope_type_2_ins_type(int operator_type)
{
	int val;

	switch (operator_type) {
		case TOKEN_MUL:
			val = INS_MUL;
			break;
		case TOKEN_DIV:
			val = INS_DIV;
			break;
		case TOKEN_ADD:
			val = INS_ADD;
			break;
		case TOKEN_SUB:
			val = INS_SUB;
			break;
		case TOKEN_EQUAL:
			val = INS_EQ;
			break;
		case TOKEN_NOT_EQUAL:
			val = INS_NEQ;
			break;
		case TOKEN_GREATER:
			val = INS_GREATER;
			break;
		case TOKEN_GREATER_EQUAL:
			val = INS_GREATEQ;
			break;
		case TOKEN_LESS:
			val = INS_LESSER;
			break;
		case TOKEN_LESS_EQUAL:
			val = INS_LESSEQ;
			break;
	}

	return val;
}

void operand_type_checker(int operator_type, TVariable *var_1, TVariable *var_2)
{
	/* Operace mezi retezcem a neretezcem */
	if (t_compare(var_1, TYPE_STRING) && !t_compare(var_2, TYPE_STRING)) {
		if (DEB_ERROR_PRINT) printf("EXPR_ERR! Operace meti neretezcem a retezcem\n");
		exit_error(E_SEMANTIC_TYPES);
	}

	switch (operator_type) {
		case TOKEN_MUL:
		case TOKEN_DIV:
		case TOKEN_ADD:
		case TOKEN_SUB:
			/* Aritmeticke operace nad retezci */
			if (t_compare(var_1, TYPE_STRING) || t_compare(var_2, TYPE_STRING)) {
				if (DEB_ERROR_PRINT) printf("EXPR_ERR! Aritmeticke operace nad retezci\n");
				exit_error(E_SEMANTIC_TYPES);
			}
	}
}

int t_compare(TVariable *var, int type)
{
	return (var->var_type == type);
}

TList_item *create_ins(int type, TVariable *addr1, TVariable *addr2, TVariable *addr3)
{
	TList_item *ins;

	ins = gmalloc(sizeof(TList_item));
	ins->ins_type = type;
	ins->addr1 = addr1;
	ins->addr2 = addr2;
	ins->addr3 = addr3;

	return ins;
}

void charlady()
{
	stack_free(expr_stack);
	stack_free(gene_stack);
	stack_free(ins_stack);
}
