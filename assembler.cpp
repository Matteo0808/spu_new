#include <stdio.h>
#include <string.h>
#include <assert.h>

#define ASSEMBLER_DEBUG_MODE

#ifdef ASSEMBLER_DEBUG_MODE
    #define STRHANDLER_DEBUG_MODE
    #define ASSEMBLER_DEBUG(...) __VA_ARGS__
#else
    #define ASSEMBLER_DEBUG(...)
#endif

#include "config.hpp"
#include "strhandler.hpp"

const size_t global_labelSize = 16;
const size_t global_cmdNumber = 30 - 6;

enum assembler_errorList{
    ASSEMBLER_NO_ERROR = 0,
    ASSEMBLER_ERROR_CONSOLE_ARGUMENT,
    ASSEMBLER_ERROR_TEXT_PROCESSING,
    ASSEMBLER_ERROR_BYTECODE_MEMORY_ALLOCATION,
    ASSEMBLER_ERROR_LABEL_MEMORY_ALLOCATION,
    ASSEMBLER_ERROR_INVALID_LABEL_COMMAND,
    ASSEMBLER_ERROR_INVALID_MEMORY_COMMAND,
    ASSEMBLER_ERROR_INVALID_COMMAND,
    ASSEMBLER_ERROR_TO_MANY_LABELS,
    ASSEMBLER_ERROR_INVALID_INSTRUCTION,
    ASSEMBLER_ERROR_INVALID_MEMORY_ARGUMENT,
    ASSEMBLER_ERROR_BINARY_OPEN,
    ASSEMBLER_ERROR_BINARY_FWRITE_SIGNATURE,
    ASSEMBLER_ERROR_BINARY_FWRITE
};

enum number_of_args{
    NOT_A_CMD = -1,
    NO_ARGS = 0,
    ONE_ARG = 1
};

struct label_t{
    const char *labelName; //TODO - 
    ssize_t number;
    ssize_t hash;
};

struct assembler_t{
    int *byteCode;
    size_t byteCodeLen;
    size_t IP;

    label_t *labels;
    size_t labelSize;
    size_t LP;
};

struct instruction_t{
    commands cmd;
    ssize_t arg;
};

struct command_t{
    const char *cmdName;
    commands cmdNumber;
    ssize_t cmdHash;
};


assembler_errorList CheckConsoleArgument(int argc);
assembler_errorList AssemblerConstructor(assembler_t *assembler, command_t **cmds, size_t lines, size_t labelSize);
assembler_errorList DoubleAssembling(assembler_t *assembler, strInfo_t *env, command_t *cmds);
assembler_errorList Assembling(assembler_t *assembler, strInfo_t *env, command_t *cmds);
void AssemblerDestructor(assembler_t *assembler);

assembler_errorList CreateBinary(assembler_t *assembler);

command_t *CreateCmdsStructure();
void HashCount(command_t *cmd);
ssize_t HashDJB(const char *str);

number_of_args IsCommand(assembler_t *assembler, instruction_t *instruction, command_t *cmds, const char *str);
assembler_errorList GetArgument(assembler_t *assembler, instruction_t *instruction, const char *str);
bool IsLabel(assembler_t *assembler, const char *str);
ssize_t GetLabel(assembler_t *assembler, const char *str);
assembler_errorList CreateLabel(assembler_t *assembler, const char *str);
assembler_errorList GetMemoryArg(instruction_t *instruction, const char *str);
ssize_t GetRegister(const char *str);
commands GetCommand(command_t *cmds, const char *str);
commands CheckLabelCommands(commands cmd);
commands CheckMemoryCommands(commands cmd);

assembler_errorList TextProcessingAsmWrap(strInfo_t *env, const char *fileInName);
void AssemblerErrorProcessing(assembler_errorList err, assembler_t *assembler, strInfo_t *env);



//TODO - remake binary; COMPLETED
//TODO - hash !!; COMPLETED
//TODO - comparison in loop to avoid switch !!; COMPLETED
//TODO - binary !!; COMPLETED
//TODO - func ptrs array
//TODO - combine proc funcs in macros

