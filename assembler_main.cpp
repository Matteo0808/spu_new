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

const int labelSize = 16;
const int global_cmdNumber = 29 - 6;

enum number_of_args{
    NOT_A_CMD = -1,
    NO_ARGS = 0,
    ONE_ARG = 1
};

struct label_t{
    const char *labelName; //TODO - 
    int number;
    ssize_t hash;
};

struct assembler_t{
    int *byteCode;
    int byteCodeLen;
    int IP;

    label_t *labels;
    int labelSize;
    int LP;
};

struct instruction_t{
    commands cmd;
    int arg;
};

struct command_t{
    const char *cmdName;
    commands cmdNumber;
    ssize_t cmdHash;
};



void AssemblerConstructor(assembler_t *assembler, command_t **cmds, int lines, int labelSize);
void DoubleAssembling(assembler_t *assembler, strInfo_t *env, command_t *cmds);
void Assembling(assembler_t *assembler, strInfo_t *env, command_t *cmds);
void AssemblerDestructor(assembler_t *assembler);

void CreateBinary(assembler_t *assembler);

command_t *CreateCmdsStructure();
void HashCount(command_t *cmd);
ssize_t HashDJB(const char *str);

number_of_args IsCommand(assembler_t *assembler, instruction_t *instruction, command_t *cmds, const char *str);
void GetArgument(assembler_t *assembler, instruction_t *instruction, const char *str);
bool IsLabel(assembler_t *assembler, const char *str);
int GetLabel(assembler_t *assembler, const char *str);
void CreateLabel(assembler_t *assembler, const char *str);
void GetMemoryArg(instruction_t *instruction, const char *str);
int GetRegister(const char *str);
commands GetCommand(command_t *cmds, const char *str);
commands CheckCmdWithoutArgument(commands cmd);
commands CheckCmdWithArgument(commands cmd);
commands CheckLabelCommands(commands cmd);
commands CheckMemoryCommands(commands cmd);



//TODO - remake binary; COMPLETED
//TODO - hash !!
//TODO - comparison in loop to avoid switch !!
//TODO - binary !!


int main(int argc, char *argv[]){

    if(argc < 2){
        assert(0);
    }

    ASSEMBLER_DEBUG(printf("debug0\n");)

    strInfo_t env = TextProcessingAsm(argv[1]);

    ASSEMBLER_DEBUG(printf("debug1\n");)

    assembler_t assembler = {};
    
    command_t *cmds = NULL;    

    ASSEMBLER_DEBUG(printf("debug2\n");)

    AssemblerConstructor(&assembler, &cmds, env.arrLinesSize, labelSize);

    ASSEMBLER_DEBUG(printf("debug3\n");)

    DoubleAssembling(&assembler, &env, cmds);

    ASSEMBLER_DEBUG(printf("debug4\n");)

    AssemblerDestructor(&assembler);

    ASSEMBLER_DEBUG(printf("debug5\n");)

    StrInfoDestructor(&env);
    
    ASSEMBLER_DEBUG(printf("debug6\n");)
}



