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
#include "expr.h"
#include "lex.h"
#include "enums.h"
#include "error.h"
#include "ilist.h"
#include "adt.h"
#include "ial.h"
#include "interpret.h"
#include "shared.h"
#include "time.h"

#define RULE_COUNTER 13

const char prece_table[RULE_COUNTER][RULE_COUNTER] = {
/* st\in   +   -   *   /   (   )   id  <   >   <=  >=  ==  != */
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
/* == */ { LO, LO, LO, LO, LO, HI, LO, LO, LO, LO, LO, HI, HI },
/* != */ { LO, LO, LO, LO, LO, HI, LO, LO, LO, LO, LO, HI, HI }
};
const char operation_table[4][4] = {
/* dst\src  i   d   s   a  */
/*  i   */ {EQ, EQ, ER, ER},
/*  d   */ {EQ, EQ, ER, ER},
/*  s   */ {ER, ER, EQ, ER},
/*  a   */ {EQ, EQ, ER, EQ}
};
TStack *expr_stack;
TStack *gene_stack;
TStack *ins_stack;

/*
TODO
	Test na low_prio ERR
ZEPTAT SE
POZNAMKY
	Nesahejte mi na kod! :D
OMEZENÍ
	Maximálně 1 000 000 000 proměnných výrazu
INTERNÍ INFORMACE
	Aktuální volné error_pos_in_code 19 29+
*/

void my_exit_error(const int error_type, const int error_pos_in_code)
{
	#ifdef DEBUG_MODE
	printf("expr: EXIT_ERROR! %d (error position %d)\n", error_type, error_pos_in_code);
	#endif
	(void)error_pos_in_code;
	exit_error(error_type);
}

int token_is(const TToken *tok, const int token_type)
{
	return tok->type == token_type;
}

void charlady()
{
	stack_clear(expr_stack);
	stack_clear(gene_stack);
	stack_clear(ins_stack);
}

char *create_t_name(const int *number)
{
	char *s;

	s = malloc(sizeof(char)*11);
	if (s == NULL) {
		my_exit_error(E_ALLOC, 25);
	}
	sprintf(s, "T%d", *number);

	return s;
}

TVariable* next_t_var(const int *t_x_type, int *t_x_var_counter)
{
	htab_item *h_item;
	TVariable* var;
	char *t_name;

	t_name = create_t_name(t_x_var_counter);
	h_item = htab_lookup(G.g_exprTab, t_name);
	if(h_item == NULL) {
		var = malloc(sizeof(TVariable));
		if (var == NULL) {
			my_exit_error(E_ALLOC, 26);
		}
		var->initialized = 1;
		var->constant = 1;
		var->name = t_name;
		var->var_type = *t_x_type;
		h_item = htab_insert(G.g_exprTab, t_name);
		h_item->data.variable = var;
		h_item->data_type = TYPE_VARIABLE;
		(*t_x_var_counter)++;

		return var;
	} else {
		return h_item->data.variable;
	}
}