int main(int argc, char *argv[]){

    strInfo_t env = {};
    assembler_t assembler = {};    
    command_t *cmds = NULL;
    assembler_errorList err = ASSEMBLER_NO_ERROR;

    err = CheckConsoleArgument(argc);
    if(err){
        AssemblerErrorProcessing(err, &assembler, &env);
        return 1;
    }    

    err = TextProcessingAsmWrap(&env, argv[1]);
    if(err){
        AssemblerErrorProcessing(err, &assembler, &env);
        return 1;
    }

    err = AssemblerConstructor(&assembler, &cmds, env.arrLinesSize, global_labelSize);
    if(err){
        AssemblerErrorProcessing(err, &assembler, &env);
        return 1;
    }

    err = DoubleAssembling(&assembler, &env, cmds);
    if(err){
        AssemblerErrorProcessing(err, &assembler, &env);
        return 1;
    }

    AssemblerDestructor(&assembler);

    StrInfoDestructor(&env);
}

assembler_errorList CheckConsoleArgument(int argc){
    if(argc < 2){
        return ASSEMBLER_ERROR_CONSOLE_ARGUMENT;
    }

    return ASSEMBLER_NO_ERROR;
}

assembler_errorList AssemblerConstructor(assembler_t *assembler, command_t **cmds, size_t lines, size_t labelSize){
    assert(assembler != NULL);
    assert(cmds != NULL);
    assert(lines != 0);
    assert(labelSize != 0);

    assembler->byteCodeLen = lines * 2;
    assembler->byteCode = (int *)calloc(assembler->byteCodeLen, sizeof(int));
    
    if(assembler->byteCode == NULL){
        return ASSEMBLER_ERROR_BYTECODE_MEMORY_ALLOCATION;
    }
    
    assembler->labelSize = labelSize;
    assembler->labels = (label_t *)calloc(assembler->labelSize, sizeof(label_t));
    if(assembler->labels == NULL){
        return ASSEMBLER_ERROR_LABEL_MEMORY_ALLOCATION;
    }

    for(size_t index = 0; index < labelSize; index++){
        assembler->labels[index].labelName = "";
        assembler->labels[index].number = -1;
        assembler->labels[index].hash = 0;
    }

    *cmds = CreateCmdsStructure();

    HashCount(*cmds);

    return ASSEMBLER_NO_ERROR;
}

command_t *CreateCmdsStructure(){
    static command_t cmds[global_cmdNumber] = {
        {"HLT",    HLT,    0},
        {"IN",     IN,     0},
        {"OUT",    OUT,    0},
        {"PUSH",   PUSH,   0},
        {"POP",    POP,    0},
/*
        {"PUSHR",  PUSHR,  0},
        {"POPR",   POPR,   0},
        {"PUSHM",  PUSHM,  0},
        {"POPM",   POPM,   0},
        {"PUSHRM", PUSHRM, 0},
        {"POPRM",  POPRM,  0},
*/
        {"ADD",    ADD,    0},
        {"SUB",    SUB,    0},
        {"MUL",    MUL,    0},
        {"DIV",    DIV,    0},
        {"POW",    POW,    0},
        {"SQRT",   SQRT,   0},
        {"JMP",    JMP,    0},
        {"JA",     JA,     0},
        {"JAE",    JAE,    0},
        {"JB",     JB,     0},
        {"JBE",    JBE,    0},
        {"JE",     JE,     0},
        {"JNE",    JNE,    0},
        {"JSUN",   JSUN,   0},
        {"DRAW",   DRAW,   0},
        {"CALL",   CALL,   0},
        {"RET",    RET,    0},
        {"FACT",   FACT,   0},
        {"FIB",    FIB,    0},
    };
    return cmds;
}

void HashCount(command_t *cmds){
    assert(cmds != NULL);

    for(size_t index = 0; index < global_cmdNumber; index++){

        ssize_t hui = HashDJB(cmds[index].cmdName);

        cmds[index].cmdHash = hui;
    }
}