void AssemblerConstructor(assembler_t *assembler, command_t **cmds, int lines, int labelSize){
    assert(assembler != NULL);
    assert(lines != 0);
    assert(labelSize != 0);

    assembler->byteCodeLen = lines * 2;
    assembler->byteCode = (int *)calloc(assembler->byteCodeLen, sizeof(int));
    if(assembler->byteCode == NULL){
        assert(0);
    }
    
    assembler->labelSize = labelSize;
    assembler->labels = (label_t *)calloc(assembler->labelSize, sizeof(label_t));
    if(assembler->labels == NULL){
        assert(0);
    }

    for(int index = 0; index < labelSize; index++){
        assembler->labels[index].labelName = "";
        assembler->labels[index].number = -1;
        assembler->labels[index].hash = 0;
    }

    *cmds = CreateCmdsStructure();
    HashCount(*cmds);
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

    for(int index = 0; index < global_cmdNumber; index++){

        ssize_t hui = HashDJB(cmds[index].cmdName);


        ASSEMBLER_DEBUG(printf("hash index = %d\n", index);)
        ASSEMBLER_DEBUG(printf("hash[%d] = %lld\n", index, hui);)

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

void DoubleAssembling(assembler_t *assembler, strInfo_t *env, command_t *cmds){
    assert(assembler != NULL);
    assert(env != NULL);

    Assembling(assembler, env, cmds);

    Assembling(assembler, env, cmds);

ASSEMBLER_DEBUG(
    printf("byteCodeLen = %d\n", assembler->byteCodeLen);
    for (int i = 0; i < assembler->byteCodeLen; i++){
        printf("%d ", assembler->byteCode[i]);
    }
    printf("\n");
)

    CreateBinary(assembler);
}

void CreateBinary(assembler_t *assembler){
    FILE *fileOut = fopen("code.bin", "wb");
    if(fileOut == NULL){
        assert(0);
    }

    if(fwrite(assembler->byteCode, sizeof(int), assembler->byteCodeLen, fileOut) <= 0){
        assert(0);
    }
    
    fclose(fileOut);
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

void Assembling(assembler_t *assembler, strInfo_t *env, command_t *cmds){
    assert(assembler != NULL);
    assert(env != NULL);

    assembler->IP = 0;

    instruction_t instruction = {cmdError, argError};

    for(int index = 0; index < env->arrLinesSize; index++){

        ASSEMBLER_DEBUG(printf("\nin asm: i = [%d]\n", index);)

        number_of_args checkCmd = IsCommand(assembler, &instruction, cmds, env->arrLines[index].line);

        ASSEMBLER_DEBUG(printf("check cmd = [%d]\n", checkCmd);)
        ASSEMBLER_DEBUG(printf("cmd = [%d]\n", (int)instruction.cmd);)

        if(checkCmd == ONE_ARG){
            ASSEMBLER_DEBUG(printf("case 1 arg:\n");)

            GetArgument(assembler, &instruction, env->arrLines[index + 1].line);

            ASSEMBLER_DEBUG(printf("cmd = <%d>\n", (int)instruction.cmd);)
            ASSEMBLER_DEBUG(printf("arg = <%d>\n", (int)instruction.arg);)

            assembler->byteCode[assembler->IP    ] = (int)instruction.cmd;
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

        if(IsLabel(assembler, env->arrLines[index].line)){
            ASSEMBLER_DEBUG(printf("case label:\n");)
            ASSEMBLER_DEBUG(printf("in case label str = [%s]\n", env->arrLines[index].line);)

            CreateLabel(assembler, env->arrLines[index].line);

            continue;
        }

        else{
            ASSEMBLER_DEBUG(printf("case poshel nahui:\n");)
            assert(0);
        }
    }
}

number_of_args IsCommand(assembler_t *assembler, instruction_t *instruction, command_t *cmds, const char *str){
    assert(assembler != NULL);
    assert(instruction != NULL);
    assert(str != NULL);

    instruction->cmd = GetCommand(cmds, str);

    if(instruction->cmd != cmdError){
        if(CheckCmdWithoutArgument(instruction->cmd) != cmdError){
            return NO_ARGS;
        }
        if(CheckCmdWithArgument(instruction->cmd) != cmdError){
            return ONE_ARG;
        }
        else{
            return NOT_A_CMD;
        }
    }
    else{
        return NOT_A_CMD;
    }
}

void GetArgument(assembler_t *assembler, instruction_t *instruction, const char *str){
    assert(assembler != NULL);
    assert(instruction != NULL);
    assert(str != NULL);
    
    int tempInt = 0;
    char tempStr[16] = "";

    ASSEMBLER_DEBUG(printf("str in get arg = [%s]\n", str);)

    if(sscanf(str, "%d", &tempInt) == 1){
        ASSEMBLER_DEBUG(printf("case number:\n");)
        instruction->arg = tempInt;
        return;
    }

    if(str[0] == ':'){
        ASSEMBLER_DEBUG(printf("case label arg:\n");)
        if(CheckLabelCommands(instruction->cmd) != cmdError){
            instruction->arg = GetLabel(assembler, str + 1);
            ASSEMBLER_DEBUG(printf("arg = [%d]\n", instruction->arg);)
            return;
        }
        else{
            assert(0);
        }
    }

    if(str[0] == '['){
        ASSEMBLER_DEBUG(printf("case memory arg:\n");)
        if(CheckMemoryCommands(instruction->cmd) != cmdError){
            GetMemoryArg(instruction, str + 1);
        }
        return;
    }

    if(sscanf(str, "%s", tempStr) != 1){
        assert(0);
    }

    instruction->arg = GetRegister(tempStr);

    if(instruction->arg != argError){
        if(instruction->cmd == PUSH){
            instruction->cmd = PUSHR;
        }
        if(instruction->cmd == POP){
            instruction->cmd = POPR;
        }
        ASSEMBLER_DEBUG(printf("case reg arg:\n");)
        return;
    }
    else{
        assert(0);
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

void CreateLabel(assembler_t *assembler, const char *str){
    assert(assembler != NULL);
    assert(str != NULL);

    if(assembler->LP >= assembler->labelSize){
        assert(0);
    }

    ASSEMBLER_DEBUG(printf("in createLabel() hash = [%d] for str[%s]\n", HashDJB(str + 1), str + 1);)

    if(GetLabel(assembler, str + 1) == -1){
        assembler->labels[assembler->LP].labelName = str + 1;
        assembler->labels[assembler->LP].number = assembler->IP;
        assembler->labels[assembler->LP].hash = HashDJB(str + 1);
        assembler->LP++;
        ASSEMBLER_DEBUG(printf("HAVE CREATED A NEW LABEL))))\n");)
    }
}

int GetLabel(assembler_t *assembler, const char *str){
    assert(assembler != NULL);
    assert(str != NULL);

    ASSEMBLER_DEBUG(printf("in getLabel() hash = [%d] for str[%s]\n", HashDJB(str), str);)

    for(int index = 0; index < assembler->labelSize; index++){
        if(HashDJB(str) == assembler->labels[index].hash){
            if(strcmp(str, assembler->labels[index].labelName) == 0){

                ASSEMBLER_DEBUG(printf("number = %d\n", assembler->labels[index].number);)
                return assembler->labels[index].number;
            }
        }
    }
    return -1;
}

void GetMemoryArg(instruction_t *instruction, const char *str){
    assert(instruction != NULL);
    assert(str != NULL);

    char tempInt = 0;
    char tempChr = 0;

    int check = sscanf(str, "%d]%c", &tempInt, &tempChr);
    if(check == 2){
        assert(0);
    }
    else if(check == 1){
        instruction->arg = tempInt;

        ASSEMBLER_DEBUG(printf("arg mem = %d\n", instruction->arg);)
        ASSEMBLER_DEBUG(printf("cmd mem = %d\n", instruction->cmd);)

        if(instruction->cmd == PUSH){
            instruction->cmd = PUSHM;
            return;
        }
        if(instruction->cmd == POP){
            instruction->cmd = POPM;
            return;
        }
        else{
            assert(0);
        }
    }
    else{
        char tempStr[16] = "";
        sscanf(str, "%[^]]", tempStr);

        instruction->arg = GetRegister(tempStr);

        if(instruction->arg == argError){
            assert(0);
        }

        if(instruction->cmd == PUSH){
            instruction->cmd = PUSHRM;
            return;
        }
        if(instruction->cmd == POP){
            instruction->cmd = POPRM;
            return;
        }
        else{
            assert(0);
        }
    }
}

int GetRegister(const char *str){
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

    for(int index = 0; index < global_cmdNumber; index++){
        if(cmds[index].cmdHash == HashDJB(str)){
            if(strcmp(str, cmds[index].cmdName) == 0){
                return cmds[index].cmdNumber;
            }
        }
    }

    return cmdError;    
}

commands CheckCmdWithoutArgument(commands cmd){
    switch(cmd){
        case HLT:
            return HLT;
        case IN:
            return IN;
        case OUT:
            return OUT;
        case ADD:
            return ADD;
        case SUB:
            return SUB;
        case MUL:
            return MUL;
        case DIV:
            return DIV;
        case RET:
            return RET;
        case FACT:
            return FACT;
        case FIB:
            return FIB;
        default:
            return cmdError;
    }
}

commands CheckCmdWithArgument(commands cmd){
    switch(cmd){
        case PUSH:
            return PUSH;
        case POP:
            return POP;
        case POW:
            return POW;
        case JMP:
            return JMP;
        case JA:
            return JA;
        case JAE:
            return JAE;
        case JB:
            return JB;
        case JBE:
            return JBE;
        case JE:
            return JE;
        case JNE:
            return JNE;
        case JSUN:
            return JSUN;
        case DRAW:
            return DRAW;
        case CALL:
            return CALL;
        default:
            return cmdError;
    }
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
