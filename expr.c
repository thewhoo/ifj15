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
#include <stdbool.h>
#include <string.h>
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

const char prece_table[RULE_COUNTER][RULE_COUNTER] = {
/*         +   -   *   /   (   )   id  <   >   <=  >=  ==  !=	 */
/* +  */ { HI, HI, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* -  */ { HI, HI, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* *  */ { HI, HI, HI, HI, LO, HI, LO, HI, HI, HI, HI, HI, HI }, /* Z */
/* /  */ { HI, HI, HI, HI, LO, HI, LO, HI, HI, HI, HI, HI, HI }, /* a */
/* (  */ { LO, LO, LO, LO, LO, EQ, LO, LO, LO, LO, LO, LO, LO }, /* s */
/* )  */ { HI, HI, HI, HI, ER, HI, ER, HI, HI, HI, HI, HI, HI }, /* o */
/* id */ { HI, HI, HI, HI, ER, HI, ER, HI, HI, HI, HI, HI, HI }, /* b */
/* <  */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI }, /* n */
/* >  */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI }, /* i */
/* <= */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI }, /* k */
/* >= */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* == */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
/* != */ { LO, LO, LO, LO, LO, HI, LO, HI, HI, HI, HI, HI, HI },
};										/* Vstupni tokeny */
TStack *expr_stack;
TStack *gene_stack;
TStack *ins_stack;

/*
TODO
	Zpracovani funkci
	Typ auto
	Automatické konverze
	Kontrola typu výstupu přiřazení
	Zkontrolovat konstanty vracenych exit_error
		pri funci kontrolovat parametry, pushovat (pres instrukci) a pak volat call
	Uklízet po sobe
POZNAMKY
	Breaks vs. Return
*/

void charlady()
{
	stack_free(expr_stack);
	stack_free(gene_stack);
	stack_free(ins_stack);
}

char *create_t_name(const int *number)
{
	char *s;

	s = gmalloc(sizeof(char)*6);	/* Maximálně 1 000 000 proměnných výrazu*/
	sprintf(s, "T%d", *number);

	return s;
}

TVariable* next_t_var(int t_x_type, int *t_x_var_counter)
{
	htab_item *h_item;
	TVariable* var;
	char *t_name;

	t_name = create_t_name(t_x_var_counter);
	h_item = htab_lookup(G.g_exprTab, t_name);
	if(h_item == NULL) {
		var = gmalloc(sizeof(TVariable));
		var->initialized = 1;
		var->constant = 1;
		var->name = gmalloc(strlen(t_name) + 1);
		strcpy(var->name, t_name);
		var->var_type = t_x_type;
		h_item = htab_insert(G.g_exprTab, t_name);
		h_item->data.variable = var;
		(*t_x_var_counter)++;

		return var;
	} else {
		return h_item->data.variable;
	}
}

//#ifdef DEBUG_MODE
void print_ins_type(int type)
{
	switch (type) {
	case INS_ASSIGN: printf("INS_ASSIGN"); break;
	case INS_ADD: printf("INS_ADD"); break;
	case INS_SUB: printf("INS_SUB"); break;
	case INS_MUL: printf("INS_MUL"); break;
	case INS_DIV: printf("INS_DIV"); break;
	case INS_EQ: printf("INS_EQ"); break;
	case INS_NEQ: printf("INS_NEQ"); break;
	case INS_GREATER: printf("INS_GREATER"); break;
	case INS_GREATEQ: printf("INS_GREATEQ"); break;
	case INS_LESSER: printf("INS_LESSER"); break;
	case INS_LESSEQ: printf("INS_LESSEQ"); break;
	case INS_JMP: printf("INS_JMP"); break;
	case INS_CJMP: printf("INS_CJMP"); break;
	case INS_LAB: printf("INS_LAB"); break;
	case INS_PUSH: printf("INS_PUSH"); break;
	case INS_CALL: printf("INS_CALL"); break;
	case INS_RET: printf("INS_RET"); break;
	case INS_PUSH_TAB: printf("INS_PUSH_TAB"); break;
	case INS_POP_TAB: printf("INS_POP_TAB"); break;
	case INS_LENGTH: printf("INS_LENGTH"); break;
	case INS_SUBSTR: printf("INS_SUBSTR"); break;
	case INS_CONCAT: printf("INS_CONCAT"); break;
	case INS_FIND: printf("INS_FIND"); break;
	case INS_SORT: printf("INS_SORT"); break;
	case INS_CIN: printf("INS_CIN"); break;
	case INS_COUT: printf("INS_COUT"); break;
	}
}