ssize_t HashDJB(const char *str){
    assert(str != NULL);
    
    ssize_t hash = 5381;
    char *temp = (char *)str;

    while(*temp){
        hash = hash + (hash << 5) + *(temp);
        temp++;
    }
    
    return hash;
}

assembler_errorList DoubleAssembling(assembler_t *assembler, strInfo_t *env, command_t *cmds){
    assert(assembler != NULL);
    assert(env != NULL);
    assert(cmds != NULL);
        
    assembler_errorList err = ASSEMBLER_NO_ERROR;
    
    err = Assembling(assembler, env, cmds);
    if(err){
        return err;
    }

    err = Assembling(assembler, env, cmds);
    if(err){
        return err;
    }

ASSEMBLER_DEBUG(
    printf("byteCodeLen = %lld\n", assembler->byteCodeLen);
    for (size_t i = 0; i < assembler->byteCodeLen; i++){
        printf("%d ", assembler->byteCode[i]);
    }
    printf("\n");
)

    err = CreateBinary(assembler);
    if(err){
        return err;
    }

    return ASSEMBLER_NO_ERROR;
}

assembler_errorList CreateBinary(assembler_t *assembler){

    FILE *fileOut = fopen("code.bin", "wb");
    if(fileOut == NULL){
        return ASSEMBLER_ERROR_BINARY_OPEN;
    }

     if(fwrite(global_signature, global_signatureLength, 1, fileOut) <= 0){
        fclose(fileOut);
        return ASSEMBLER_ERROR_BINARY_FWRITE_SIGNATURE;
     }

    if(fwrite(assembler->byteCode, sizeof(int), assembler->byteCodeLen, fileOut) <= 0){
        fclose(fileOut);
        return ASSEMBLER_ERROR_BINARY_FWRITE;;
    }
    
    fclose(fileOut);

    return ASSEMBLER_NO_ERROR;
}

void AssemblerDestructor(assembler_t *assembler){
    assert(assembler != NULL);

    assembler->byteCodeLen = 0;
    assembler->IP = 0;
    assembler->labelSize = 0;
    assembler->LP = 0;

    if(assembler->byteCode != NULL){
        free(assembler->byteCode);
    }
    if(assembler->labels != NULL){
        free(assembler->labels);
    }
}

assembler_errorList Assembling(assembler_t *assembler, strInfo_t *env, command_t *cmds){
    assert(assembler != NULL);
    assert(env != NULL);
    assert(cmds != NULL);

    assembler->IP = 0;

    instruction_t instruction = {cmdError, argError};

    assembler_errorList err = ASSEMBLER_NO_ERROR;

    for(size_t index = 0; index < env->arrLinesSize; index++){

        ASSEMBLER_DEBUG(printf("\nin asm: i = [%lld]\n", index);)

        number_of_args checkCmd = IsCommand(assembler, &instruction, cmds, env->arrLines[index].line);

        ASSEMBLER_DEBUG(printf("check cmd = [%d]\n", (int)checkCmd);)
        ASSEMBLER_DEBUG(printf("cmd = [%d]\n", (int)instruction.cmd);)
        if(checkCmd != NOT_A_CMD){
            if(checkCmd == ONE_ARG){
                ASSEMBLER_DEBUG(printf("case 1 arg:\n");)

                err = GetArgument(assembler, &instruction, env->arrLines[index + 1].line);
                if(err){
                    return err;
                }

                ASSEMBLER_DEBUG(printf("cmd = <%d>\n", (int)instruction.cmd);)
                ASSEMBLER_DEBUG(printf("arg = <%lld>\n", instruction.arg);)

                assembler->byteCode[assembler->IP] = (int)instruction.cmd;
                assembler->byteCode[assembler->IP + 1] = (int)instruction.arg;
                assembler->IP += 2;

                index++;

                continue;
            }
            else if(checkCmd == NO_ARGS){
                ASSEMBLER_DEBUG(printf("case 0 arg:\n");)

                ASSEMBLER_DEBUG(printf("cmd = <%d>\n", (int)instruction.cmd);)

                assembler->byteCode[assembler->IP] = (int)instruction.cmd;
                assembler->IP++;
                
                continue;
            }
        }
        if(IsLabel(assembler, env->arrLines[index].line)){ //TODO - 
            ASSEMBLER_DEBUG(printf("case label:\n");)
            ASSEMBLER_DEBUG(printf("in case label str = [%s]\n", env->arrLines[index].line);)

            err = CreateLabel(assembler, env->arrLines[index].line);
            if(err){
                return err;
            }

            continue;
        }

        else{
            return ASSEMBLER_ERROR_INVALID_INSTRUCTION;
        }
    }

    return ASSEMBLER_NO_ERROR;
}