#ifdef DEBUG_MODE
void print_ins_type(const int *type)
{
	printf("expr: ");
	switch (*type) {
	case INS_ASSIGN:
		printf("INS_ASSIGN");
		break;
	case INS_ADD:
		printf("INS_ADD");
		break;
	case INS_SUB:
		printf("INS_SUB");
		break;
	case INS_MUL:
		printf("INS_MUL");
		break;
	case INS_DIV:
		printf("INS_DIV");
		break;
	case INS_EQ:
		printf("INS_EQ");
		break;
	case INS_NEQ:
		printf("INS_NEQ");
		break;
	case INS_GREATER:
		printf("INS_GREATER");
		break;
	case INS_GREATEQ:
		printf("INS_GREATEQ");
		break;
	case INS_LESSER:
		printf("INS_LESSER");
		break;
	case INS_LESSEQ:
		printf("INS_LESSEQ");
		break;
	case INS_JMP:
		printf("INS_JMP");
		break;
	case INS_CJMP:
		printf("INS_CJMP");
		break;
	case INS_LAB:
		printf("INS_LAB");
		break;
	case INS_PUSH_PARAM:
		printf("INS_PUSH_PARAM");
		break;
	case INS_CALL:
		printf("INS_CALL");
		break;
	case INS_RET:
		printf("INS_RET");
		break;
	case INS_PUSH_TAB:
		printf("INS_PUSH_TAB");
		break;
	case INS_POP_TAB:
		printf("INS_POP_TAB");
		break;
	case INS_LENGTH:
		printf("INS_LENGTH");
		break;
	case INS_SUBSTR:
		printf("INS_SUBSTR");
		break;
	case INS_CONCAT:
		printf("INS_CONCAT");
		break;
	case INS_FIND:
		printf("INS_FIND");
		break;
	case INS_SORT:
		printf("INS_SORT");
		break;
	case INS_CIN:
		printf("INS_CIN");
		break;
	case INS_COUT:
		printf("INS_COUT");
		break;
	}
}

void print_variable(const TVariable* var)
{
	if (var != NULL) {
		printf(" %s", var->name);
	} else {
		printf(" NULL");
	}
}

void print_addr_type(const int *t, const int controler)
{
	switch (controler) {
		case 1:
			printf(" (Dst: ");
			break;
		case 2:
			printf(" Src: ");
			break;
				
	}	
	switch (*t) {
		case TYPE_INT:
			printf("INTEGER");
			break;
		case TYPE_DOUBLE:
			printf("DOUBLE");
			break;
		case TYPE_STRING:
			printf("STRING");
			break;
	}
	if (controler == 2) {
		printf(")");
	}
}
#endif

TList_item *create_ins(const int ins_type, TVariable *addr1, TVariable *addr2, TVariable *addr3)
{
	TList_item *ins;

	if (ins_type == INS_ASSIGN) {
		if (operation_table[addr1->var_type][addr2->var_type] == ER) {
			if (strcmp(addr1->name, "return")) {
				my_exit_error(E_SEMANTIC_TYPES, 23);
			} else {
				my_exit_error(E_SEMANTIC_OTHERS, 11);
			}
		}
		if (addr1->var_type == TYPE_AUTO) {
			addr1->var_type = addr2->var_type;
		}
	}
	ins = malloc(sizeof(TList_item));
	if (ins == NULL) {
		my_exit_error(E_ALLOC, 27);
	}
	ins->ins_type = ins_type;
	ins->addr1 = addr1;
	ins->addr2 = addr2;
	ins->addr3 = addr3;

	#ifdef DEBUG_MODE
	print_ins_type(&ins_type);
	if (ins_type != INS_CALL) {
		print_variable(addr1);
	} else {
		htab_item *item;
		item = (htab_item*)addr1;
		printf(" %s", item->data.function->name);
	}
	print_variable(addr2);
	print_variable(addr3);
	print_addr_type(&addr1->var_type, 1);
	if (addr2 != NULL) {
		print_addr_type(&addr2->var_type, 2);
	}
	printf("\n");
	#endif

	return ins;
}

int t_compare(const TVariable *var, const int type)
{
	return (var->var_type == type);
}

int type_after_operation(const int *operator_type, const TVariable *var_1, const TVariable *var_2)
{
	/* Auto type in expression */
	if ((var_1->var_type == TYPE_AUTO) || (var_2->var_type == TYPE_AUTO)) {
		my_exit_error(E_AUTO_TYPE, 21);
	}
	/* String XOR String */
	if (t_compare(var_1, TYPE_STRING) ^ t_compare(var_2, TYPE_STRING)) {
		my_exit_error(E_SEMANTIC_TYPES, 1);
	}
	switch (*operator_type) {
		case TOKEN_MUL:
		case TOKEN_DIV:
		case TOKEN_ADD:
		case TOKEN_SUB:
			/* String +/-* String */
			if (t_compare(var_1, TYPE_STRING) || t_compare(var_2, TYPE_STRING)) {
				my_exit_error(E_SEMANTIC_TYPES, 2);
			}
			if (t_compare(var_1, TYPE_DOUBLE) || t_compare(var_2, TYPE_DOUBLE)) {
				return TYPE_DOUBLE;
			}
	}
	return TYPE_INT;
}

