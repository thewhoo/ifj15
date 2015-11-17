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
    else if(output->var_type == TYPE_DOUBLE)
        printf("%g", output->data.d);
    else if(output->var_type == TYPE_STRING)
        printf("%s", output->data.str);
    else
        fprintf(stderr, "Cout: No idea about data type!\n");
}

enum state{ START, S_INT, S_DOT, S_DBL, S_EXPO_E, S_EXPO, S_EXPO_M, };

void cin(TVariable* in)
{
    int state = START;
    int reading = 1;

    int c;
    int int_num;
    double double_num;
    TString buffer;

    initString(&buffer, STR_SIZE);
    
    in->initialized = 1;

    if(in->var_type == TYPE_STRING)
    {
        do{
            c = getchar();      
        } while(isspace(c));

        if(c == EOF)
        {
            insertIntoString(&buffer, 0);
            in->data.str = buffer.data;
            return; 
        }

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
        while(reading)
        {
            c = getchar();
            switch(state)
            {
                case(START):
                    if (isspace(c))
                        break;
                    else if(isdigit(c))
                    {
                        insertIntoString(&buffer, c);
                        state = S_INT;
                    }
                    else            
                        exit_error(E_READ_NUMBER);
                break;
                case(S_INT):
                    if (isdigit(c))
                        insertIntoString(&buffer, c);
                    else if(c == '.')
                    {
                        insertIntoString(&buffer, '.');
                        state = S_DOT;
                    }
                    else if(c == 'E' || c == 'e')
                    {
                        insertIntoString(&buffer, c);
                        state = S_EXPO_E;
                    }
                    else
                    {
                        ungetc(c, stdin);
                        insertIntoString(&buffer, 0);
                        reading = 0;
                    }
                    break;
                case(S_DOT):
                    if (isdigit(c))
                    {
                        insertIntoString(&buffer, c);
                        state = S_DBL;
                    }
                    else
                        exit_error(E_READ_NUMBER);
                    break;
                case S_DBL: 
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
                        ungetc(c, stdin);
                        reading = 0;
                    }
                    break;
                case S_EXPO_E:
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
                        exit_error(E_READ_NUMBER);
                    break;			
                case S_EXPO_M:
                    if (isdigit(c))
                    {
                        insertIntoString(&buffer, c);
                        state= S_EXPO;
                    }		
                    else
                        exit_error(E_READ_NUMBER);
                    break;	
                case S_EXPO:
                    if (isdigit(c))
                        insertIntoString(&buffer, c);
                    else
                    {
                        insertIntoString(&buffer, 0);
                        ungetc(c, stdin);
                        reading = 0;
                    }
                    break;
            }

        }

        double_num = strtod(buffer.data, NULL);
        in->data.d = double_num;
    }
    else if(in->var_type == TYPE_INT)
    {
        do{
            c = getchar();      
        } while(isspace(c));
        
        if((c == EOF) || (!isdigit(c)))
            exit_error(E_READ_NUMBER);

        while(isdigit(c))
        {
            insertIntoString(&buffer, c);
            c = getchar();
        }
        ungetc(c, stdin);
            
        insertIntoString(&buffer, 0);

        int_num = strtol(buffer.data, NULL, 10);       
        in->data.i = int_num;
    }
    else
    {
        fprintf(stderr, "Cin: No idea about data type!\n");
        exit_error(99);
    }
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
