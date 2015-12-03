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

#define RETURN_VAR_NAME "return"
#define MAIN_FUNC_NAME "main"

TToken* token;

// Forward declarations of state functions
bool PROG();
bool FUNCTION_DECL();
bool DATA_TYPE();
bool FUNC_DECL_PARAMS();
bool FUNC_DECL_PARAMS_NEXT();
bool NESTED_BLOCK(bool);
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
TList_item *createPseudoFrame();
void killPseudoFrame();
void checkFunctionDefinitions();
void pushVar(TVariable*);
void logger();

enum
{
    T_FUNC,
    T_VAR,
    T_BLOCK,
    T_IF
};

void logger(char *c)
{
#ifdef DEBUG_MODE
    fprintf(stdout, "parser: %s\n", c);
#endif // PARSER_DEBUG

    // Surpress unused variable warning
    (void)c;
}

TList_item *createInstruction(int type, void *addr1, void *addr2, void *addr3)
{
    TList_item *ins = gmalloc(sizeof(TList_item));
    ins->ins_type = type;
    ins->addr1 = addr1;
    ins->addr2 = addr2;
    ins->addr3 = addr3;

    return ins;
}

void checkFunctionDefinitions()
{
    TFunction *found;

    for(unsigned int i = 0; i < G.g_globalTab->htab_size; i++)
    {
        for(htab_item *item = G.g_globalTab->list[i]; item != NULL; item = item->next)
        {
            found = item->data.function;

            // All functions must be defined at some point
            if(!found->defined)
                exit_error(E_SEMANTIC_DEF);

            // Main must match the fixed type signature
            if(!strcmp(found->name, MAIN_FUNC_NAME))
            {
                if(!(found->return_type == TYPE_INT && stack_empty(found->params_stack)))
                    exit_error(E_SEMANTIC_DEF);
            }
        }
    }
}
// ============== Control functions start here ==============

void storeFunction(TFunction *f)
{
    // Check if the function has already been declared or defined
    htab_item *result = htab_lookup(G.g_globalTab, f->name);
    if(result)
    {
        // Retrieve found function
        TFunction *found = result->data.function;

        // Check if return type matches
        if(!(found->return_type == f->return_type))
            exit_error(E_SEMANTIC_DEF);

        // Check if data types of parameters match
        for(int i = 0; i < found->params_stack->used; i++)
        {
            TVariable *providedVar = f->params_stack->data[i];
            TVariable *foundVar = found->params_stack->data[i];

            // Param list length mismatch
            if(providedVar == NULL || foundVar == NULL)
                exit_error(E_SEMANTIC_DEF);

            if((providedVar->var_type != foundVar->var_type) || strcmp(providedVar->name, foundVar->name))
                exit_error(E_SEMANTIC_DEF);
        }

        if(found->defined)
        {
            // A redefinition is wrong!
            if(f->defined)
                exit_error(E_SEMANTIC_DEF);
            // A redeclaration after definition is OK, but don't do anything
            else
                return;
        }

        // Replace forward declaration with definition (and update the return var address)
        f->return_var = found->return_var;
        result->data.function = f;
    }
    else
    {
        // Function has not been declared before
        htab_item *newFunc = htab_insert(G.g_globalTab, f->name);
        newFunc->data.function = f;
        newFunc->data_type = TYPE_FUNCTION;
    }

}