int ope_2_ins_type(const TToken *tok)
{
	switch (tok->type) {
		case TOKEN_MUL:
			return INS_MUL;
		case TOKEN_DIV:
			return INS_DIV;
		case TOKEN_ADD:
			return INS_ADD;
		case TOKEN_SUB:
			return INS_SUB;
		case TOKEN_EQUAL:
			return INS_EQ;
		case TOKEN_NOT_EQUAL:
			return INS_NEQ;
		case TOKEN_GREATER:
			return INS_GREATER;
		case TOKEN_GREATER_EQUAL:
			return INS_GREATEQ;
		case TOKEN_LESS:
			return INS_LESSER;
		case TOKEN_LESS_EQUAL:
			return INS_LESSEQ;
		case TOKEN_LENGTH:
			return INS_LENGTH;
		case TOKEN_SUBSTR:
			return INS_SUBSTR;
		case TOKEN_CONCAT:
			return INS_CONCAT;
		case TOKEN_FIND:
			return INS_FIND;
		case TOKEN_SORT:
			return INS_SORT;
	}
	return 0;
}

int token_is_operand(const TToken *tok)
{
	switch (tok->type) {
		case TOKEN_IDENTIFIER:
		case TOKEN_STRING_VALUE:
		case TOKEN_INT_VALUE:
		case TOKEN_DOUBLE_VALUE:
			return 1;
	}
	return 0;
}

int token_is_operator(const TToken *tok)
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
	}
	return 0;
}

void postfix_count_test()
{
	int counter;
	TToken *tok;
	TStack *cache_stack;

	counter = -1;
	while (!stack_empty(gene_stack)) {
		tok = stack_top(gene_stack);
		if (token_is_operand(tok)) {
			counter++;
		} else {
			counter--;
		}
		stack_pop(gene_stack);
		stack_push(expr_stack, tok);
	}
	cache_stack = gene_stack;
	gene_stack = expr_stack;
	expr_stack = cache_stack;
	if (counter) {
		my_exit_error(E_SYNTAX, 3);
	}
}

TVariable *find_var(/*const */TToken *tok)
{
	switch (tok->type) {
		case TOKEN_IDENTIFIER:
			for(int i=G.g_frameStack->used-1; i >= 0; i--) {
				TFunction *f = G.g_frameStack->data[i];
				htab_item *found = htab_lookup(f->local_tab, tok->data);
				if(found) {
					return found->data.variable;
				}
			}
			my_exit_error(E_SEMANTIC_DEF, 4);
			break;
		case TOKEN_STRING_VALUE:
		case TOKEN_INT_VALUE:
		case TOKEN_DOUBLE_VALUE:
			return token_to_const(tok);
	}
	/* Volani funkce pamametricke bez parametru */
	if (tok->type == TOKEN_RROUND_BRACKET) {
		my_exit_error(E_SEMANTIC_TYPES, 6);
	}
	my_exit_error(E_SEMANTIC_DEF, 22);
	return 0;
}

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
			t_var_type = type_after_operation(&tok->type, var_1, var_2);
			new_t_var = next_t_var(&t_var_type, &t_var_counter);
			actual_ins = create_ins(ope_2_ins_type(tok), new_t_var, var_2, var_1);			
			list_insert(act_ins_list, actual_ins);
			stack_push(ins_stack, new_t_var);
		}
	}
	actual_ins = create_ins(INS_ASSIGN, ret_var, stack_top(ins_stack), NULL);
	list_insert(act_ins_list, actual_ins);
}

