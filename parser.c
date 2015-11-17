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
#include <stdlib.h>
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
#include "expr.h"

TToken* token;

// Forward declarations of state functions
bool PROG();
bool FUNCTION_DECL();
bool DATA_TYPE();
bool FUNC_DECL_PARAMS();
bool FUNC_DECL_PARAMS_NEXT();
bool NESTED_BLOCK();
bool NBC();
bool DECL_OR_ASSIGN();
bool DECL_ASSIGN();
bool ASSIGN();
bool HARD_VALUE();
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


// Forward declarations of control functions
TFunction *getNewFunction();
TVariable *getNewVariable();
void pushParam();
void storeFuncName();
void storeVarName();
void storeFunction();
void storeVariable();
TVariable *findVariable();
TList_item *createInstruction();
void createPseudoFrame();

enum
{
	T_FUNC,
	T_VAR
};


TList_item *createInstruction(int type, void *addr1, void *addr2, void *addr3)
{
  TList_item *ins = gmalloc(sizeof(TList_item));
  ins->ins_type = type;
  ins->addr1 = addr1;
  ins->addr2 = addr2;
  ins->addr3 = addr3;

  return ins;
}

// ============== Control functions start here ==============

void storeFunction(TFunction *f)
{
  // Check if the function has already been declared
  htab_item *result = htab_lookup(G.g_globalTab, f->name);
  if(result)
  {
    // Check if return type matches
    if(!(result->data.function->return_type == f->return_type))
     exit_error(E_SEMANTIC_DEF);
    // Replace forward declaration with definition
    gfree(result->data.function);
    result->data.function = f;
  }
  else
  {
    // Function has not been declared before
    htab_item *newFunc = htab_insert(G.g_globalTab, f->name);
    newFunc->data.function = f;
  }

}

void createPseudoFrame()
{
  // Get current function
  TFunction *current = stack_top(G.g_frameStack);

  // Create and push a pseudo-function which represents a nested block
  TFunction *f = gmalloc(sizeof(TFunction));
	f->name = NULL;
	f->return_type = 0;
	f->defined = 1;
	f->ins_list = current->ins_list;
	f->local_tab = htab_init(HTAB_SIZE);
	f->params_stack = NULL;

  // Push this on the frameStack
  stack_push(G.g_frameStack, f);

  // Create the instruction
  TList_item *ins = createInstruction(INS_PUSH_TAB, f->local_tab, NULL, NULL);
  list_insert(f->ins_list, ins);
}

void destroyPseudoFrame()
{
  stack_pop(G.g_frameStack);
}

void storeNewVariable(TFunction *f, TVariable *v)
{
  // Check for redeclaration
  if(htab_lookup(f->local_tab, v->name))
    exit_error(E_SEMANTIC_DEF);

  htab_item *newVar = htab_insert(f->local_tab, v->name);
  newVar->data.variable = v;

}

void storeNewConstant(TVariable *c)
{
  // Check if the constant already has an entry
  if(htab_lookup(G.g_constTab, c->name))
     return;

  htab_item *newConst = htab_insert(G.g_constTab, c->name);
  newConst->data.variable = c;

}

// This will search the frame stack for the var and return it (returns NULL if the var is not found)
TVariable *findVariable(char *name)
{
  // Search all the frames for the variable
  for(int i=G.g_frameStack->used-1; i >= 0; i--)
  {
    TFunction *f = G.g_frameStack->data[i];
    htab_item *found = htab_lookup(f->local_tab, name);

    if(found)
      return found->data.variable;
  }

  return NULL;
}

TFunction *getNewFunction()
{
  TFunction *f = gmalloc(sizeof(TFunction));

	f->name = NULL;
	f->return_type = 0;
	f->defined = 0;
	f->ins_list = list_init();
	f->local_tab = htab_init(HTAB_SIZE);
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

// ============== End of control functions ===============

// ============== Rule functions start here ==============
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

  // Tell everyone we are currently in a new function (block)
  stack_push(G.g_frameStack, currentFunc);

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

      // Function has been declared but not defined
      if(token->type == TOKEN_SEMICOLON)
      {
        currentFunc->defined = false;
        token = get_token();
      }
      // Process function block if the function is defined
			else
      {
        currentFunc->defined = true;
        if(!NESTED_BLOCK())
          return false;
      }

      // Store the complete function "object" in the global table
      storeFunction(currentFunc);
		}
	}

  // Function has been processed, jump out of block
	stack_pop(G.g_frameStack);

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

// NOTE: This function can and will be called with indirect recursion from FUNC_DECL_PARAMS_NEXT
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

      // Push param on stack
      pushParam(func, param);

      // Process next param
			if(!FUNC_DECL_PARAMS_NEXT(func))
        return false;
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