number_of_args IsCommand(assembler_t *assembler, instruction_t *instruction, command_t *cmds, const char *str){
    assert(assembler != NULL);
    assert(instruction != NULL);
    assert(str != NULL);

    instruction->cmd = GetCommand(cmds, str);

    if(instruction->cmd != cmdError){
        switch(instruction->cmd){
        case PUSH: return ONE_ARG;
        case POP:  return ONE_ARG;
        case POW:  return ONE_ARG;
        case JMP:  return ONE_ARG;
        case JA:   return ONE_ARG;
        case JAE:  return ONE_ARG;
        case JB:   return ONE_ARG;
        case JBE:  return ONE_ARG;
        case JE:   return ONE_ARG;
        case JNE:  return ONE_ARG;
        case JSUN: return ONE_ARG;
        case DRAW: return ONE_ARG;
        case CALL: return ONE_ARG;
        case HLT:  return NO_ARGS;
        case IN:   return NO_ARGS;
        case OUT:  return NO_ARGS;
        case ADD:  return NO_ARGS;
        case SUB:  return NO_ARGS;
        case MUL:  return NO_ARGS;
        case DIV:  return NO_ARGS;
        case RET:  return NO_ARGS;
        case FACT: return NO_ARGS;
        case FIB:  return NO_ARGS;
        case SQRT: return NO_ARGS;
        default: return NOT_A_CMD;
        }
    }
    else{
        return NOT_A_CMD;
    }
}

assembler_errorList GetArgument(assembler_t *assembler, instruction_t *instruction, const char *str){
    assert(assembler != NULL);
    assert(instruction != NULL);
    assert(str != NULL);
    
    int tempInt = 0;

    assembler_errorList err = ASSEMBLER_NO_ERROR;


    ASSEMBLER_DEBUG(printf("str in get arg = [%s]\n", str);)

    if(sscanf(str, "%d", &tempInt) == 1){
        ASSEMBLER_DEBUG(printf("case number:\n");)
        instruction->arg = tempInt;
        return ASSEMBLER_NO_ERROR;
    }
    if(str[0] == ':'){
        ASSEMBLER_DEBUG(printf("case label arg:\n");)
        if(CheckLabelCommands(instruction->cmd) != cmdError){
            instruction->arg = GetLabel(assembler, str + 1);
            ASSEMBLER_DEBUG(printf("arg = [%lld]\n", instruction->arg);)
            return ASSEMBLER_NO_ERROR;
        }
        else{
            return ASSEMBLER_ERROR_INVALID_LABEL_COMMAND;
        }
    }

    if(str[0] == '['){
        ASSEMBLER_DEBUG(printf("case memory arg:\n");)
        if(CheckMemoryCommands(instruction->cmd) != cmdError){
            err = GetMemoryArg(instruction, str + 1);
            if(err){
                return err;
            }
            return ASSEMBLER_NO_ERROR;
        }
        else{
            return ASSEMBLER_ERROR_INVALID_MEMORY_COMMAND;
        }
    }

    instruction->arg = GetRegister(str); //TODO - 

    printf("arg = %lld\n", instruction->arg);

    if(instruction->arg != argError){
        if(instruction->cmd == PUSH){
            instruction->cmd = PUSHR;
        }
        if(instruction->cmd == POP){
            instruction->cmd = POPR;
        }
        ASSEMBLER_DEBUG(printf("case reg arg:\n");)
        return ASSEMBLER_NO_ERROR;
    }
    else{
        return ASSEMBLER_ERROR_INVALID_COMMAND;
    }
}