#ifdef DEBUG_MODE
void print_token(const TToken *tok)
{
	switch (tok->type) {
		case TOKEN_MUL:
			printf("*");
			break;
		case TOKEN_DIV:
			printf("/");
			break;
		case TOKEN_ADD:
			printf("+");
			break;
		case TOKEN_SUB:
			printf("-");
			break;
		case TOKEN_EQUAL:
			printf("==");
			break;
		case TOKEN_NOT_EQUAL:
			printf("!=");
			break;
		case TOKEN_GREATER:
			printf(">");
			break;
		case TOKEN_GREATER_EQUAL:
			printf(">=");
			break;
		case TOKEN_LESS:
			printf("<");
			break;
		case TOKEN_LESS_EQUAL:
			printf("<=");
			break;
		case TOKEN_IDENTIFIER:
		case TOKEN_STRING_VALUE:
		case TOKEN_INT_VALUE:
		case TOKEN_DOUBLE_VALUE:
			printf("%s", tok->data);
			break;
		default:
			printf("%d %s", tok->type, tok->data);
	}
}

void stack_print(/*const */TStack *st)
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
#endif

int token_type_2_var_type(const int *n)
{
	switch (*n) {
		case TOKEN_INT:
				return TYPE_INT;
		case TOKEN_DOUBLE:
				return TYPE_DOUBLE;
		case TOKEN_STRING:
				return TYPE_STRING;
	}
	return 0;
}

TVariable *get_next_para(const int operand_type)
{
	TVariable *new_var;
	TToken *tok;

	tok = get_token();
	if (!token_is_operand(tok)) {
		if (token_is(tok, TOKEN_RROUND_BRACKET)) {
			my_exit_error(E_SEMANTIC_TYPES, 12);
		} else {
			my_exit_error(E_SYNTAX, 8);
		}
	}
	new_var = find_var(tok);
	if (operation_table[operand_type][new_var->var_type] == ER) {
			my_exit_error(E_SEMANTIC_TYPES, 5);
	}
	return new_var;
}

void skip_token(int token_type)
{
	TToken *tok;

	tok = get_token();
	if ((token_type != TOKEN_RROUND_BRACKET) && token_is(tok, TOKEN_RROUND_BRACKET)) {
		my_exit_error(E_SEMANTIC_TYPES, 20);
	}
	if (token_is(tok, TOKEN_COMMA) && (token_type != TOKEN_COMMA)) {
		my_exit_error(E_SEMANTIC_TYPES, 24);
	}
	if ((token_type == TOKEN_RROUND_BRACKET) && token_is_operand(tok)) {
		my_exit_error(E_SEMANTIC_TYPES, 28);
	}
	if (tok->type != token_type) {
		my_exit_error(E_SYNTAX, 7);
	}
}

void generate_external_function(TVariable *ret_var, Tins_list *act_ins_list)
{
	TToken *tok;
	TList_item *actual_ins;
	htab_item *h_item;
	TFunction *f_stored;
	int f_sto_param_count;
	TVariable *f_stored_var;
	TVariable *f_readed_var;

	tok = get_token();
	h_item = htab_lookup(G.g_globalTab, tok->data);
	f_stored = h_item->data.function;
	skip_token(TOKEN_LROUND_BRACKET);
	f_sto_param_count = f_stored->params_stack->used;
	for(int i = 0; i < f_sto_param_count; i++) {
		f_stored_var = f_stored->params_stack->data[i];
		f_readed_var = get_next_para(f_stored_var->var_type);
		if (i < f_sto_param_count-1) {
			skip_token(TOKEN_COMMA);
		}
		actual_ins = create_ins(INS_PUSH_PARAM, f_readed_var, NULL, NULL);
		list_insert(act_ins_list, actual_ins);
	}
	actual_ins = create_ins(INS_CALL, (TVariable*)h_item, NULL, NULL);
	list_insert(act_ins_list, actual_ins);
	actual_ins = create_ins(INS_ASSIGN, ret_var, f_stored->return_var, NULL);
	list_insert(act_ins_list, actual_ins);
	skip_token(TOKEN_RROUND_BRACKET);
}

