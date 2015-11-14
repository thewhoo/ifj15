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
#include <stdbool.h>
#include <string.h>
#include "parser.h"
#include "adt.h"
#include "lex.h"
#include "galloc.h"
#include "enums.h"
#include "error.h"
#include "stack.h"
#include "ial.h"
#include "ilist.h"
#include "shared.h"
#include "interpret.h"

#define LOCALTAB_DEFAULT_SIZE 20

TToken* token;

// Forward declarations of "rule" functions
bool PROG();
bool FUNCTION_DECL();
bool DATA_TYPE();
bool FUNC_DECL_PARAMS();
bool FUNC_DECL_PARAMS_NEXT();
bool NESTED_BLOCK();
bool NBC();
bool DECL_OR_ASSIGN();
bool DECL_ASSIGN();
bool FCALL_OR_ASSIGN();
bool FOA_PART2();
bool HARD_VALUE();
bool FUNCTION_CALL_PARAMS();
bool FUNCTION_CALL_PARAM();
bool FUNCTION_CALL_PARAMS_NEXT();
bool BUILTIN_CALL();
bool BUILTIN_FUNC();
bool IF_STATEMENT();
bool ELSE_STATEMENT();
bool COUT();
bool COUT_NEXT();
bool COUT_OUTPUT();
bool CIN();
bool CIN_NEXT();
bool FOR_STATEMENT();
bool FOR_DECLARATION();
bool FOR_EXPR();
bool FOR_ASSIGN();
bool RETURN();
void CALL_EXPR();
TFunction *getNewFunction();
TVariable *getNewVariable();
void pushParam();
void storeFuncName();
void storeVarName();
void storeFunction();
void storeVariable();

enum
{
	T_FUNC,
	T_VAR
};

void storeFunction(TFunction *f)
{
  // Check for redeclaration of function
  if(htab_lookup(g_globalTab, f->name))
     exit_error(E_SEMANTIC_DEF);

  htab_item *newFunc = htab_insert(g_globalTab, f->name);
  newFunc->data.function = f;

}

void storeNewVariable(TFunction *f, TVariable *v)
{
  // Check for redeclaration
  if(htab_lookup(f->local_tab, v->name))
    exit_error(E_SEMANTIC_DEF);

  htab_item *newVar = htab_insert(f->local_tab, v->name);
  newVar->data.variable = v;

}

TFunction *getNewFunction()
{
  TFunction *f = gmalloc(sizeof(TFunction));

	f->name = NULL;
	f->return_type = 0;
	f->defined = 0;
	f->ins_list = list_init();
	f->local_tab = htab_init(LOCALTAB_DEFAULT_SIZE);
	f->params_stack = stack_init();

  return f;
}

TVariable *getNewVariable()
{
  TVariable *v = gmalloc(sizeof(TVariable));

  v->var_type = 0;
  v->name = NULL;
  v->initialized = 0;

  return v;
}

void pushParam(TFunction *f, TVariable *p)
{
  stack_push(f->params_stack, p);
}

void storeVarValue(TVariable *v)
{

}

void storeFuncName(TFunction *f)
{
  f->name = gmalloc(strlen(token->data) + 1);
  strcpy(f->name, token->data);
}

void storeVarName(TVariable *v)
{
  v->name = gmalloc(strlen(token->data) + 1);
  strcpy(v->name, token->data);
}

void CALL_EXPR()
{
}

