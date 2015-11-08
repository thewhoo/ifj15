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

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "error.h"
#include "adt.h"
#include "galloc.h"
#include "string.h"
#include "lex.h"
#include "enums.h"

#define STR_SIZE 20

int length(TVariable *str)
{
    int size = 0;

    if(str->var_type != TYPE_STRING)
        exit_error(E_SEMANTIC_TYPES);

    size = strlen(str->data.str);
    return size;
}

char* concat(TVariable* str1, TVariable* str2)
{
    if(str1->var_type != TYPE_STRING || str2->var_type != TYPE_STRING)
        exit_error(E_SEMANTIC_TYPES);

    char* s1 = str1->data.str;
    char* s2 = str2->data.str;
    char* new_str = gmalloc(strlen(s1) + strlen(s2) + 1);
    
    strcpy(new_str, s1);
    strcat(new_str, s2);

    return new_str;
}

void cout(TVariable* output)
{
    if(output->var_type == TYPE_INT)
        printf("%d", output->data.i);
    else if(output->var_type == TYPE_INT)
        printf("%g", output->data.d);
    else if(output->var_type == TYPE_STRING)
        printf("%s", output->data.str);
    else
        fprintf(stderr, "Cout: No idea about data type!\n");
}

void cin(TVariable* in)
{
    int c;
    TString buffer;
    TToken *token;

    int int_number = 0;
    double double_number = 0;

    initString(&buffer, STR_SIZE);

    if(in->var_type == TYPE_STRING)
    {
        c = getchar();      
        while(isspace(c))
            c = getchar();
        if(c == EOF)
        {
            insertIntoString(&buffer, 0);
            in->data.str = buffer.data;
            return; 
        }

        insertIntoString(&buffer, c);
        c = getchar();      
        while(!isspace(c))
        {
            insertIntoString(&buffer, c);
            c = getchar();
        }
        ungetc(c, stdin);
            
        insertIntoString(&buffer, 0);
        in->data.str = buffer.data;
    }
    else if(in->var_type == TYPE_DOUBLE)
    {
        lex_init(stdin);
        token = get_token();
        if((token->type != TOKEN_DOUBLE_VALUE) && (token->type != TOKEN_INT_VALUE))
            exit_error(E_READ_NUMBER);
        
        double_number = atof(token->data);
        in->data.d = double_number;
        gfree(token->data);
        gfree(token);
    }
    else if(in->var_type == TYPE_INT)
    {
        lex_init(stdin);
        token = get_token();
        if(token->type != TOKEN_INT_VALUE)
            exit_error(E_READ_NUMBER);
        
        int_number = atoi(token->data);
        in->data.i = int_number;
        gfree(token->data);
        gfree(token);
    }
    else
    {
        fprintf(stderr, "Cin: No idea about data type!\n");
        exit_error(99);
    }

    in->initialized = 1;
}


char* substr(char *s, int i, int n)
{
    int subslen;
    char* subs;
    int subs_i = 0;
    int slen = strlen(s);     

    if(i < 0 || i > slen)
        exit_error(E_RUNTIME_OTHERS);

    if(i == slen)
    {
        subs = gmalloc(1);   
        subs[0] = 0;
        return subs;
    }
    
    if(n < 0)
        n = slen;

    if((i + n) > slen)
        subslen = slen - i + 1;
    else
        subslen = n + 1;
    subs = gmalloc(subslen);
    
    while((i < slen) && (subs_i < n))
    {
        subs[subs_i] = s[i];
        subs_i++;
        i++;
    }
    subs[subs_i] = 0;

    return subs;
}