void print_variable(TVariable* var)
{
	if (var != NULL) {
		printf(" %s", var->name);
	} else {
		printf(" NULL");
	}
}
//#endif

TList_item *create_ins(int type, TVariable *addr1, TVariable *addr2, TVariable *addr3)
{
	TList_item *ins;

	ins = gmalloc(sizeof(TList_item));
	ins->ins_type = type;
	ins->addr1 = addr1;
	ins->addr2 = addr2;
	ins->addr3 = addr3;

	//#ifdef DEBUG_MODE
	print_ins_type(type);
	print_variable(addr1);
	print_variable(addr2);
	print_variable(addr3);
	printf("\n");
	//#endif

	return ins;
}

int t_compare(TVariable *var, int type)
{
	return (var->var_type == type);
}

int operand_type_checker(int operator_type, TVariable *var_1, TVariable *var_2)
{
	/* Operace mezi retezcem a neretezcem */
	if (t_compare(var_1, TYPE_STRING) && !t_compare(var_2, TYPE_STRING)) {
		//#ifdef DEBUG_MODE
		printf("expr: Operace mezi neretezcem a retezcem\n");
		//#endif
		exit_error(E_SEMANTIC_TYPES);
	}

	switch (operator_type) {
		case TOKEN_MUL:
		case TOKEN_DIV:
		case TOKEN_ADD:
		case TOKEN_SUB:
			/* Aritmeticke operace nad retezci */
			if (t_compare(var_1, TYPE_STRING) || t_compare(var_2, TYPE_STRING)) {
				//#ifdef DEBUG_MODE
				printf("expr: Aritmeticke operace nad retezci\n");
				//#endif
				exit_error(E_SEMANTIC_TYPES);
			}
			if (t_compare(var_1, TYPE_DOUBLE) || t_compare(var_2, TYPE_DOUBLE)) {
				return TYPE_DOUBLE;
			}
	}

	return TYPE_INT;
}

int ope_type_2_ins_type(const int *operator_type)
{
	int val;

	switch (*operator_type) {
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
	gene_stack = expr_stack;
	expr_stack = cache_stack;
	if (++operator_counter != operand_counter) {
		//#ifdef DEBUG_MODE
		printf("expr: Nesouhlasi pocet operatoru a operandu\n");
		//#endif
		exit_error(E_SEMANTIC_DEF);
	}
}

TVariable *find_var(TToken *tok)
{
	if (tok->type == TOKEN_IDENTIFIER) {
		for(int i=G.g_frameStack->used-1; i >= 0; i--) {
			TFunction *f = G.g_frameStack->data[i];
			htab_item *found = htab_lookup(f->local_tab, tok->data);
			if(found) {
				return found->data.variable;
			}
		}
		//#ifdef DEBUG_MODE
		printf("expr: EXIT_ERROR\n");
		//#endif
		exit_error(E_SEMANTIC_DEF);
	}

	return token_to_const(tok);
}

//#ifdef DEBUG_MODE
void print_dst_type(const int *t)
{
	switch (*t) {
		case TYPE_INT:
			printf("expr: Dest INTEGER -> ");
			break;
		case TYPE_DOUBLE:
			printf("expr: Dest DOUBLE -> ");
			break;
		case TYPE_STRING:
			printf("expr: Dest STRING -> ");
			break;
	}
}
//#endif

void generate_code(TVariable *ret_var, Tins_list *act_ins_list)
{
	TToken *tok;
	TList_item *actual_ins;
	TVariable *var_1;
	TVariable *var_2;
	TVariable *var_to_push;
	TVariable *new_t_var;
	int t_var_type;
	int t_var_counter;

	t_var_counter = 0;
	postfix_count_test();
	while (!stack_empty(gene_stack)) {
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
			t_var_type = operand_type_checker(tok->type, var_1, var_2);
			//#ifdef DEBUG_MODE
			print_dst_type(&t_var_type);
			//#endif
			new_t_var = next_t_var(t_var_type, &t_var_counter);
			actual_ins = create_ins(ope_type_2_ins_type(&tok->type), new_t_var, var_2, var_1);
			list_insert(act_ins_list, actual_ins);
			stack_push(ins_stack, new_t_var);
		}
	}
	//#ifdef DEBUG_MODE
		printf("expr: ");
	//#endif
	actual_ins = create_ins(INS_ASSIGN, ret_var, stack_top(ins_stack), NULL);
	list_insert(act_ins_list, actual_ins);
}