// Returns pointer to first instruction in frame (need for if-statements and cycles)
TList_item *createPseudoFrame(int type)
{
    // Get current function
    TFunction *current = stack_top(G.g_frameStack);

    // Create and push a pseudo-function which represents a nested block
    TFunction *f = gmalloc(sizeof(TFunction));
    f->name = NULL;
    f->return_type = TYPE_PSEUDO;
    f->defined = 1;
    f->ins_list = current->ins_list;
    f->local_tab = htab_init(HTAB_SIZE);
    f->params_stack = NULL;
    f->return_var = NULL;
    f->var_count = 0;

    // Push this on the frameStack
    stack_push(G.g_frameStack, f);

    // Create the instruction
    if(type == T_BLOCK)
    {
        TList_item *ins = createInstruction(INS_PUSH_TAB, f, NULL, NULL);
        list_insert(f->ins_list, ins);
        return ins;
    }
    else
    {
        // Conditional jump skeleton
        TList_item *ins1 = createInstruction(INS_CJMP, NULL, NULL, NULL);
        list_insert(f->ins_list, ins1);
        // We still need to push local table
        TList_item *ins2 = createInstruction(INS_PUSH_TAB, f, NULL, NULL);
        list_insert(f->ins_list, ins2);
        return ins1;
    }

    return NULL;
}

void killPseudoFrame()
{
    // Get current function
    TFunction *f = stack_top(G.g_frameStack);

    // Kill the pseudo frame
    TList_item *ins = createInstruction(INS_POP_TAB, NULL, NULL, NULL);
    list_insert(f->ins_list, ins);
}

void storeNewVariable(TFunction *f, TVariable *v)
{
    // Check for redeclaration or clash with function name
    if(htab_lookup(f->local_tab, v->name) || htab_lookup(G.g_globalTab, v->name))
        exit_error(E_SEMANTIC_DEF);

    htab_item *newVar = htab_insert(f->local_tab, v->name);
    newVar->data.variable = v;
    newVar->data_type = TYPE_VARIABLE;

}

void storeNewConstant(TVariable *c)
{
    if(c->var_type == TYPE_STRING)
    {
        if(htab_lookup(G.g_constTabStr, c->name))
            return;

        htab_item *newConst = htab_insert(G.g_constTabStr, c->name);
        newConst->data.variable = c;
        newConst->data_type = TYPE_VARIABLE;

    }
    else
    {
        if(htab_lookup(G.g_constTabNum, c->name))
            return;

        htab_item *newConst = htab_insert(G.g_constTabNum, c->name);
        newConst->data.variable = c;
        newConst->data_type = TYPE_VARIABLE;
    }
}

void pushVar(TVariable *v)
{
    // Grab current ins list
    TFunction *f = stack_top(G.g_frameStack);
    TList_item *ins = createInstruction(INS_PUSH_VAR, v, NULL, NULL);
    list_insert(f->ins_list, ins);
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
    f->return_var = getNewVariable();
    f->return_var->name = RETURN_VAR_NAME;
    f->return_var->constant = true;
    f->var_count = 0;

    return f;
}

TVariable *getNewVariable()
{
    TVariable *v = gmalloc(sizeof(TVariable));

    v->var_type = 0;
    v->name = NULL;
    v->initialized = 0;
    v->constant = false;

    return v;
}

void pushParam(TFunction *f, TVariable *p)
{
    // Param or function with same name already declared, error!
    if(htab_lookup(f->local_tab, p->name) || htab_lookup(G.g_globalTab, p->name) || !strcmp(p->name, f->name))
        exit_error(E_SEMANTIC_DEF);

    stack_push(f->params_stack, p);
    htab_item *param = htab_insert(f->local_tab, p->name);
    p->initialized = true;
    param->data.variable = p;
    param->data_type = TYPE_VARIABLE;
}

void storeFuncName(TFunction *f)
{
    f->name = token->data;
}

void storeVarName(TVariable *v)
{
    v->name = token->data;
}

// ============== End of control functions ===============

