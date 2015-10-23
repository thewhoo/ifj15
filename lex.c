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
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "enums.h"
#include "galloc.h"
#include "lex.h" 
#include "adt.h" 
#include "error.h"
#include "string.h"

FILE *fp;

void lex_init(FILE *f)
{
    fp = f;
}

void keyword_check(TToken* token) // compares identifier with keywords
{
	int i;
	char *token_keywords[] = {"auto", "cin", "cout", "double", "else",
                          	"for", "if", "int", "return", "string", 
                          	"length", "substr", "concat", "find", "sort"};
	for (i = 0; i < KEYWORDS_COUNT; i++)
		if (strcmp(token_keywords[i], token->data) == 0)
			token->type = i;
}

TToken* get_token()
{	
	TToken *token = gmalloc(sizeof(TToken));
	TString buffer; 
	States state = S_START;
	int c;

	initString(&buffer,STR_DEFAULT_SIZE); 
	token->data = buffer.data;
	while(1)
	{
		c = fgetc(fp);
		switch(state)
		{
		case S_START:
			switch (c)
			{
				case EOF:
					token->type = TOKEN_EOF;
					return token;
				case '+':
					token->type = TOKEN_ADD;
					return token;
				case '-':
					token->type = TOKEN_SUB;
					return token;
				case '*':
					token->type = TOKEN_MUL;
					return token;
				case '/':
					state = S_DIV;
				    break;
				case '=':
					state = S_ASSIGN;
				    break;
				case '!':
					state = S_EXCM;
				    break;
				case '<':
					state = S_LESS;
				    break;
				case '>':
					state = S_GREAT;
				    break;		
				case '(':
					token->type = TOKEN_LROUND_BRACKET;
					return token;
				case ')':
					token->type = TOKEN_RROUND_BRACKET;
					return token;
				case '{':
					token->type = TOKEN_LCURLY_BRACKET;
					return token;
				    break;	
				case '}':
					token->type = TOKEN_RCURLY_BRACKET;
					return token;
				    break;	
				case ',':
					token->type = TOKEN_COMMA;
					return token;
				case ';':
					token->type = TOKEN_SEMICOLON;
				    return token;
                case '"':
					state = S_QUOT;
				    break;
				default:
					if(isdigit(c))
					{
						state = S_INT;
						insertIntoString(&buffer, c);	
					}
					else if (isalpha(c) || c == '_')
					{
						state = S_IDENT;
						insertIntoString(&buffer, c);
					}
					else if (isspace(c))
						break;	
					else
						state = S_ERROR;								
			}
		    break;

//********************************************** 
		case S_INT: // INTEGER
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
			}
			else if(c == '.')
			{
				insertIntoString(&buffer, c);
				state = S_DOT;				
			}
			else if(c == 'E' || c == 'e')
			{
				insertIntoString(&buffer, c);
				state = S_EXPO_E;				
			}
			else
			{
				insertIntoString(&buffer, 0);
				ungetc(c,fp);
				token->type = TOKEN_INT_VALUE;
				return token;
			}
		    break;

//*************************************************
		case S_DOT: // // INTEGER DOT
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
				state = S_DBL;
			}
			else if(c == 'E' || c == 'e')
			{
				insertIntoString(&buffer, c);
				state = S_EXPO_E;	
			}	
			else
			{
				state = S_ERROR;
			}
		    break;

//*************************************************
		case S_DBL: // DOUBLE
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
			}
			else if(c == 'E' || c == 'e')
			{
				insertIntoString(&buffer, c);
				state = S_EXPO_E;				
			}
			else
			{
				insertIntoString(&buffer, 0);
				ungetc(c,fp);
				token->type = TOKEN_DOUBLE_VALUE;
				return token;
			}
		    break;

//******************************************************
		case S_EXPO_E: // state S_EXPO_E
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
				state = S_EXPO;
			}
			else if (c == '+' || c == '-')
			{
				insertIntoString(&buffer, c);
				state= S_EXPO_M;				
			}		
			else
			{
				state = S_ERROR;
			}
		    break;			

//*****************************************************
		case S_EXPO_M: // state S_EXPO_M
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
				state= S_EXPO;
			}		
			else
			{
				state=S_ERROR;
			}
		    break;	

//*****************************************************
		case S_EXPO: // state S_EXPO
			if (isdigit(c))
				insertIntoString(&buffer, c);
			else
			{
				insertIntoString(&buffer, 0);
				ungetc(c,fp);
				token->type = TOKEN_DOUBLE_VALUE;
				return token;
			}
		    break;
		
//***************************************************
		case S_IDENT: //STATE IDENTIFIER
			if (isalnum(c) || c == '_')
			{
				insertIntoString(&buffer, c);
			}
			else
			{
				insertIntoString(&buffer, 0);
				token->type = TOKEN_IDENTIFIER;
				keyword_check(token);
				ungetc(c,fp);
				return token;
			}
		    break;
//*************************************************************
		case S_DIV: // DIVIDE
			if (c == '/')
				state = S_LCOM;

			else if (c == '*')
				state = S_LBC;			
			else
			{
				ungetc(c,fp);
				token->type = TOKEN_DIV;
				return token;
			}
		    break;	

//***************************************************
		case S_LCOM: // LINE COMMENT
			if (c == '\n')
				state = S_START;
		    break;
				
//****************************************************
		case S_LBC: // LEFT BLOCK COMMENT
			if (c == '*')
				state = S_RBC;
			if(c == EOF)
				state = S_ERROR;
		    break;

//****************************************************
		case S_RBC: //RIGHT BLOCK COMMENT
			if (c == '/')
				state = S_START;
			else if (c == EOF)
				state = S_ERROR;
			else
				state = S_LBC;
		break;
				
//****************************************************
		case S_ASSIGN: // ASSIGNMENT
			if (c == '=')
			{
				token->type = TOKEN_EQUAL;
				return token;
			}			
			else
			{
				ungetc(c,fp);
				token->type = TOKEN_ASSIGN;
				return token;
			}
	      	 break;	
	
//****************************************************
		case S_EXCM: // EXCLAMATION MARK
			if (c == '=')
			{
				token->type = TOKEN_NOT_EQUAL;
				return token;
			}			
			else
				state = S_ERROR;
		   break;	

//****************************************************
		case S_LESS: // LESS THAN
			if (c=='=')
			{
				token->type=TOKEN_LESS_EQUAL;
				return token;
			}
			else if (c == '<')
			{
				token->type=TOKEN_COUT_BRACKET;
				return token;
			}				
			else
			{
				ungetc(c,fp);
				token->type = TOKEN_LESS;
				return token;
			}
		   break;

//****************************************************
		case S_GREAT: //GREATER THAN
			if (c=='=')
			{
				token->type = TOKEN_GREATER_EQUAL;
				return token;
			}
			else if (c == '>')
			{
				token->type = TOKEN_CIN_BRACKET;
				return token;
			}				
			else
			{
				ungetc(c,fp);
				token->type = TOKEN_GREATER;
				return token;
			}
		   break;

//****************************************************
		case S_QUOT: //QUOTATION
			if (c == '"') { 
				insertIntoString(&buffer, 0); 
				token->type = TOKEN_STRING_VALUE;
				return token;
			}
			else if(c == EOF)
			{
				state = S_ERROR;
			}
			else
			{
				insertIntoString(&buffer, c);
			}
		   break;

//****************************************************
		case S_ERROR: // ERROR
			exit_error(E_LEX);
		}
	}
}
