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
#include "parser.h"
#include "adt.h"
#include "lex.h"
#include "galloc.h"
#include "enums.h"
#include "error.h"
#include "shared.h"

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

void CALL_EXPR()
{
}

// Definitions of "rule" functions
bool PROG()
{
	bool ret = false;

	if (token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
		ret = FUNCTION_DECL() && PROG();
	}
	else if (token->type == TOKEN_EOF)
	{
		ret = true;
	}

	return ret;
}

bool FUNCTION_DECL()
{
	bool ret = false;
	bool midway = true;

	if (token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
		if(DATA_TYPE())
		{
			if (token->type == TOKEN_IDENTIFIER)
				token = get_token();
			else
				midway = false;
			
			if (token->type == TOKEN_LROUND_BRACKET)
				token = get_token();
			else
				midway = false;

			if(FUNC_DECL_PARAMS())
			{
				if (token->type == TOKEN_RROUND_BRACKET)
				{
					token = get_token();
					ret = NESTED_BLOCK();
				}
			}
		}
	}

	return ret && midway;
}

bool DATA_TYPE()
{
	bool ret = false;

	if (token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
		token = get_token();
		ret = true;
	}

	return ret;
}

bool FUNC_DECL_PARAMS()
{
	bool ret = false;

	if (token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
		if(DATA_TYPE() && token->type == TOKEN_IDENTIFIER)
		{
			token = get_token();
			ret = FUNC_DECL_PARAMS_NEXT();
		}
	}
	else if (token->type == TOKEN_RROUND_BRACKET)
	{
		ret = true;
	}

	return ret;
}

bool FUNC_DECL_PARAMS_NEXT()
{
	bool ret = false;

	if (token->type == TOKEN_COMMA)
	{
		token = get_token();
		ret = FUNC_DECL_PARAMS();
	}
	else if (token->type == TOKEN_RROUND_BRACKET)
	{
		ret = true;
	}

	return ret;
}

bool NESTED_BLOCK()
{
	bool ret = false;

	if (token->type == TOKEN_LCURLY_BRACKET)
	{
		token = get_token();
		if(NBC())
		{
			ret = (token->type == TOKEN_RCURLY_BRACKET);
			token = get_token();
		}
	}

	return ret;
}

bool NBC()
{
	bool ret = false;
	
	switch(token->type)
	{
		case TOKEN_AUTO:
		case TOKEN_INT:
		case TOKEN_DOUBLE:
		case TOKEN_STRING:
			ret = DECL_OR_ASSIGN() && NBC();
			break;
		
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

bool DECL_OR_ASSIGN()
{
	bool ret = false;
	bool midway = true;

	if (token->type == TOKEN_INT || token->type == TOKEN_DOUBLE || token->type == TOKEN_STRING)
	{
		if(DATA_TYPE() && token->type == TOKEN_IDENTIFIER)
		{
			token = get_token();
			if(DECL_ASSIGN())
			{
				ret = (token->type == TOKEN_SEMICOLON);
				token = get_token();
			}
		}
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

bool DECL_ASSIGN()
{
	bool ret = false;

	if(token->type == TOKEN_ASSIGN)
	{
		token = get_token();
		CALL_EXPR();
		ret = true;
	}
	else if(token->type == TOKEN_SEMICOLON)
	{
		ret = true;
	}

	return ret;
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
	TConstTab *ConstTab = gmalloc(sizeof(TConstTab));
	TGlobalTab *GlobalTab = gmalloc(sizeof(TGlobalTab));
	
	initConstTab(ConstTab);
	initGlobalTab(GlobalTab);

	token = get_token();
	if(PROG())
		printf("Syntax OK\n");
	else
		printf("Syntax ERROR\n");
}
