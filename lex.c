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

#include "enums.h"
#include "galloc.h"
#include "lex.h" 
#include "adt.h" 
#include "error.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

//lex.h si vytvor sam..bude tam asi len deklaracia funckie get_token 
//vytvorim asi nejaky subor types.h kde bude ta struktura pre token
//
//v nejakej funkci set token type porovnas len s tymito hodnotami,
//toto pole daj len ako lokalnu premennu funkcie, ak sa to nebude zhodovat,
//tak nacitane slovo je len obycajny identifikator token_identifier
//tie token typy su v enums.h

void TToken add_data()
{

}

void keyword_check(TToken token) // compares identifier with keywords
{
	int i;
	char *token_keywords[] = {"auto", "cin", "cout", "double", "else",
                          	"for", "if", "int", "return", "string", 
                          	"length", "substr", "concat", "find", "sort"};
	for (i=0;i<KEYWORDS_COUNT;i++)
	{
		if (strcmp(token_keywords[i],token.data)==0)
		{
			token.data = token_keywords[i];
			token.type=i;
		}		
	}
}

TToken lex_init(FILE *fp)
{	
  gcInit(); //garbage collector init
	TToken token;
	TString buffer; //rewritable buffer
	States state=S_START;
	bool read=true,dot=false;
	char c;

	initString(&buffer,STR_DEFAULT_SIZE); // buffer initialization

	while(read)
	{
		c=getchar(fp);
		switch(state)
		{
		case S_START:
			switch (c)
			{
				case EOF:
					read=false;
					token.type=TOKEN_ADD;
					return token;
				break;
				case '+':
					token.type=TOKEN_ADD;
					return token;
				break;
				case '-':
					token.type=TOKEN_SUB;
					return token;
				break;
				case '*':
					state=S_MUL;
				break;
				case '/':
					state= S_DIV;
				break;
				case '=':
					state= S_ASSIGN;
				break;
				case '!':
					state= S_EXCM;
				break;
				case '<':
					state= S_LESS;
				break;
				case '>':
					state= S_GREAT;
				break;		
				case '(':
					token.type=TOKEN_LROUND_BRACKET;
					return token;
				break;				
				case ')':
					token.type=TOKEN_RROUND_BRACKET;
					return token;
				break;		
				case '{':
					token.type=TOKEN_LCURLY_BRACKET;
					return token;
				break;	
				case '}':
					token.type=TOKEN_RCURLY_BRACKET;
					return token;
				break;	
				case ',':
					token.type=TOKEN_COMMA;
					return token;
					break;
				case ';':
					printf("'%c' ",c);
					token.type=TOKEN_SEMICOLON;
				break;
				case '"':
					state= S_QUOT;
					return token;
				break;
				default:
					if(isdigit(c))
					{
						state=S_INT;
						insertIntoString(&buffer, c);	
						break;
					}
					else if (isalpha(c) || c == '_')
					{
						state=S_IDENT;
						insertIntoString(&buffer, c);
						break;
					}
					else if (isspace(c))
						break;
			
					else
						state=S_ERROR;								
			}
		break;

//********************************************** 
		case S_INT: // INTEGER
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
			}
			else if(c=='.')
			{
				dot=true;
				insertIntoString(&buffer, c);
				state= S_DOT;				
			}
			else if(c=='E'||c=='e')
			{
				insertIntoString(&buffer, c);
				state= S_EXPO_E;				
			}
			else
			{
				insertIntoString(&buffer, 0);
				ungetc(c,fp);
				token.type=TOKEN_INT_VALUE;
				token.data=buffer.data;
				state=S_START;
				deleteFromString(&buffer,0,buffer.used);
				return token;
			}
		break;

//*************************************************
		case S_DOT: // // INTEGER DOT
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
				state= S_DBL;
			}
			else
			{
				state=S_ERROR;
				deleteFromString(&buffer,0,buffer.used);
			}
		break;

//*************************************************
		case S_DBL: // DOUBLE
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
			}
			else if(c=='E'||c=='e')
			{
				insertIntoString(&buffer, c);
				state= S_EXPO_E;				
			}
			else
			{
				insertIntoString(&buffer, 0);
				ungetc(c,fp);
				token.type=TOKEN_DOUBLE_VALUE;
				token.data=buffer.data;
				state=S_START;
				deleteFromString(&buffer,0,buffer.used);
				return token;
			}
		break;

//******************************************************
		case S_EXPO_E: // state S_EXPO_E
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
				state= S_EXPO;
			}
			else if(c=='+'||c=='-')
			{
				insertIntoString(&buffer, c);
				state= S_EXPO_M;				
			}		
			else
			{
				state=S_ERROR;
				deleteFromString(&buffer,0,buffer.used);
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
				deleteFromString(&buffer,0,buffer.used);
			}
		break;	

