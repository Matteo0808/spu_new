#ifndef CONFIG_H
#define CONFIG_H

typedef int elem_t;

const int global_signatureLength = 4;
const char global_signature[4] = {'M', 'M', 'A', '\0'};

enum returns{
    RETURN_SUCCESS = 0,
    RETURN_ERROR = 1,
};

enum commands{
    cmdError = -1,
    HLT = 0,
    IN,
    OUT,
    PUSH,
    POP,
    PUSHR,
    POPR,
    PUSHM,
    POPM,
    PUSHRM,
    POPRM,    
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    SQRT,
    JMP,
    JA,
    JAE,
    JB,
    JBE,
    JE,
    JNE,
    JSUN,
    DRAW,
    CALL,
    RET,
    FACT,
    FIB
};

enum registers{
    argError = -1,
    R_AX = 0,
    R_BX = 1,
    R_CX = 2,
    R_DX = 3
};

#endif