bool IsLabel(assembler_t *assembler, const char *str){
    assert(assembler != NULL);
    assert(str != NULL);

    if(str[0] == ':'){
        return true;      
    }

    else{
        return false;
    }
}

assembler_errorList CreateLabel(assembler_t *assembler, const char *str){
    assert(assembler != NULL);
    assert(str != NULL);

    if(assembler->LP >= assembler->labelSize){
        return ASSEMBLER_ERROR_TO_MANY_LABELS;
    }

    ASSEMBLER_DEBUG(printf("in createLabel() hash = [%lld] for str[%s]\n", HashDJB(str + 1), str + 1);)

    if(GetLabel(assembler, str + 1) == -1){
        assembler->labels[assembler->LP].labelName = str + 1;
        assembler->labels[assembler->LP].number = assembler->IP;
        assembler->labels[assembler->LP].hash = HashDJB(str + 1);
        assembler->LP++;
        ASSEMBLER_DEBUG(printf("HAVE CREATED A NEW LABEL))))\n");)
    }

    return ASSEMBLER_NO_ERROR;
}

ssize_t GetLabel(assembler_t *assembler, const char *str){
    assert(assembler != NULL);
    assert(str != NULL);

    ASSEMBLER_DEBUG(printf("in getLabel() hash = [%lld] for str[%s]\n", HashDJB(str), str);)

    for(size_t index = 0; index < assembler->labelSize; index++){
        if(HashDJB(str) == assembler->labels[index].hash){
            if(strcmp(str, assembler->labels[index].labelName) == 0){

                ASSEMBLER_DEBUG(printf("number = %lld\n", assembler->labels[index].number);)
                return assembler->labels[index].number;
            }
        }
    }
    return -1;
}

assembler_errorList GetMemoryArg(instruction_t *instruction, const char *str){
    assert(instruction != NULL);
    assert(str != NULL);

    int tempInt = 0;
    char tempChr = 0;

    int check = sscanf(str, "%d]%c", &tempInt, &tempChr);
    if(check == 2){
        return ASSEMBLER_ERROR_INVALID_INSTRUCTION;
    }
    else if(check == 1){
        instruction->arg = tempInt;

        ASSEMBLER_DEBUG(printf("cmd mem = %d\n", instruction->cmd);)
        ASSEMBLER_DEBUG(printf("arg mem = %lld\n", instruction->arg);)

        if(instruction->cmd == PUSH){
            instruction->cmd = PUSHM;
            return ASSEMBLER_NO_ERROR;
        }
        if(instruction->cmd == POP){
            instruction->cmd = POPM;
            return ASSEMBLER_NO_ERROR;
        }
        else{
            return ASSEMBLER_ERROR_INVALID_MEMORY_COMMAND;
        }
    }
    else{
        char tempStr[16] = "";
        sscanf(str, "%[^]]", tempStr);

        instruction->arg = GetRegister(tempStr);

        if(instruction->arg == argError){
            return ASSEMBLER_ERROR_INVALID_MEMORY_ARGUMENT;
        }

        if(instruction->cmd == PUSH){
            instruction->cmd = PUSHRM;
            return ASSEMBLER_NO_ERROR;
        }
        if(instruction->cmd == POP){
            instruction->cmd = POPRM;
            return ASSEMBLER_NO_ERROR;
        }
        else{
            return ASSEMBLER_ERROR_INVALID_MEMORY_COMMAND;
        }
    }
}