// ============== Rule functions start here ==============
bool PROG()
{
    logger("enter PROG");
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
    logger("enter FUNCTION_DECL");

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
    // Builtin function redefinition (this must be handled separately as it is not a syntax error)
    else if(token->type == TOKEN_LENGTH || token->type == TOKEN_SUBSTR || token->type == TOKEN_CONCAT || token->type == TOKEN_FIND || token->type == TOKEN_SORT)
        exit_error(E_SEMANTIC_DEF);
    // Syntax error
    else
        return false;

    if (token->type == TOKEN_LROUND_BRACKET)
        token = get_token();
    // Syntax error
    else
        return false;

    // If all params were processed, store function and process the function block
    if(FUNC_DECL_PARAMS(currentFunc))
    {
        if (token->type == TOKEN_RROUND_BRACKET)
        {
            token = get_token();

            // Function has been declared but not defined
            if(token->type == TOKEN_SEMICOLON)
            {
                currentFunc->defined = false;

                // Store the complete function "object" in the global table
                storeFunction(currentFunc);
                logger("stored function in G.globalTab");

                token = get_token();
            }
            // Process function block if the function is defined
            else
            {
                currentFunc->defined = true;

                // Store the complete function "object" in the global table
                storeFunction(currentFunc);
                logger("stored function in G.globalTab");

                if(!NESTED_BLOCK(true))
                    return false;

                // printf("parser: total variables in function %s: %d\n",currentFunc->name, currentFunc->var_count);
            }
        }
    }

    return true;
}

// Detect and store the data type of the new function/variable
bool DATA_TYPE(void *object, int type)
{
    logger("enter DATA_TYPE");
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

bool FUNC_DECL_PARAMS(TFunction *func)
{
    logger("enter FUNC_DECL_PARAMS");
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

            return true;
        }
        else
            return false;

    }
    // If auto arrives, its a semantic error and must be handled separately
    else if(token->type == TOKEN_AUTO)
        exit_error(E_SEMANTIC_OTHERS);

    // No more params, all good
    else if(token->type == TOKEN_RROUND_BRACKET)
        return true;

    return false;

}

bool FUNC_DECL_PARAMS_NEXT(TFunction *func)
{
    logger("enter FUNC_DECL_PARAMS_NEXT");
    // Next param must start with comma if it exists
    if (token->type == TOKEN_COMMA)
    {
        token = get_token();
        // This is not ideal, but i'm too lazy to rewrite the grammar...
        if(token->type == TOKEN_RROUND_BRACKET)
            return false;

        return FUNC_DECL_PARAMS(func);
    }

    else if (token->type == TOKEN_RROUND_BRACKET)
        return true;

    else
        return false;
}

bool NESTED_BLOCK(bool removeBlockAtEnd)
{
    logger("enter NESTED_BLOCK");
    TFunction *func = stack_top(G.g_frameStack);

    if (token->type == TOKEN_LCURLY_BRACKET)
    {
        token = get_token();
        if(NBC() && token->type == TOKEN_RCURLY_BRACKET)
        {
            // Loops don't want us to kill their blocks
            if(removeBlockAtEnd)
            {
                // If this is not a function but just a block, kill the pseudo frame
                if(func->return_type == TYPE_PSEUDO)
                    killPseudoFrame();

                // Pop current function from frame stack
                stack_pop(G.g_frameStack);
            }

            token = get_token();
            return true;
        }
    }

    return false;
}

bool NBC()
{
    logger("enter NBC");
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
        createPseudoFrame(T_BLOCK);
        return NESTED_BLOCK(true) && NBC();

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
    logger("enter DECL_OR_ASSIGN");
    // Create new variable "object"
    TVariable *var = getNewVariable();
    pushVar(var);
    // The variable belongs to the function or block on top of the frame stack
    TFunction *func = stack_top(G.g_frameStack);
    // Increment function var count
    (func->var_count)++;

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

        var->var_type = TYPE_AUTO;

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
            expression(var, func->ins_list);
        }
        // Auto derivation error
        else
            exit_error(E_AUTO_TYPE);

        token = get_token();

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
    logger("enter DECL_ASSIGN");
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
    logger("enter ASSIGN");
    // Current function
    TFunction *func = stack_top(G.g_frameStack);

    if(token->type == TOKEN_IDENTIFIER)
    {
        // Retrieve the variable which we will be assigning to
        TVariable *var = findVariable(token->data);
        // Cannot assign to an undefined variable
        if(var == NULL)
            exit_error(E_SEMANTIC_DEF);

        token = get_token();

        if(token->type == TOKEN_ASSIGN)
        {
            // Use a custom list if it is specified
            expression(var, func->ins_list);

            token = get_token();
            if(token->type == TOKEN_SEMICOLON)
            {
                token = get_token();
                return true;
            }
        }
        // Syntax error
        else
            return false;

    }

    // Syntax error, unexpected token
    return false;
}

