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

bool keyword_check(TToken token) // compares identifier with keywords
{
	int i;
	char *token_keywords[] = {"auto", "cin", "cout", "double", "else",
                          	"for", "if", "int", "return", "string", 
                          	"length", "substr", "concat", "find", "sort"};
	for (i=0;i<15;i++)
	{
		if (strcmp(token_keywords[i],token.data)==0)
		{
			token.data = token_keywords[i];
			token.type=i;
			printf("'/%s/' ", token.data);
			return true;
		}		
	}
	return false;
}

int main()
{	

  gcInit(); //garbage collector init
	TToken token;
	TString buffer; //rewritable buffer
	States state=S_START;
	bool read=true,dot=true;
	char c;
	char buff[1024]; 
	int i, b_i = 0;
	memset(&buff,0,1024);

	initString(&buffer,STR_DEFAULT_SIZE); // buffer initialization

	while(read)
	{
		c=getchar();
		switch(state)
		{
		case S_START:
			switch (c)
			{
				case EOF:
					read=false;
				break;
				case '+':
					token.type=TOKEN_ADD;
					printf("'%c' ",c);
				break;
				case '-':
					token.type=TOKEN_SUB;
					printf("'%c' ",c);
				break;
				case '*':
					state=S_MUL;
					buff[b_i]=c;
				break;
				case '/':
					state= S_DIV;
					buff[b_i] = c;
				break;
				case '=':
					state= S_ASSIGN;
					buff[b_i] = c;
				break;
				case '!':
					state= S_EXCM;
					buff[b_i] = c;
				break;
				case '<':
					state= S_LESS;
					buff[b_i] = c;
				break;
				case '>':
					state= S_GREAT;
					buff[b_i] = c;
				break;		
				case '(':
					printf("'%c' ",c);
					token.type=TOKEN_LROUND_BRACKET;
				break;				
				case ')':
					printf("'%c' ",c);
					token.type=TOKEN_RROUND_BRACKET;
				break;		
				case '{':
					printf("'%c' ",c);
					token.type=TOKEN_LCURLY_BRACKET;
				break;	
				case '}':
					printf("'%c' ",c);
					token.type=TOKEN_RCURLY_BRACKET;
				break;	
				case ',':
					printf("'%c' ",c);
					token.type=TOKEN_COMMA;
					break;
				case ';':
					printf("'%c' ",c);
					token.type=TOKEN_SEMICOLON;
				break;
				case '"':
					state= S_QUOT;
					insertIntoString(&buffer, c);
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
				printf("'%s' ",buffer.data);
				ungetc(c,stdin);
				token.type=TOKEN_INT_VALUE;
				state=S_START;
				deleteFromString(&buffer,0,buffer.used);
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
				printf("'%s' ",buff);
				ungetc(c,stdin);
				token.type=TOKEN_DOUBLE_VALUE;
				state=S_START;
				deleteFromString(&buffer,0,buffer.used);
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
				printf("'%s' ",buffer.data);
				ungetc(c,stdin);
				
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
				deleteFromString(&buffer,0,buffer.used);
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
				if(keyword_check(token)==false)
				{
					printf("'%s' ",buffer.data);
				}
				ungetc(c,stdin);
				state=S_START;
				deleteFromString(&buffer,0,buffer.used);
			}
		break;

//*************************************************************
		case S_MUL: // MULTIPLY
			if (c=='/')
				state=S_ERROR;		
			else
			{
				ungetc(c,stdin);
				printf("'%c' ",buff[b_i]);
				token.type=TOKEN_MUL;
				buff[b_i]=0;
				state=S_START;
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
				ungetc(c,stdin);
				printf("'%c' ",buff[b_i]);
				buff[b_i]=0;
				token.type=TOKEN_DIV;
				state=S_START;
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
				b_i++;
				buff[b_i] = c;
				printf("'%s' ",buff);
				token.type=TOKEN_EQUAL;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}			
			else
			{
				ungetc(c,stdin);
				printf("'%c' ",buff[b_i]);
				token.type=TOKEN_ASSIGN;
				buff[b_i]=0;
				state=S_START;
			}
		break;	
	
//****************************************************
		case S_EXCM: // EXCLAMATION MARK
			if (c=='=')
			{
				b_i++;
				buff[b_i] = c;
				printf("'%s' ",buff);
				token.type=TOKEN_NOT_EQUAL;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}			
			else
				state=S_ERROR;
		break;	

//****************************************************
		case S_LESS: // LESS THAN
			if (c=='=')
			{
				b_i++;
				buff[b_i] = c;
				printf("'%s' ",buff);
				token.type=TOKEN_LESS_EQUAL;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}
			else if (c=='<')
			{
				b_i++;
				buff[b_i] = c;
				printf("'%s' ",buff);
				token.type=TOKEN_COUT_BRACKET;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}				
			else
			{
				ungetc(c,stdin);
				printf("'%c' ",buff[b_i]);
				token.type=TOKEN_LESS;
				buff[b_i]=0;
				state=S_START;
			}
		break;

//****************************************************
		case S_GREAT: //GREATER THAN
			if (c=='=')
			{
				b_i++;
				buff[b_i] = c;
				printf("'%s' ",buff);
				token.type=TOKEN_GREATER_EQUAL;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}
			else if (c=='>')
			{
				b_i++;
				buff[b_i] = c;
				printf("'%s' ",buff);
				token.type=TOKEN_CIN_BRACKET;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}				
			else
			{
				ungetc(c,stdin);
				printf("'%c' ",buff[b_i]);
				token.type=TOKEN_GREATER;
				buff[b_i]=0;
				state=S_START;
			}
		break;

//****************************************************
		case S_QUOT: //QUOTATION
			if (c=='"') { 
				insertIntoString(&buffer, c);
				insertIntoString(&buffer, 0); 
				printf("'%s' ", buffer.data);
				token.type=TOKEN_STRING_VALUE;
				state= S_START;
				deleteFromString(&buffer,0,buffer.used);
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
			printf("ERROR\n");
			freeString(&buffer);
    	gcDestroy();
			exit_error(E_LEX);
		}
	}
	printf("\n");
	freeString(&buffer);
  gcDestroy();
}