void generate_internal_function(TVariable *ret_var, Tins_list *act_ins_list)
{
	TToken *tok;
	TList_item *actual_ins;
	int ins_type;
	TVariable *var_1;
	TVariable *var_2;
	TVariable *var_3;

	tok = get_token();
	ins_type = ope_2_ins_type(tok);
	skip_token(TOKEN_LROUND_BRACKET);
	switch (ins_type) {
		case INS_LENGTH:
		case INS_SORT:
			var_1 = get_next_para(TYPE_STRING);
			actual_ins = create_ins(ins_type, ret_var, var_1, NULL);
			list_insert(act_ins_list, actual_ins);
			break;
		case INS_CONCAT:
		case INS_FIND:
			var_1 = get_next_para(TYPE_STRING);
			skip_token(TOKEN_COMMA);
			var_2 = get_next_para(TYPE_STRING);
			actual_ins = create_ins(ins_type, ret_var, var_1, var_2);
			list_insert(act_ins_list, actual_ins);
			break;
		case INS_SUBSTR:
			var_1 = get_next_para(TYPE_STRING);
			skip_token(TOKEN_COMMA);
			var_2 = get_next_para(TYPE_INT);
			skip_token(TOKEN_COMMA);
			var_3 = get_next_para(TYPE_INT);
			actual_ins = create_ins(INS_PUSH_PARAM, var_1, NULL, NULL);
			list_insert(act_ins_list, actual_ins);
			actual_ins = create_ins(INS_PUSH_PARAM, var_2, NULL, NULL);
			list_insert(act_ins_list, actual_ins);
			actual_ins = create_ins(INS_PUSH_PARAM, var_3, NULL, NULL);
			list_insert(act_ins_list, actual_ins);
			actual_ins = create_ins(ins_type, ret_var, NULL, NULL); /* SUBSTR obsahuje parametry ASSIGN */
			list_insert(act_ins_list, actual_ins);
	}
	skip_token(TOKEN_RROUND_BRACKET);
}

int its_function()
{
	int answer;
	TToken *tok;
	htab_item *item;

	answer = not_function;
	tok = get_token();
	switch (tok->type) {
		case TOKEN_IDENTIFIER:
			item = htab_lookup(G.g_globalTab, tok->data);
			if (item == NULL) {
				if (!find_var(tok)) {
					my_exit_error(E_SEMANTIC_DEF, 9);
				}
			} else {
				answer = external_function;
			}
			break;
		case TOKEN_LENGTH:
		case TOKEN_SUBSTR:
		case TOKEN_CONCAT:
		case TOKEN_FIND:
		case TOKEN_SORT:
			answer = internal_function;
	}
	unget_token(tok);

	return answer;
}