//#ifdef DEBUG_MODE
void print_token(TToken *tok)
{
	switch (tok->type) {
		case TOKEN_MUL: printf("*"); break;
		case TOKEN_DIV: printf("/"); break;
		case TOKEN_ADD: printf("+"); break;
		case TOKEN_SUB: printf("-"); break;
		case TOKEN_EQUAL: printf("=="); break;
		case TOKEN_NOT_EQUAL: printf("!="); break;
		case TOKEN_GREATER: printf(">"); break;
		case TOKEN_GREATER_EQUAL: printf(">="); break;
		case TOKEN_LESS: printf("<"); break;
		case TOKEN_LESS_EQUAL: printf("<="); break;
		case TOKEN_IDENTIFIER:
		case TOKEN_STRING_VALUE:
		case TOKEN_INT_VALUE:
		case TOKEN_DOUBLE_VALUE: printf("%s", tok->data); break;
		default: printf("%d %s", tok->type, tok->data);
	}
}

void stack_print(TStack *st)
{
	TToken *tok;
	TStack *cache_stack;

	cache_stack = stack_init();
	printf("expr: Postfix  ");
	while (!stack_empty(st)) {
		tok = stack_top(st);
		print_token(tok);
		printf("  ");
		stack_push(cache_stack, tok);
		stack_pop(st);
	}
	while (!stack_empty(cache_stack)) {
		tok = stack_top(cache_stack);
		stack_push(st, tok);
		stack_pop(cache_stack);
	}
	stack_free(cache_stack);
	printf("\n");
}
//#endif

void function_elaboration(int f_symptom)
{
	/*TToken *tok;

	if (f_symptom == internal_function) {
		tok = get_token();
		switch (tok->type) {
			case TOKEN_LENGTH:


		}
	} else {
		printf("Externi funkce\n");
	}*/
}

int its_function()
{
	int answer;
	TToken *tok;
	htab_item *item;

	tok = get_token();
	switch (tok->type) {
		case TOKEN_IDENTIFIER:
			item = htab_lookup(G.g_globalTab, tok->data);
			if (item != NULL) {
				answer = external_function;
			}
			break;
		case TOKEN_LENGTH:
		case TOKEN_SUBSTR:
		case TOKEN_CONCAT:
		case TOKEN_FIND:
		case TOKEN_SORT:
			answer = internal_function;
			break;
		default:
			answer = not_function;
	}
	unget_token(tok);

	return answer;
}

