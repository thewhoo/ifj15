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
 #include "parser.h"
 #include "adt.h"
 #include "stack.h"
 #include "lex.h"
 #include "galloc.h"
 #include "enums.h"
 #include "error.h"

 #define LL_ROWS 30
 #define LL_COLS 35

 #define RULES 62
 #define MAX_PUSHED_STATES 8


 /* LL Table definition
  * Each row number corresponds to Token Number + 1
  * Below is how the top row header would look like in tokens

t_auto,t_cin,t_cout,t_double,t_else,t_for,t_if,t_int,t_return,t_string,token_bf_length,token_bf_substr,token_bf_concat,token_bf_find,token_bf_sort,res,res,res,res,res,t_identifier,t_eof,t_string_value,t_int_value,t_double_value,t_assign,t_semicolon,t_comma,t_cout_bracket,t_cin_bracket,t_lround_bracket,t_rround_bracket,t_lcurly_bracket,t_rcurly_bracket,t_expression

  *
  */


 int LLtab[LL_ROWS][LL_COLS] = {

 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,3,0,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,5,0,0,0,4,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,7,0,0,0,7,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,10,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0},
 	{0,12,17,16,12,0,18,15,12,20,12,14,14,14,14,14,0,0,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,19,21},
 	{0,23,0,0,22,0,0,0,22,0,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,25,0,0,0,24,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,27,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,29,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,31,32,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,34,34,34,0,0,0,0,0,0,35,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,37,37,37,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,39,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,40,40,40,40,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,41,42,43,44,45,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,46,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,48,48,48,48,47,48,48,48,48,48,48,48,48,48,48,0,0,0,0,0,48,0,0,0,0,0,0,0,0,0,0,0,48,48},
 	{0,0,0,49,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,50,0,51,51,51,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,53,0,52,0,0,0,0,0},
 	{0,0,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,0,0,55,0,0,0,0},
 	{0,0,0,0,0,0,57,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,59,0,0,58,0,0,0,58,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,60,0,0,0,0,0,0,0,0,0,0,0,0,0},
 	{0,0,0,0,0,0,0,0,0,61,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

 };

 /* Rules definition
  * States are pushed on stack in reverse order
  */

  int rules[RULES][MAX_PUSHED_STATES] = {

  	{0,0,0,0,0,0,0,0},	// Rule 0 - placeholder rule
  	{1,2,0,0,0,0,0,0},	// Rule 1
  	{0,0,0,0,0,0,0,0},	// Rule 2 - epsilon rule
  	{6,-32,4,-31,-21,3},	// Rule 3
  	{-8,0,0,0,0,0,0,0},	// Rule 4
  	{-4,0,0,0,0,0,0,0},	// Rule 5
  	{-10,0,0,0,0,0,0,0},	// Rule 6
  	{5,-21,3,0,0,0,0,0},	// Rule 7
  	{0,0,0,0,0,0,0,0},	// Rule 8 - epsilon rule
  	{4,-28,0,0,0,0,0,0},	// Rule 9
  	{0,0,0,0,0,0,0,0},	// Rule 10 - epsilon rule
  	{-34,7,-33,0,0,0,0,0},	// Rule 11
  	{7,8,0,0,0,0,0,0},	// Rule 12
  	{7,11,0,0,0,0,0,0},	// Rule 13
  	{7,17,0,0,0,0,0,0},	// Rule 14
  	{7,19,0,0,0,0,0,0},	// Rule 15
  	{7,21,0,0,0,0,0,0},	// Rule 16
  	{7,24,0,0,0,0,0,0},	// Rule 17
  	{7,26,0,0,0,0,0,0},	// Rule 18
  	{7,6,0,0,0,0,0,0},	// Rule 19
  	{29,0,0,0,0,0,0,0},	// Rule 20
  	{0,0,0,0,0,0,0,0},	// Rule 21 - epsilon rule
  	{9,-21,3,0,0,0,0,0},	// Rule 22
  	{-27,30,-26,-21,-1,0,0,0}, // Rule 23
  	{6,-32,4,-31,0,0,0,0},	// Rule 24
  	{-27,10,0,0,0,0,0,0},	// Rule 25
  	{30,-26,0,0,0,0,0,0},	// Rule 26
  	{0,0,0,0,0,0,0,0},	// Rule 27 - epsilon rule
  	{12,-21,0,0,0,0,0,0},	// Rule 28
  	{-27,-32,14,-31,0,0,0,0}, // Rule 29
  	{-27,30,-26,0,0,0,0,0},	// Rule 30
  	{-24,0,0,0,0,0,0,0},	// Rule 31
  	{-25,0,0,0,0,0,0,0},	// Rule 32
  	{-23,0,0,0,0,0,0,0},	// Rule 33
  	{16,15,0,0,0,0,0,0},	// Rule 34
  	{0,0,0,0,0,0,0,0},	// Rule 35 - epsilon rule
  	{-21,0,0,0,0,0,0,0},	// Rule 36
  	{13,0,0,0,0,0,0,0},	// Rule 37
  	{14,-28,0,0,0,0,0,0},	// Rule 38
  	{0,0,0,0,0,0,0,0},	// Rule 39 - epsilon rule
  	{-27,-32,14,-31,18,0,0,0}, // Rule 40
  	{-11,0,0,0,0,0,0,0},	// Rule 41
  	{-12,0,0,0,0,0,0,0},	// Rule 42
  	{-13,0,0,0,0,0,0,0},	// Rule 43
  	{-14,0,0,0,0,0,0,0},	// Rule 44
  	{-15,0,0,0,0,0,0,0},	// Rule 45
  	{20,6,-32,30,-31,-7,0,0}, // Rule 46
  	{6,-5,0,0,0,0,0,0},	// Rule 47
  	{0,0,0,0,0,0,0,0},	// Rule 48 - epsilon rule
  	{-27,23,22,-29,-3,0,0,0}, // Rule 49
  	{-21,0,0,0,0,0,0,0},	// Rule 50
  	{13,0,0,0,0,0,0,0},	// Rule 51
  	{23,22,-29,0,0,0,0,0},	// Rule 52
  	{0,0,0,0,0,0,0,0},	// Rule 53 - epsilon rule
  	{-27,25,-21,-30,-2,0,0,0}, // Rule 54
  	{25,-21,-30,0,0,0,0,0},	// Rule 55
  	{0,0,0,0,0,0,0,0},	// Rule 56 - epsilon rule
  	{6,-32,28,-27,30,27,-31,-6}, // Rule 57
  	{-27,10,-21,3,0,0,0,0},	// Rule 58
  	{-27,30,-26,-21,-1,0,0,0}, // Rule 59
  	{30,-26,-21,0,0,0,0,0},	// Rule 60
  	{-27,30,-9,0,0,0,0,0}	// Rule 61

  };

 // Definition of stack used for parsing
 TStack* parseStack;
 TToken* token;

 void expr()
 {
	token=get_token();
 }

 void parse_error()
 {
 	fprintf(stderr,"Parse error, stack dump:\n");
 	// TODO: Print stack dump here

	exit_error(E_SYNTAX);
 }

 void parse()
 {
     int initialState = 1;
     parseStack = stack_init();
     stack_push(parseStack, &initialState);
     token = get_token();

     while(token->type != TOKEN_EOF)
     {
     	int* sTop = stack_top(parseStack);
     	if(*sTop == 30)
	{
		printf("Calling expr!\n");
     		expr();
		continue;
	}
     	if(*sTop < 0)
     	{
     	    	printf("Expecting token: %d", -(*sTop)-1);
     	    	printf("\tReceived token: %d\n", token->type);
     		if (token->type != (-(*sTop) - 1))
     			parse_error();

    		stack_pop(parseStack);
    		token = get_token();
		printf("Token processed\n");
     	}
     	else
     	{
     		int rule = LLtab[*sTop][token->type + 1];
     		printf("Resolved rule: %d\n", rule);
		if(rule == 0)
     			parse_error();

		stack_pop(parseStack);

     		for(int i = 0;i < MAX_PUSHED_STATES; i++)
     		{
     			int value = rules[rule][i];
     			int* valToStack = gmalloc(sizeof(int));
     			*valToStack = value;
     			if (value == 0)
     				break;

     			stack_push(parseStack, valToStack);
     		}
     	}

     }

     printf("Syntaxe OK\n");

 }