int token_to_op_type(const TToken *tok)
{
	switch (tok->type) {
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
	return 0;
}

int stack_lower_prio(const TToken *token_in, const TToken *token_stack)
{
	int x;
	int y;

	x = token_to_op_type(token_in);
	y = token_to_op_type(token_stack);
	switch (prece_table[y][x]) {
		case HI:
		case EQ:
			return 0;
		case LO:
			return 1;
		case ER:
			my_exit_error(E_SYNTAX, 13);
	}
	return 0;
}

void transfer_rest_of_expr_stack()
{
	while (!stack_empty(expr_stack)) {
		stack_push(gene_stack, stack_top(expr_stack));
		stack_pop(expr_stack);
	}
}

void check_expr_integrity(const TToken *tok, int *last_type)
{
	switch (*last_type) {
		case TOKEN_EOF:
				if (!token_is_operand(tok) && !token_is(tok, TOKEN_LROUND_BRACKET)) {
					my_exit_error(E_SYNTAX, 14);
				}
				break;
		case TOKEN_INT_VALUE:
		case TOKEN_DOUBLE_VALUE:
		case TOKEN_STRING_VALUE:
		case TOKEN_IDENTIFIER:
				if (!token_is_operator(tok) && !token_is(tok, TOKEN_RROUND_BRACKET)) {
					my_exit_error(E_SYNTAX, 15);
				}
				break;
		case TOKEN_LROUND_BRACKET:
				if (!token_is_operand(tok) && !token_is(tok, TOKEN_LROUND_BRACKET)) {
					my_exit_error(E_SYNTAX, 16);
				}
				break;
		case TOKEN_RROUND_BRACKET:
				if (!token_is_operator(tok) && !token_is(tok, TOKEN_RROUND_BRACKET)) {
					my_exit_error(E_SYNTAX, 17);
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
				if (!(token_is_operand(tok)) && !token_is(tok, TOKEN_LROUND_BRACKET)) {
					my_exit_error(E_SYNTAX, 18);
				}
	}
	*last_type = tok->type;
}

TToken *save_tok(TToken *tok)
{
	TToken *tok_push;
	
	tok_push = malloc(sizeof(TToken));
	tok_push->type = tok->type;
	tok_push->data = tok->data;
	
	return tok_push;
}

void infix_2_postfix()
{
	TToken *tok_in;
	TToken *tok_stack;
	
	int bracket_counter;
	int last_token_type = TOKEN_EOF;	

	bracket_counter = 1;
	tok_in = get_token();
	while ((tok_in->type != TOKEN_SEMICOLON) && (bracket_counter)) {
		check_expr_integrity(tok_in, &last_token_type);
		switch (tok_in->type) {
			case TOKEN_INT_VALUE:
			case TOKEN_DOUBLE_VALUE:
			case TOKEN_STRING_VALUE:
			case TOKEN_IDENTIFIER:				
				stack_push(gene_stack, save_tok(tok_in));
				break;
			case TOKEN_LROUND_BRACKET:
				bracket_counter++;				
				stack_push(expr_stack, save_tok(tok_in));
				break;
			case TOKEN_RROUND_BRACKET:
				bracket_counter--;
				if (!bracket_counter) {
					unget_token(tok_in);
					break;
				}
				tok_stack = stack_top(expr_stack);
				stack_pop(expr_stack);
				while (tok_stack->type != TOKEN_LROUND_BRACKET) {
					
					stack_push(gene_stack, save_tok(tok_stack));
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
					stack_push(expr_stack, save_tok(tok_in));
				} else {
					while (!stack_empty(expr_stack) && !stack_lower_prio(tok_in, tok_stack)) {
						stack_push(gene_stack, tok_stack);
						stack_pop(expr_stack);
						tok_stack = stack_top(expr_stack);
					}
					stack_push(expr_stack, save_tok(tok_in));
				}
				break;
		}
		tok_in = get_token();
	}
	transfer_rest_of_expr_stack();
	#ifdef DEBUG_MODE
	printf("expr: Unget token ");
	print_token(tok_in);
	printf("\n");
	stack_print(gene_stack);
	#endif
	unget_token(tok_in);
}

void expr_init()
{
	expr_stack = stack_init();
	gene_stack = stack_init();
	ins_stack = stack_init();
}

void expression(TVariable *ret_var, Tins_list *act_ins_list)
{
	#ifdef DEBUG_MODE
	printf("expr: --START--\n");
	TToken *tok = get_token();
	printf("expr: First token ");
	print_token(tok);
	printf("\n");
	unget_token(tok);
	#endif
	
	switch (its_function()) {
		case not_function:
			infix_2_postfix();
			generate_code(ret_var, act_ins_list);
			break;
		case external_function:
			generate_external_function(ret_var, act_ins_list);
			break;
		case internal_function:
			generate_internal_function(ret_var, act_ins_list);
	}
	charlady();

	#ifdef DEBUG_MODE
	printf("expr: --END--\n");
	#endif
}