int find_priority(const int *n)
{
	int val;

	switch (*n) {
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

int stack_lower_prio(const TToken *token_in, const TToken *token_stack)
{
	int x;
	int y;
	int val;

	x = find_priority(&token_in->type);
	y = find_priority(&token_stack->type);
	switch (prece_table[y][x]) {
		case HI:
		case EQ:
			val = 0;
			break;
		case LO:
			val = 1;
			break;
		case ER:
			//#ifdef DEBUG_MODE
			printf("expr: EXIT_ERROR\n");
			//#endif
			exit_error(E_SYNTAX);
	}

	return val;
}

void transfer_rest_of_expr_stack()
{
	while (!stack_empty(expr_stack)) {
		stack_push(gene_stack, stack_top(expr_stack));
		stack_pop(expr_stack);
	}
}

void check_expr_integrity(TToken *tok, int *last_type)
{
	switch (*last_type) {
		case TOKEN_EOF:
				break;
		case TOKEN_INT_VALUE:
		case TOKEN_DOUBLE_VALUE:
		case TOKEN_STRING_VALUE:
		case TOKEN_IDENTIFIER:
				if (!token_is_operator(tok) && (tok->type != TOKEN_RROUND_BRACKET)) {
					//#ifdef DEBUG_MODE
					printf("expr: EXIT_ERROR\n");
					//#endif
					exit_error(E_SYNTAX);
				}
				break;
		case TOKEN_LROUND_BRACKET:
				if (!token_is_operand(tok)) {
					//#ifdef DEBUG_MODE
					printf("expr: EXIT_ERROR\n");
					//#endif
					exit_error(E_SYNTAX);
				}
				break;
		case TOKEN_RROUND_BRACKET:
				if (!token_is_operator(tok)) {
					//#ifdef DEBUG_MODE
					printf("expr: EXIT_ERROR\n");
					//#endif
					exit_error(E_SYNTAX);
				}
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
				if (!(token_is_operand(tok))) {
					//#ifdef DEBUG_MODE
					printf("expr: EXIT_ERROR\n");
					//#endif
					exit_error(E_SYNTAX);
				}
				break;
		default:
			//#ifdef DEBUG_MODE
			printf("expr: EXIT_ERROR\n");
			//#endif
			exit_error(E_SYNTAX);
	}
	*last_type = tok->type;
}

void infix_2_postfix()
{
	TToken *tok_in;
	TToken *tok_stack;
	int bracket_counter;
	int last_token_type = TOKEN_EOF;

	bracket_counter = 0;
	tok_in = get_token();
	while ((tok_in->type != TOKEN_SEMICOLON) && ((bracket_counter >= 0))) {
		check_expr_integrity(tok_in, &last_token_type);
		switch (tok_in->type) {
			case TOKEN_INT_VALUE:
			case TOKEN_DOUBLE_VALUE:
			case TOKEN_STRING_VALUE:
			case TOKEN_IDENTIFIER:
				stack_push(gene_stack, tok_in);
				break;
			case TOKEN_LROUND_BRACKET:
				bracket_counter++;
				stack_push(expr_stack, tok_in);
				break;
			case TOKEN_RROUND_BRACKET:
				bracket_counter--;
				if (bracket_counter < 0) {
					unget_token(tok_in);
					break;
				}
				tok_stack = stack_top(expr_stack);
				stack_pop(expr_stack);
				while (tok_stack->type != TOKEN_LROUND_BRACKET) {
					stack_push(gene_stack, tok_stack);
					tok_stack = stack_top(expr_stack);
					stack_pop(expr_stack);
				}
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
			default:
				//#ifdef DEBUG_MODE
				printf("expr: Wrong token!\n");
				//#endif
				exit_error(E_SYNTAX);
		}
		tok_in = get_token();
	}
	transfer_rest_of_expr_stack();
	//#ifdef DEBUG_MODE
	printf("expr: Unget token ");
	print_token(tok_in);
	printf("\n");
	stack_print(gene_stack);
	//#endif
	unget_token(tok_in);
}

void expr_init()
{
	expr_stack = stack_init();
	gene_stack = stack_init();
	ins_stack = stack_init();
}

void expression(TVariable *ret_var, Tins_list *act_ins_list, bool f_is_possible)
{
	int function_symptom;

	/*TToken *tok;

	for (int i = 0; i < 5000; i++) {
			tok = get_token();
			print_token(tok);
			printf("\n");
	}*/


	//#ifdef DEBUG_MODE
	printf("expr: --START--\n");
	TToken *tok = get_token();
	printf("expr: First token ");
	print_token(tok);
	printf("\n");
	unget_token(tok);
	//#endif

	expr_init();
	function_symptom = its_function();
	if (function_symptom == not_function) {
		//#ifdef DEBUG_MODE
		printf("expr: expr comming!\n");
		//#endif
		infix_2_postfix();
		generate_code(ret_var, act_ins_list);
		//#ifdef DEBUG_MODE
		printf("expr: expr done!\n");
		//#endif
	} else {
		//#ifdef DEBUG_MODE
		printf("expr: function comming!\n");
		//#endif
		if (f_is_possible) {
			function_elaboration(function_symptom);
		} else {
			//#ifdef DEBUG_MODE
			printf("expr: EXIT_ERROR\n");
			//#endif
			exit_error(E_SEMANTIC_DEF);
		}
		//#ifdef DEBUG_MODE
		printf("expr: function done!\n");
		//#endif
	}
	charlady();
	//#ifdef DEBUG_MODE
	printf("expr: --END--\n");
	//#endif
}

