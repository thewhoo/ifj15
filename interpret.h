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
    INS_ASSIGN,     //dest, src
    INS_ADD,        //dest, src1, src2
    INS_SUB,        //dest, src1, src2
    INS_MUL,        //dest, src1, src2
    INS_DIV,        //dest, src1, src2
    INS_EQ,         //dest, src1, src2
    INS_NEQ,        //dest, src1, src2
    INS_GREATER,    //dest, src1, src2
    INS_GREATEQ,    //dest, src1, src2
    INS_LESSER,     //dest, src1, src2
    INS_LESSEQ,     //dest, src1, src2
    INS_JMP,        //dest
    INS_CJMP,       //
    INS_LAB,        //
    INS_PUSH,       //src1
    INS_CALL,       //
    INS_RET,        //dest
    INS_PUSH_TAB,   //htab_t*
    INS_POP_TAB,    //
    //built-int functions
    INS_LENGTH,     //dest, src1
    INS_SUBSTR,
    INS_CONCAT,     //dest, src1, src2
    INS_FIND,       //dest, src1, src2
    INS_SORT,       //dest, src1
    INS_CIN,        //dest
    INS_COUT        //src1
}; 

/**
  * @brief 3 address code interpretation
  *
  */
void interpret();

#endif