ssize_t GetRegister(const char *str){
    assert(str != NULL);

    if(strcmp(str, "AX") == 0){
        return R_AX;
    }
    if(strcmp(str, "BX") == 0){
        return R_BX;
    }
    if(strcmp(str, "CX") == 0){
        return R_CX;
    }
    if(strcmp(str, "DX") == 0){
        return R_DX;
    }
    else{
        return argError;
    }
}

commands GetCommand(command_t *cmds, const char *str){
    assert(str != NULL);

    for(size_t index = 0; index < global_cmdNumber; index++){
        if(cmds[index].cmdHash == HashDJB(str)){
            if(strcmp(str, cmds[index].cmdName) == 0){
                return cmds[index].cmdNumber;
            }
        }
    }

    return cmdError;    
}


commands CheckLabelCommands(commands cmd){
    switch(cmd){
        case JMP:
            return JMP;
        case JA:
            return JA;
        case JAE:
            return JAE;
        case JB:
            return JBE;
        case JBE:
            return JBE;
        case JE:
            return JE;
        case JNE:
            return JNE;
        case JSUN:
            return JSUN;
        case CALL:
            return CALL;
        default:
            return cmdError;
    }
}

commands CheckMemoryCommands(commands cmd){
    switch(cmd){
        case PUSH:
            return PUSH;            
        case POP:
            return POP;
        default:
            return cmdError;
    }
}

assembler_errorList TextProcessingAsmWrap(strInfo_t *env, const char *fileInName){
    if(TextProcessingAsm(env, fileInName)){
        return ASSEMBLER_ERROR_TEXT_PROCESSING;
    }
    return ASSEMBLER_NO_ERROR;
}

void AssemblerErrorProcessing(assembler_errorList err, assembler_t *assembler, strInfo_t *env){
    if(assembler->byteCode != NULL){
        free(assembler->byteCode);
    }
    if(assembler->labels != NULL){
        free(assembler->labels);
    }
    if(env->buffer != NULL){
        free(env->buffer);
    }
    switch(err){
        case ASSEMBLER_ERROR_BINARY_FWRITE:
            fprintf(stderr, "Error in writing in binary file\n");
            return;
        case ASSEMBLER_ERROR_BINARY_FWRITE_SIGNATURE:
            fprintf(stderr, "Error in writing signature in binary file\n");
            return;
        case ASSEMBLER_ERROR_BINARY_OPEN:
            fprintf(stderr, "Error in opening binary file\n");
            return;
        case ASSEMBLER_ERROR_BYTECODE_MEMORY_ALLOCATION:
            fprintf(stderr, "Error in bytecode memory allocation\n");
            return;
        case ASSEMBLER_ERROR_CONSOLE_ARGUMENT:
            fprintf(stderr, "Error in console arguments. Usage: ./program.exe \"fileInName.asm\"\n");
            return;
        case ASSEMBLER_ERROR_INVALID_COMMAND:
            fprintf(stderr, "Invalid command in asm file\n");
            return;
        case ASSEMBLER_ERROR_INVALID_INSTRUCTION:
            fprintf(stderr, "Invalid instruction\n");
            return;
        case ASSEMBLER_ERROR_INVALID_LABEL_COMMAND:
            fprintf(stderr, "Invalid label command\n");
            return;
        case ASSEMBLER_ERROR_INVALID_MEMORY_COMMAND:
            fprintf(stderr, "Invalid memory command\n");
            return;
        case ASSEMBLER_ERROR_INVALID_MEMORY_ARGUMENT:
            fprintf(stderr, "Invalid memory argument\n");
            return;
        case ASSEMBLER_ERROR_LABEL_MEMORY_ALLOCATION:
            fprintf(stderr, "Error in label memory allocation\n");
            return;
        case ASSEMBLER_ERROR_TEXT_PROCESSING:
            fprintf(stderr, "Error in text processing\n");
            return;
        case ASSEMBLER_ERROR_TO_MANY_LABELS:
            fprintf(stderr, "Too many labels\n");
            return;
        default:
            printf("OK\n");
            return;

    }
}