bool NESTED_BLOCK()
{
	if (token->type == TOKEN_LCURLY_BRACKET)
	{
		token = get_token();
		if(NBC() && token->type == TOKEN_RCURLY_BRACKET)
		{
      // Pop current function from frame stack
      stack_pop(G.g_frameStack);

      token = get_token();
			return true;
		}
	}

	return false;
}

bool NBC()
{
	switch(token->type)
	{
		case TOKEN_AUTO:
		case TOKEN_INT:
		case TOKEN_DOUBLE:
		case TOKEN_STRING:
			return DECL_OR_ASSIGN() && NBC();

		case TOKEN_IDENTIFIER:
			return ASSIGN() && NBC();

		case TOKEN_IF:
			return IF_STATEMENT() && NBC();

		case TOKEN_COUT:
			return COUT() && NBC();

		case TOKEN_CIN:
			return CIN() && NBC();

		case TOKEN_FOR:
			return FOR_STATEMENT() && NBC();

		case TOKEN_LCURLY_BRACKET:
      createPseudoFrame();
			return NESTED_BLOCK() && NBC();

		case TOKEN_RETURN:
			return RETURN();

		case TOKEN_RCURLY_BRACKET:
			return true;

		default:
			return false;
	}
}

bool DECL_OR_ASSIGN()
{
  // Create new variable "object"
  TVariable *var = getNewVariable();
  // The variable belongs to the function or block on top of the frame stack
  TFunction *func = stack_top(G.g_frameStack);

  // Received identifier, expecting declaration or declaration with assignment
	if (token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
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

    return true;
	}

  // Received auto, declaration must contain an assignment
	else if (token->type == TOKEN_AUTO)
	{
		token = get_token();

		if (token->type == TOKEN_IDENTIFIER)
    {
      storeVarName(var);
      token = get_token();
    }
    // Syntax error
		else
			return false;

    // There must be an assignment
		if (token->type == TOKEN_ASSIGN)
		{
			token = get_token();
			expression(var, func->ins_list);
		}
    // Syntax error
		else
			return false;

    // Syntax error
		if(!(token->type == TOKEN_SEMICOLON))
      return false;

    // TODO: Set init and type


    // Store variable in function symbol table
    storeNewVariable(func, var);

    token = get_token();

    return true;
	}

  // Received unexpected token, syntax error
	return false;

}

bool DECL_ASSIGN(TVariable *var)
{
  // Current function
  TFunction *func = stack_top(G.g_frameStack);

  // We must initialize the variable
	if(token->type == TOKEN_ASSIGN)
	{
		expression(var, func->ins_list);
    token = get_token();
		return true;
	}

  // Variable was only declared, not initialized
	else if(token->type == TOKEN_SEMICOLON)
	{
    return true;
	}
  // Syntax error
	return false;
}

bool ASSIGN()
{
  // Current function
  TFunction *func = stack_top(G.g_frameStack);

	if(token->type == TOKEN_IDENTIFIER)
	{
    // Retrieve the variable which we will be assigning to
    TVariable *var = findVariable(token->data);
    // Cannot assign to an undefined variable
    if(var == NULL)
      exit_error(E_SEMANTIC_OTHERS);

    if(token->type == TOKEN_ASSIGN)
      expression(var, func->ins_list);
    // Syntax error
    else
      return false;

    return true;
	}

  // Syntax error, unexpected token
  return false;
}

bool HARD_VALUE(TVariable **v)
{
  // If we get a magic value, add to constants table if its not already there
	if (token->type == TOKEN_INT_VALUE || token->type == TOKEN_DOUBLE_VALUE || token->type == TOKEN_STRING_VALUE)
  {
    // New constant
    TVariable *var = getNewVariable();

    // Set constant properties
    switch(token->type)
    {
      case TOKEN_INT_VALUE:
        var->var_type = TYPE_INT;
        var->name = gmalloc(strlen(token->data) + 1);
        strcpy(var->name, token->data);
        var->data.i = strtod(token->data, NULL);
        break;
      case TOKEN_DOUBLE_VALUE:
        var->var_type = TYPE_DOUBLE;
        var->name = gmalloc(strlen(token->data) + 1);
        strcpy(var->name, token->data);
        var->data.d = strtod(token->data, NULL);
        break;
      case TOKEN_STRING_VALUE:
        var->var_type = TYPE_STRING;
        var->name = gmalloc(strlen(token->data) + 1);
        strcpy(var->name, token->data);
        var->data.str = var->name;
        break;
    }
    var->initialized = true;

    // Pass pointer to constant to caller
    *v = var;

    token = get_token();

    return true;

  }
  // Syntax error
  else
    return false;

}

bool IF_STATEMENT()
{
  /*
	bool ret = false;

	if(token->type == TOKEN_IF)
	{
		token = get_token();
		if(token->type == TOKEN_LROUND_BRACKET)
		{
			//CALL_EXPR();
			if(token->type == TOKEN_RROUND_BRACKET)
				ret = NESTED_BLOCK() && ELSE_STATEMENT();
		}
	}

	return ret;
  */
  return true;
}

