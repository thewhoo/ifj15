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

/**
  * @file interpret.h
  * @brief Interpret for 3-address code.
  *
  */

#ifndef INTERPRET_H
#define INTERPRET_H

enum instructions
{
    INS_ASSIGN,     //0     dest, src
    INS_ADD,        //1     dest, src1, src2
    INS_SUB,        //2     dest, src1, src2
    INS_MUL,        //3     dest, src1, src2
    INS_DIV,        //4     dest, src1, src2
    INS_EQ,         //5     dest, src1, src2
    INS_NEQ,        //6     dest, src1, src2
    INS_GREATER,    //7     dest, src1, src2
    INS_GREATEQ,    //8     dest, src1, src2
    INS_LESSER,     //9     dest, src1, src2
    INS_LESSEQ,     //10    dest, src1, src2
    INS_JMP,        //11    dest
    INS_CJMP,       //12    TVar, label
    INS_LAB,        //13
    INS_PUSH,       //14    src1
    INS_CALL,       //15
    INS_RET,        //16    dest
    INS_PUSH_TAB,   //17    htab_t*
    INS_POP_TAB,    //18
    INS_PUSH_VAR,   //19    src1
    //built-int functions
    INS_LENGTH,     //20    dest, src1
    INS_SUBSTR,     //21
    INS_CONCAT,     //22    dest, src1, src2
    INS_FIND,       //23    dest, src1, src2
    INS_SORT,       //24    dest, src1
    INS_CIN,        //25    dest
    INS_COUT        //26    src1
}; 

/**
  * @brief 3 address code interpretation
  *
  */
void interpret();

#endif