bool HARD_VALUE(TVariable **v)
{
    logger("enter HARD_VALUE");
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
            var->name = token->data;
            var->data.i = strtod(token->data, NULL);
            break;
        case TOKEN_DOUBLE_VALUE:
            var->var_type = TYPE_DOUBLE;
            var->name = token->data;
            var->data.d = strtod(token->data, NULL);
            break;
        case TOKEN_STRING_VALUE:
            var->var_type = TYPE_STRING;
            var->name = token->data;
            var->data.str = var->name;
            break;
        }
        var->constant = true;
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
    logger("enter IF_STATEMENT");

    // Current function
    TFunction *func = stack_top(G.g_frameStack);
    // We need to store the result of the evaluated expression
    TVariable *var = getNewVariable();
    var->initialized = true;
    var->constant = true;
    var->name = "";

    if(token->type == TOKEN_IF)
    {
        token = get_token();
        if(token->type == TOKEN_LROUND_BRACKET)
        {
            // Evaluate condition
            expression(var, func->ins_list);
            token = get_token();

            if(token->type == TOKEN_RROUND_BRACKET)
                token = get_token();
            else
                return false;

            // Create the "then" block pseudo frame and get ptr to first instruction
            TList_item *condIns = createPseudoFrame(T_IF);
            condIns->addr1 = var;

            // Create the instruction for else block skip
            TList_item *skipElse = createInstruction(INS_JMP, NULL, NULL, NULL);

            if(NESTED_BLOCK(true))
            {
                // Insert else skip instruction
                list_insert(func->ins_list, skipElse);

                // Create "false" label after if block ends
                TList_item *ifEnd = createInstruction(INS_LAB, NULL, NULL, NULL);
                list_insert(func->ins_list, ifEnd);

                // Ammend initial conditional jump address to ifEnd
                condIns->addr2 = ifEnd;

                if(ELSE_STATEMENT(skipElse))
                    return true;
            }

        }
    }

    // Syntax error
    return false;
}

bool ELSE_STATEMENT(TList_item *skipIns)
{
    logger("enter ELSE_STATEMENT");

    TFunction *func = stack_top(G.g_frameStack);

    if(token->type == TOKEN_ELSE)
    {
        // Create pseudo frame for else block
        createPseudoFrame(T_BLOCK);
        token = get_token();
        if(NESTED_BLOCK(true))
        {
            // Create skip label after block ends
            TList_item *elseEnd = createInstruction(INS_LAB, NULL, NULL, NULL);
            list_insert(func->ins_list, elseEnd);

            // Amend if true jump to skip else block
            skipIns->addr1 = elseEnd;

            return true;
        }
    }
    /*
    We might implement this later as SIMPLE extension

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
    */

    return false;
}

bool COUT()
{
    logger("enter COUT");
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
    logger("enter COUT_OUTPUT");
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
        TVariable *con = getNewVariable();

        if(!HARD_VALUE(&con))
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
    logger("enter COUT_NEXT");
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
    logger("enter CIN");
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
                if(CIN_NEXT())
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
    logger("enter CIN_NEXT");
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
            if(CIN_NEXT())
                return true;
        }
    }
    else if(token->type == TOKEN_SEMICOLON)
        return true;

    // Syntax error
    return false;
}