// Definitions of "rule" functions
bool PROG()
{

  // We need a function declaration or nothing
	if (token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
		return FUNCTION_DECL() && PROG();

	else if (token->type == TOKEN_EOF)
		return true;

  // Syntax error
	return false;

}

bool FUNCTION_DECL()
{

  TFunction *currentFunc = getNewFunction();

  // Tell everyone we are currently in a new function
  stack_push(g_frameStack, currentFunc);

	// No valid data type detected --> Syntax error
	if(!DATA_TYPE(currentFunc, T_FUNC))
		return false;

  // Store the name
	if (token->type == TOKEN_IDENTIFIER)
	{
		storeFuncName(currentFunc);
		token = get_token();
	}
	// Syntax error
	else
		return false;

	if (token->type == TOKEN_LROUND_BRACKET)
		token = get_token();
	// Syntax error
	else
		return false;

  // If all params were processed, process the function block
	if(FUNC_DECL_PARAMS(currentFunc))
	{
		if (token->type == TOKEN_RROUND_BRACKET)
		{
			token = get_token();

      // Process function block
			if(!NESTED_BLOCK(func))
        return false;

      // Store the complete function "object" in the global table
      storeFunction(currentFunc);
		}
	}


	return true;
}

// Detect and store the data type of the new function/variable
bool DATA_TYPE(void *object, int type)
{
  TFunction *f = NULL;
  TVariable *v = NULL;

  if(type == T_VAR)
    v = object;
  else
    f = object;

	// Add the data type to the currently processed function/variable
	switch(token->type)
	{
		case TOKEN_INT:
			if (type == T_VAR)
				v->var_type = TYPE_INT;
			else
				f->return_type = TYPE_INT;
			break;

		case TOKEN_DOUBLE:
			if (type == T_VAR)
				v->var_type = TYPE_DOUBLE;
			else
				f->return_type = TYPE_DOUBLE;
			break;

		case TOKEN_STRING:
			if (type == T_VAR)
				v->var_type = TYPE_STRING;
			else
				f->return_type = TYPE_STRING;
			break;

		// Syntax error
		default:
			return false;
	}

	token = get_token();

	return true;
}

// NOTE: This function could be called with indirect recursion from FUNC_DECL_PARAMS_NEXT
bool FUNC_DECL_PARAMS(TFunction *func)
{
  // If an identifier arrives, we must create and store a new param variable
	if(token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
    TVariable *param = getNewVariable();

    // Record the data type of the new param, then proceed
		if(DATA_TYPE(param, T_VAR) && token->type == TOKEN_IDENTIFIER)
		{
      // Record the name of the param
      storeVarName(param);
			token = get_token();

      // Process next param
			if(!FUNC_DECL_PARAMS_NEXT(func))
        return false;

      // Push the param on stack
      // FUNC_DECL_PARAMS is called recursively, so the last param will be pushed first
      pushParam(func, param);
		}
    else
      return false;

  }

  // No more params, all good
  else if(token->type == TOKEN_RROUND_BRACKET)
    return true;

  return false;

}

bool FUNC_DECL_PARAMS_NEXT(TFunction *func)
{
  // Next param must start with comma if it exists
	if (token->type == TOKEN_COMMA)
	{
		token = get_token();
		return FUNC_DECL_PARAMS(func);
	}

	else if (token->type == TOKEN_RROUND_BRACKET)
		return true;

  else
    return false;
}

bool NESTED_BLOCK(TFunction *func)
{
	bool ret = false;

	if (token->type == TOKEN_LCURLY_BRACKET)
	{
		token = get_token();
		if(NBC(func))
		{
			ret = (token->type == TOKEN_RCURLY_BRACKET);
			token = get_token();
		}
	}

	return ret;
}

// TODO: FIX
bool NBC(TFunction *func)
{

	switch(token->type)
	{
		case TOKEN_AUTO:
		case TOKEN_INT:
		case TOKEN_DOUBLE:
		case TOKEN_STRING:
			return DECL_OR_ASSIGN(func) && NBC(func);

		case TOKEN_IDENTIFIER:
			ret = FCALL_OR_ASSIGN() && NBC();
			break;

		case TOKEN_LENGTH:
		case TOKEN_SUBSTR:
		case TOKEN_CONCAT:
		case TOKEN_FIND:
		case TOKEN_SORT:
			ret = BUILTIN_CALL() && NBC();
			break;

		case TOKEN_IF:
			ret = IF_STATEMENT() && NBC();
			break;

		case TOKEN_COUT:
			ret = COUT() && NBC();
			break;

		case TOKEN_CIN:
			ret = CIN() && NBC();
			break;

		case TOKEN_FOR:
			ret = FOR_STATEMENT() && NBC();
			break;

		case TOKEN_LCURLY_BRACKET:
			ret = NESTED_BLOCK() && NBC();
			break;

		case TOKEN_RETURN:
			ret = RETURN();
			break;

		case TOKEN_RCURLY_BRACKET:
			ret = true;
			break;
	}

	return ret;
}

bool DECL_OR_ASSIGN(TFunction *func)
{
	bool ret = false;
	bool midway = true;

  // Received identifier, expecting declaration or declaration with assignment
	if (token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
    // Create new variable "object"
    TVairable *var = getNewVariable();

    // Store data type in variable object
		if(DATA_TYPE(var, T_VAR) && token->type == TOKEN_IDENTIFIER)
		{
      // Store variable name
      storeVarName(var);
			token = get_token();

      // Process assignment
			if(!(DECL_ASSIGN(var) && token->type == TOKEN_SEMICOLON))
        // Syntax error
        return false;

      // Store variable in function symbol table
      storeNewVariable(func, var);

      token = get_token();
		}
    // Syntax error
    else
      return false;
	}
	else if (token->type == TOKEN_AUTO)
	{
		token = get_token();
		if (token->type == TOKEN_IDENTIFIER)
			token = get_token();
		else
			midway = false;

		if (token->type == TOKEN_ASSIGN)
		{
			token = get_token();
			CALL_EXPR();
		}
		else
			midway = false;

		ret = (token->type == TOKEN_SEMICOLON);
		token = get_token();

	}

	return ret && midway;
}

bool DECL_ASSIGN(TVariable *v)
{
  // We must initialize the variable
	if(token->type == TOKEN_ASSIGN)
	{
		token = get_token();
		CALL_EXPR();
		ret = true;
	}

  // Variable was only declared, not initialized
	else if(token->type == TOKEN_SEMICOLON)
	{
		v->initialized = false;
    return true;
	}

	return false;
}

bool FCALL_OR_ASSIGN()
{
	bool ret = false;

	if(token->type == TOKEN_IDENTIFIER)
	{
		token = get_token();
		ret = FOA_PART2();
	}

	return ret;
}

bool FOA_PART2()
{
	bool ret = false;

	if(token->type == TOKEN_LROUND_BRACKET)
	{
		token = get_token();
		if(FUNCTION_CALL_PARAMS() && token->type == TOKEN_RROUND_BRACKET)
		{
			token = get_token();
			ret = (token->type == TOKEN_SEMICOLON);
			token = get_token();
		}
	}
	else if (token->type == TOKEN_ASSIGN)
	{
		token = get_token();
		CALL_EXPR();
		ret = (token->type == TOKEN_SEMICOLON);
		token = get_token();
	}

	return ret;
}

bool HARD_VALUE()
{
	bool ret = (token->type == TOKEN_INT_VALUE || token->type == TOKEN_DOUBLE_VALUE || token->type == TOKEN_STRING_VALUE);
	token = get_token();
	return ret;
}

bool FUNCTION_CALL_PARAMS()
{
	bool ret = false;

	if(token->type == TOKEN_IDENTIFIER || token->type == TOKEN_INT_VALUE || token->type == TOKEN_DOUBLE_VALUE || token->type == TOKEN_STRING_VALUE)
	{
		ret = FUNCTION_CALL_PARAM() && FUNCTION_CALL_PARAMS_NEXT();
	}
	else if (token->type == TOKEN_RROUND_BRACKET)
	{
		ret = true;
	}

	return ret;
}

bool FUNCTION_CALL_PARAM()
{
	bool ret = false;

	if(token->type == TOKEN_IDENTIFIER)
	{
		token = get_token();
		ret = true;
	}
	else
	{
		ret = HARD_VALUE();
	}

	return ret;
}

bool FUNCTION_CALL_PARAMS_NEXT()
{
	bool ret = false;

	if(token->type == TOKEN_COMMA)
	{
		token = get_token();
		ret = FUNCTION_CALL_PARAMS();
	}
	else if (token->type == TOKEN_RROUND_BRACKET)
	{
		ret = true;
	}

	return ret;
}

bool BUILTIN_CALL()
{
	bool ret = false;

	if(BUILTIN_FUNC() && token->type == TOKEN_LROUND_BRACKET)
	{
		token = get_token();
		if(FUNCTION_CALL_PARAMS() && token->type == TOKEN_RROUND_BRACKET)
		{
			token = get_token();
			ret = (token->type == TOKEN_SEMICOLON);
			token = get_token();
		}
	}

	return ret;
}

bool BUILTIN_FUNC()
{
	bool ret = false;
	ret = (token->type == TOKEN_LENGTH || token->type == TOKEN_SUBSTR || token->type == TOKEN_CONCAT || token->type == TOKEN_FIND || token->type == TOKEN_SORT);
	token = get_token();
	return ret;
}

bool IF_STATEMENT()
{
	bool ret = false;

	if(token->type == TOKEN_IF)
	{
		token = get_token();
		if(token->type == TOKEN_LROUND_BRACKET)
		{
			CALL_EXPR();
			if(token->type == TOKEN_RROUND_BRACKET)
				ret = NESTED_BLOCK() && ELSE_STATEMENT();
		}
	}

	return ret;
}

bool ELSE_STATEMENT()
{
	bool ret = false;

	switch(token->type)
	{
		case TOKEN_ELSE:
			token = get_token();
			ret = NESTED_BLOCK();
			break;

		case TOKEN_AUTO:
		case TOKEN_INT:
		case TOKEN_DOUBLE:
		case TOKEN_STRING:
		case TOKEN_IDENTIFIER:
		case TOKEN_LENGTH:
		case TOKEN_SUBSTR:
		case TOKEN_CONCAT:
		case TOKEN_FIND:
		case TOKEN_SORT:
		case TOKEN_IF:
		case TOKEN_COUT:
		case TOKEN_CIN:
		case TOKEN_FOR:
		case TOKEN_LCURLY_BRACKET:
		case TOKEN_RETURN:
		case TOKEN_RCURLY_BRACKET:
			ret = true;
			break;
	}

	return ret;
}

bool COUT()
{
	bool ret = false;

	if(token->type == TOKEN_COUT)
	{
		token = get_token();
		if(token->type == TOKEN_COUT_BRACKET)
		{
			token = get_token();
			ret = COUT_OUTPUT() && COUT_NEXT() && token->type == TOKEN_SEMICOLON;
			token = get_token();
		}
	}

	return ret;
}

bool COUT_OUTPUT()
{
	bool ret = false;

	if(token->type == TOKEN_IDENTIFIER)
	{
		token = get_token();
		ret = true;
	}
	else
		ret = HARD_VALUE();

	return ret;
}

bool COUT_NEXT()
{
	bool ret = false;

	if(token->type == TOKEN_COUT_BRACKET)
	{
		token = get_token();
		ret = COUT_OUTPUT() && COUT_NEXT();
	}
	else if (token->type == TOKEN_SEMICOLON)
		ret = true;

	return ret;
}

bool CIN()
{
	bool ret = false;
	bool midway = true;

	if(token->type == TOKEN_CIN)
		token = get_token();
	else
		midway = false;

	if(token->type == TOKEN_CIN_BRACKET)
		token = get_token();
	else
		midway = false;

	if(token->type == TOKEN_IDENTIFIER)
	{
		token = get_token();
		ret = CIN_NEXT() && (token->type == TOKEN_SEMICOLON);
		token = get_token();
	}

	return ret && midway;
}

bool CIN_NEXT()
{
	bool ret = false;

	if(token->type == TOKEN_CIN_BRACKET)
	{
		token = get_token();
		if(token->type == TOKEN_IDENTIFIER)
		{
			token = get_token();
			ret = CIN_NEXT();
		}
	}
	else if(token->type == TOKEN_SEMICOLON)
		ret = true;

	return ret;
}

bool FOR_STATEMENT()
{
	bool ret = false;

	if(token->type == TOKEN_FOR)
	{
		token = get_token();
		if(token->type == TOKEN_LROUND_BRACKET)
		{
			token = get_token();
			if(FOR_DECLARATION() && FOR_EXPR() && FOR_ASSIGN() && token->type == TOKEN_RROUND_BRACKET)
				ret = NESTED_BLOCK();
		}
	}

	return ret;
}

bool FOR_DECLARATION()
{
	bool ret = false;

	if(token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
		if(DATA_TYPE() && token->type == TOKEN_IDENTIFIER)
		{
			token = get_token();
			ret = DECL_ASSIGN() && token->type == TOKEN_SEMICOLON;
			token = get_token();
		}
	}
	else if(token->type == TOKEN_AUTO)
	{
		token = get_token();
		if(token->type == TOKEN_IDENTIFIER)
		{
			token = get_token();
			if(token->type == TOKEN_ASSIGN)
			{
				token = get_token();
				CALL_EXPR();
				ret = (token->type == TOKEN_SEMICOLON);
				token = get_token();
			}
		}
	}

	return ret;
}

bool FOR_EXPR()
{
	bool ret = false;

	ret = (token->type == TOKEN_SEMICOLON);
	token = get_token();

	return ret;
}

bool FOR_ASSIGN()
{
	bool ret = false;

	if(token->type == TOKEN_IDENTIFIER)
	{
		token = get_token();
		if(token->type == TOKEN_ASSIGN)
		{
			token = get_token();
			CALL_EXPR();
			ret = true;
		}
	}

	return ret;
}

bool RETURN()
{
	bool ret = false;

	if(token->type == TOKEN_RETURN)
	{
		token = get_token();
		CALL_EXPR();
		ret = (token->type == TOKEN_SEMICOLON);
		token = get_token();
	}

	return ret;
}

void parse()
{
	htab_t *g_constTab = htab_init(CONSTTAB_INITIAL_SIZE);
	htab_t *g_globalTab = htab_init(GLOBALTAB_INITIAL_SIZE);
	TStack *g_frameStack = stack_init();

	token = get_token();
	if(PROG())
		printf("Syntax OK\n");
	else
		printf("Syntax ERROR\n");
}
