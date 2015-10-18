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
#include "types.h" 
#include "error.h"

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


char *token_keywords[] = {"auto", "cin", "cout", "double", "else",
                          "for", "if", "int", "return", "string", 
                          "length", "substr", "concat", "find", "sort"};

int main()
{
	States state=S_START;
	bool read=true;
	char c,c_prev;
	char buff[256]; 
	int i, b_i = 0;
	memset(&buff,0,256);

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
					printf("'%c' ",c);
					break;
				case '-':
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
					break;				
				case ')':
					printf("'%c' ",c);
					break;		
				case '{':
					printf("'%c' ",c);
					break;	
				case '}':
					printf("'%c' ",c);
					break;	
				case ',':
					printf("'%c' ",c);
					break;
				case ';':
					printf("'%c' ",c);
					break;
				case '"':
					state= S_QUOT;
					buff[b_i] = c;
					b_i++;
					break;
				default:
					if(isdigit(c))
					{
						state=S_INT;
						buff[b_i] = c;
						b_i++;	
						break;
					}
					else if (isalpha(c) || c == '_')
					{
						state=S_IDENT;
						buff[b_i] = c;
						b_i++;
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
				buff[b_i] = c;  
				b_i++;
			}
			else if(c=='.')
			{
				buff[b_i] = c;  
				b_i++;
				state= S_DOT;				
			}
			else if(c=='E'||c=='e')
			{
				buff[b_i] = c;  
				b_i++;
				state= S_EXPO_E;				
			}
			else
			{
				printf("'%s' ",buff);
				ungetc(c,stdin);
				state=S_START;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
			}
		break;

//*************************************************
		case S_DOT: // // INTEGER DOT
			if (isdigit(c))
			{
				buff[b_i] = c;  
				b_i++;
				state= S_DBL;
			}
			else
			{
				state=S_ERROR;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
			}
		break;

//*************************************************
		case S_DBL: // DOUBLE
			if (isdigit(c))
			{
				buff[b_i] = c;  
				b_i++;
			}
			else if(c=='E'||c=='e')
			{
				buff[b_i] = c;  
				b_i++;
				state= S_EXPO_E;				
			}
			else
			{
				printf("'%s' ",buff);
				ungetc(c,stdin);
				state=S_START;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
			}
		break;

//******************************************************
		case S_EXPO_E: // state S_EXPO_E
			if (isdigit(c))
			{
				buff[b_i] = c;  
				b_i++;
				state= S_EXPO;
			}
			else if(c=='+'||c=='-')
			{
				buff[b_i] = c;  
				b_i++;
				state= S_EXPO_M;				
			}		
			else
			{
				state=S_ERROR;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
			}
		break;			

//*****************************************************
		case S_EXPO_M: // state S_EXPO_M
			if (isdigit(c))
			{
				buff[b_i] = c;  
				b_i++;
				state= S_EXPO;
			}		
			else
			{
				state=S_ERROR;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
			}
		break;	

//*****************************************************
		case S_EXPO: // state S_EXPO
			if (isdigit(c))
			{
				buff[b_i] = c;  
				b_i++;
			}		
			else
			{
				printf("'%s' ",buff);
				ungetc(c,stdin);
				state=S_START;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
			}
		break;
		
//***************************************************
		case S_IDENT: //STATE IDENTIFIER
			if (isalnum(c) || c=='_')
			{
				buff[b_i] = c; 
				b_i++;
			}
			else
			{
				printf("'%s' ",buff);
				ungetc(c,stdin);
				state=S_START;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
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
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}			
			else
			{
				ungetc(c,stdin);
				printf("'%c' ",buff[b_i]);
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
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}			
			else
			{
				state=S_ERROR;
			}
		break;	

//****************************************************
		case S_LESS: // LESS THAN
			if (c=='=')
			{
				b_i++;
				buff[b_i] = c;
				printf("'%s' ",buff);
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
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}				
			else
			{
				ungetc(c,stdin);
				printf("'%c' ",buff[b_i]);
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
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
				state=S_START;
			}				
			else
			{
				ungetc(c,stdin);
				printf("'%c' ",buff[b_i]);
				buff[b_i]=0;
				state=S_START;
			}
		break;

//****************************************************
		case S_QUOT: //QUOTATION
			buff[b_i] = c;  
			b_i++;
			if (c=='"') {  
				printf("'%s' ", buff);
				state= S_START;
				for(i=0; i<=b_i; i++)
					buff[i]=0;
				b_i=0;
			}
		 break;
//****************************************************
		case S_ERROR: // ERROR
			printf ("ERROR\n");
			exit(1);
		}
	}
	printf("\n");
}