bool FOR_STATEMENT()
{
    logger("enter FOR_STATEMENT");

    if(token->type == TOKEN_FOR)
    {
        // Create basic pseudo frame
        createPseudoFrame(T_BLOCK);
        TFunction *frame = stack_top(G.g_frameStack);

        // Create the variable to which will store eval result
        TVariable *expr = getNewVariable();
        expr->initialized = true;
        expr->constant = true;
        expr->name = "";

        token = get_token();

        if(!(token->type == TOKEN_LROUND_BRACKET))
            return false;

        token = get_token();

        // Process assign, append to nested block ilist
        if(!DECL_OR_ASSIGN())
            return false;

        // Create another pseudo frame for the for loop body
        createPseudoFrame(T_BLOCK);
        frame = stack_top(G.g_frameStack);

        // Create loop label
        TList_item *loopLabel = createInstruction(INS_LAB, NULL, NULL, NULL);
        list_insert(frame->ins_list, loopLabel);

        // Add instruction for expression evaluation under loop label
        unget_token(token);
        expression(expr, frame->ins_list);

        // Add conditional jump under evaluation
        TList_item *condJump = createInstruction(INS_CJMP, expr, NULL, NULL);
        list_insert(frame->ins_list, condJump);

        token = get_token();

        if(!(token->type == TOKEN_SEMICOLON))
            return false;

        token = get_token();

        // Create temp list for assign instruction
        Tins_list *tempList = list_init();

        // Create and store the assign instruction
        if(token->type == TOKEN_IDENTIFIER)
        {
            // Retrieve the variable which we will be assigning to
            TVariable *var = findVariable(token->data);
            // Cannot assign to an undefined variable
            if(var == NULL)
                exit_error(E_SEMANTIC_DEF);

            token = get_token();

            if(token->type == TOKEN_ASSIGN)
            {
                // Place assign instruction in temporary list
                expression(var, tempList);

                token = get_token();
            }
            // Syntax error
            else
                return false;

        }

        if(!(token->type == TOKEN_RROUND_BRACKET))
            return false;

        token = get_token();

        // Process nested block without killing frame at the end
        if(!NESTED_BLOCK(false))
            return false;

        // Append assignment to end of loop block
        TList_item *tmp;
        do
        {
            tmp = tempList->first;
            tempList->first = tempList->first->next;
            list_insert(frame->ins_list, tmp);
        }
        while(tempList->first);

        // Add hard jump to loop label
        TList_item *jump = createInstruction(INS_JMP, loopLabel, NULL, NULL);
        list_insert(frame->ins_list, jump);

        // Add loop end label, amend conditional jump to point here
        TList_item *loopEnd = createInstruction(INS_LAB, NULL, NULL, NULL);
        list_insert(frame->ins_list, loopEnd);
        condJump->addr2 = loopEnd;

        // Clean up
        killPseudoFrame();
        stack_pop(G.g_frameStack);
        killPseudoFrame();
        stack_pop(G.g_frameStack);

        return true;

    }


    return false;
}

bool RETURN()
{
    logger("enter RETURN");
    // Currently processed function
    TFunction *func = stack_top(G.g_frameStack);
    TFunction *frame = func;

    if(token->type == TOKEN_RETURN)
    {
        // If the function return var is undefined, we are in a pseudo frame and need to place it in the parent function
        int fsOffset = 2;
        while(!(func->return_var))
        {
            func = G.g_frameStack->data[G.g_frameStack->used - fsOffset];
            fsOffset++;
        }

        func->return_var->var_type = func->return_type;
        expression(func->return_var, frame->ins_list);

        token = get_token();

        // Generate return instructions
        TList_item *ins = createInstruction(INS_RET, NULL, NULL, NULL);
        list_insert(frame->ins_list, ins);

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
    logger("--START--");
    token = get_token();

    if(!PROG())
        exit_error(E_SYNTAX);

    checkFunctionDefinitions();
    logger("--END--");

}