bool ELSE_STATEMENT()
{
  /*
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
  */
  return true;
}

bool COUT()
{
	if(token->type == TOKEN_COUT)
	{
		token = get_token();
		if(token->type == TOKEN_COUT_BRACKET)
		{
			token = get_token();
			if(COUT_OUTPUT() && COUT_NEXT() && token->type == TOKEN_SEMICOLON)
      {
			  token = get_token();
        return true;
      }
		}
  }

  return false;
}

bool COUT_OUTPUT()
{
  // Get current function
  TFunction *func = stack_top(G.g_frameStack);

  // Print a variable
	if(token->type == TOKEN_IDENTIFIER)
	{
    // Find the variable
    TVariable *var = findVariable(token->data);

    // Undefined var, semnatic error
    if(var == NULL)
      exit_error(E_SEMANTIC_DEF);

    // Create instruction
    TList_item *ins = createInstruction(INS_COUT, var, NULL, NULL);

    // Append instruction to current ins list
    list_insert(func->ins_list, ins);

		token = get_token();
		return true;
	}
  // Print a literal
	else
  {
		// Store the constant here
    TVariable **con = NULL;

    if(!HARD_VALUE(con))
      // Syntax error
      return false;

    TList_item *ins = createInstruction(INS_COUT, con, NULL, NULL);

    list_insert(func->ins_list, ins);

    return true;
  }

  // Syntax error
	return false;
}

bool COUT_NEXT()
{
	if(token->type == TOKEN_COUT_BRACKET)
	{
		token = get_token();
		return COUT_OUTPUT() && COUT_NEXT();
	}
	else if (token->type == TOKEN_SEMICOLON)
		return true;

  // Syntax error
	return false;
}

bool CIN()
{
  // Current function
  TFunction *func = stack_top(G.g_frameStack);

	if(token->type == TOKEN_CIN)
  {
		token = get_token();

	  if(token->type == TOKEN_CIN_BRACKET)
    {
		  token = get_token();

	    if(token->type == TOKEN_IDENTIFIER)
	    {
        // Find the variable
        TVariable *var = findVariable(token->data);
        if(var == NULL)
          // Semantic error
          exit_error(E_SEMANTIC_DEF);

        // Generate insert instruction
        TList_item *ins = createInstruction(INS_CIN, var, NULL, NULL);
        // Append instruction to current function
        list_insert(func->ins_list, ins);

		    token = get_token();
		    if(CIN_NEXT() && (token->type == TOKEN_SEMICOLON))
        {
		      token = get_token();
          return true;
        }
	    }
    }
  }

	// Syntax error
  return false;
}

bool CIN_NEXT()
{
	// Current function
  TFunction *func = stack_top(G.g_frameStack);

	if(token->type == TOKEN_CIN_BRACKET)
  {
	  token = get_token();

    if(token->type == TOKEN_IDENTIFIER)
    {
      // Find the variable
      TVariable *var = findVariable(token->data);
      if(var == NULL)
        // Semantic error
        exit_error(E_SEMANTIC_DEF);

      // Generate insert instruction
      TList_item *ins = createInstruction(INS_CIN, var, NULL, NULL);
      // Append instruction to current function
      list_insert(func->ins_list, ins);

	    token = get_token();
	    if(CIN_NEXT() && (token->type == TOKEN_SEMICOLON))
      {
	      token = get_token();
        return true;
      }
    }
  }
	else if(token->type == TOKEN_SEMICOLON)
		return true;

	// Syntax error
  return false;
}

bool FOR_STATEMENT()
{
  /*
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
  */
  return true;
}

bool FOR_DECLARATION()
{
  /*
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
  */
  return true;
}

bool FOR_EXPR()
{
  /*
	bool ret = false;

	ret = (token->type == TOKEN_SEMICOLON);
	token = get_token();

	return ret;
  */
  return true;
}

bool FOR_ASSIGN()
{
  /*
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
  */
  return true;
}

bool RETURN()
{
  // Currently processed function
  TFunction *func = stack_top(G.g_frameStack);

	if(token->type == TOKEN_RETURN)
	{
    token = get_token();

    G.g_return->var_type = func->return_type;
		expression(G.g_return, func->ins_list);

    // Generate return instructions
    TList_item *ins = createInstruction(INS_RET, NULL, NULL, NULL);
    list_insert(func->ins_list, ins);

		if(token->type == TOKEN_SEMICOLON)
    {
      token = get_token();
      return true;
    }

	}

  // Syntax error
  return false;
}

void parse()
{
	token = get_token();
	if(PROG())
		printf("Syntax OK\n");
	else
		printf("Syntax ERROR\n");
}