//*****************************************************
		case S_EXPO: // state S_EXPO
			if (isdigit(c))
			{
				insertIntoString(&buffer, c);
			}		
			else
			{
				insertIntoString(&buffer, 0);
				ungetc(c,fp);
				
				if (dot=true)
				{
					token.type=TOKEN_DOUBLE_VALUE;
					dot=false;
				}
				else
				{
					token.type=TOKEN_INT_VALUE;
				}
				state=S_START;
				token.data=buffer.data;
				deleteFromString(&buffer,0,buffer.used);
				return token;
			}
		break;
		
//***************************************************
		case S_IDENT: //STATE IDENTIFIER
			if (isalnum(c) || c=='_')
			{
				insertIntoString(&buffer, c);
			}
			else
			{
				insertIntoString(&buffer, 0);
				token.data=buffer.data;
				token.type=TOKEN_IDENTIFIER;
				keyword_check(token);
				ungetc(c,fp);
				state=S_START;
				deleteFromString(&buffer,0,buffer.used);
				return token;
			}
		break;

//*************************************************************
		case S_MUL: // MULTIPLY
			if (c=='/')
				state=S_ERROR;		
			else
			{
				ungetc(c,fp);
				token.type=TOKEN_MUL;
				state=S_START;
				return token;
			}
		break;

//*************************************************************
		case S_DIV: // DIVIDE
			if (c=='/')
				state=S_LCOM;

			else if (c=='*')
				state=S_LBC;			
			else
			{
				ungetc(c,fp);
				token.type=TOKEN_DIV;
				state=S_START;
				return token;
			}
		break;	

//***************************************************
		case S_LCOM: // LINE COMMENT
			if (c=='\n')
				state=S_START;
		break;
				
//****************************************************
		case S_LBC: // LEFT BLOCK COMMENT
			if (c=='*')
				state=S_RBC;
			if(c==EOF)
				state=S_ERROR;
		break;

//****************************************************
		case S_RBC: //RIGHT BLOCK COMMENT
			if (c=='/')
				state=S_START;
			else if (c==EOF)
				state=S_ERROR;
			else
				state=S_LBC;
		break;
				
//****************************************************
		case S_ASSIGN: // ASSIGNMENT
			if (c=='=')
			{
				token.type=TOKEN_EQUAL;
				state=S_START;
				return token;
			}			
			else
			{
				ungetc(c,fp);
				token.type=TOKEN_ASSIGN;
				state=S_START;
				return token;
			}
		break;	
	
//****************************************************
		case S_EXCM: // EXCLAMATION MARK
			if (c=='=')
			{
				token.type=TOKEN_NOT_EQUAL;
				state=S_START;
				return token;
			}			
			else
				state=S_ERROR;
		break;	

//****************************************************
		case S_LESS: // LESS THAN
			if (c=='=')
			{
				token.type=TOKEN_LESS_EQUAL;;
				state=S_START;
				return token;
			}
			else if (c=='<')
			{
				token.type=TOKEN_COUT_BRACKET;
				state=S_START;
				return token;
			}				
			else
			{
				ungetc(c,fp);
				token.type=TOKEN_LESS;
				state=S_START;
				return token;
			}
		break;

//****************************************************
		case S_GREAT: //GREATER THAN
			if (c=='=')
			{
				token.type=TOKEN_GREATER_EQUAL;
				state=S_START;
				return token;
			}
			else if (c=='>')
			{
				token.type=TOKEN_CIN_BRACKET;
				state=S_START;
				return token;
			}				
			else
			{
				ungetc(c,fp);
				token.type=TOKEN_GREATER;
				state=S_START;
				return token;
			}
		break;

//****************************************************
		case S_QUOT: //QUOTATION
			if (c=='"') { 
				insertIntoString(&buffer, c);
				insertIntoString(&buffer, 0); 
				token.type=TOKEN_STRING_VALUE;
				token.data=buffer.data;
				state= S_START;
				deleteFromString(&buffer,0,buffer.used);
				return token;
			}
			else if(c==EOF)
			{
				state=S_ERROR;
				deleteFromString(&buffer,0,buffer.used);
			}
			else
			{
				insertIntoString(&buffer, c);
			}
		 break;

//****************************************************
		case S_ERROR: // ERROR
			freeString(&buffer);
    	gcDestroy();
			exit_error(E_LEX);
		}
	}
	freeString(&buffer);
  gcDestroy();
}
