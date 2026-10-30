#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "config.hpp"
#include "stack.hpp"
#include "strhandler.hpp"



const int labelSize = 16;

enum number_of_args{
    NOT_A_CMD = -1,
    NO_ARGS = 0,
    ONE_ARG = 1
};

struct label_t{
    const char *labelName;
    int number;
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
    int cmdHash;
};



void AssemblerConstructor(assembler_t *assembler, int lines, int labelSize);
void DoubleAssembling(assembler_t *assembler, strInfo_t *env);
void Assembling(assembler_t *assembler, strInfo_t *env);
void AssemblerDestructor(assembler_t *assembler);

void CreateCode(assembler_t *assembler);//TODO - remake binary;

number_of_args IsCommand(assembler_t *assembler, instruction_t *instruction, const char *str);
void GetArgument(assembler_t *assembler, instruction_t *instruction, const char *str);
bool IsLabel(assembler_t *assembler, const char *str);
int GetLabel(assembler_t *assembler, const char *str);
void CreateLabel(assembler_t *assembler, const char *str);
void GetMemoryArg(instruction_t *instruction, const char *str);
int GetRegister(const char *str);
commands GetCommand(const char *str);
commands CheckCmdWithoutArgument(commands cmd);
commands CheckCmdWithArgument(commands cmd);
commands CheckLabelCommands(commands cmd);
commands CheckMemoryCommands(commands cmd);

int main(int argc, char *argv[]){

    if(argc < 2){
        assert(0);
    }

    printf("debug0\n");

    strInfo_t env = TextProcessingAsm(argv[1]);

    printf("debug1\n");

    assembler_t assembler = {};

    printf("debug2\n");

    AssemblerConstructor(&assembler, env.arrLinesSize, labelSize);

    printf("debug3\n");

    DoubleAssembling(&assembler, &env); //TODO - createcodefunc in

    printf("debug4\n");

    AssemblerDestructor(&assembler);

    printf("debug5\n");

    StrInfoDestructor(&env);
    
    printf("debug6\n");
}



void AssemblerConstructor(assembler_t *assembler, int lines, int labelSize){
    assert(assembler != NULL);
    assert(lines != 0);
    assert(labelSize != 0);

    assembler->byteCode = (int *)calloc(lines * 2, sizeof(int));

    assembler->labelSize = labelSize;

    assembler->labels = (label_t *)calloc(assembler->labelSize, sizeof(label_t));

    memset(assembler->labels, -1, labelSize);
}

void DoubleAssembling(assembler_t *assembler, strInfo_t *env){
    assert(assembler != NULL);
    assert(env != NULL);

    printf("asm1\n");

    assembler->byteCodeLen = env->arrLinesSize;

    assembler->byteCode = (int *)calloc(assembler->byteCodeLen, sizeof(int));

    Assembling(assembler, env);

    printf("asm2\n");

    Assembling(assembler, env);

    printf("asm3\n");

    printf("byteCodeLen = %d\n", assembler->byteCodeLen);

    for (int i = 0; i < assembler->byteCodeLen; i++){
        printf("%d ", assembler->byteCode[i]);
    }

    CreateCode(assembler);

    printf("asm4\n");
}

void CreateCode(assembler_t *assembler){
    FILE *fileOut = fopen("code.txt", "w");

    for(int index = 0; index < assembler->byteCodeLen; index++){
        fprintf(fileOut, "%d ", assembler->byteCode[index]);
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

void Assembling(assembler_t *assembler, strInfo_t *env){
    assert(assembler != NULL);
    assert(env != NULL);

    assembler->IP = 0;

    instruction_t instruction = {cmdError, argError};

    for(assembler->IP = 0; assembler->IP < env->arrLinesSize; assembler->IP++){

        printf("in asm: i = [%d]\n", assembler->IP);

        number_of_args checkCmd = IsCommand(assembler, &instruction, env->arrLines[assembler->IP].line);

        printf("check cmd = [%d]\n", checkCmd);
        printf("cmd = [%d]\n", (int)instruction.cmd);

        if(checkCmd == ONE_ARG){
            printf("case 1 arg:\n");

            GetArgument(assembler, &instruction, env->arrLines[assembler->IP + 1].line);

            assembler->byteCode[assembler->IP    ] = (int)instruction.cmd;
            assembler->byteCode[assembler->IP + 1] = (int)instruction.arg;
            assembler->IP++;

            continue;
        }
        else if(checkCmd == NO_ARGS){
            printf("case 0 arg:\n");

            assembler->byteCode[assembler->IP] = (int)instruction.cmd;
            
            continue;
        }

        if(IsLabel(assembler, env->arrLines[assembler->IP].line)){
            printf("case label:\n");

            CreateLabel(assembler, env->arrLines[assembler->IP].line);

            continue;
        }

        else{
            printf("case poshel nahui:\n");
            assert(0);
        }
    }
}

number_of_args IsCommand(assembler_t *assembler, instruction_t *instruction, const char *str){
    assert(assembler != NULL);
    assert(instruction != NULL);
    assert(str != NULL);

    instruction->cmd = GetCommand(str);

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

    printf("str in get arg = [%s]\n", str);

    if(sscanf(str, "%d", &tempInt) == 1){
        printf("case alpha:\n");
        instruction->arg = tempInt;
        return;
    }

    if(str[0] == ':'){
        printf("case label arg:\n");
        if(CheckLabelCommands(instruction->cmd) != cmdError){
            instruction->arg = GetLabel(assembler, str + 1);
        }
        return;
    }

    if(str[0] == '['){
        printf("case memory arg:\n");
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
        printf("case reg arg:\n");
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

    assembler->labels[assembler->LP].labelName = str;
    assembler->LP++;
}

int GetLabel(assembler_t *assembler, const char *str){
    assert(assembler != NULL);
    assert(str != NULL);

    for(int index = 0; index < assembler->labelSize; index++){
        if(index = assembler->labelSize - 1){
            if(strcmp(str, assembler->labels[index].labelName) != 0){
                return -1;
            }
        }
        if(strcmp(str, assembler->labels[index].labelName) != 0){
            continue;
        }
        return assembler->labels[index].number;
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

        if(instruction->cmd == PUSH){
            instruction->cmd == PUSHM;
        }
        if(instruction->cmd == POP){
            instruction->cmd == POPM;
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
            instruction->cmd == PUSHRM;
        }
        if(instruction->cmd == POP){
                instruction->cmd == POPRM;
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
    if(strcmp(str, "SX") == 0){
        return R_SX;
    }
    else{
        return argError;
    }
}

commands GetCommand(const char *str){
    assert(str != NULL);

    if(strcmp(str, "HLT") == 0){
        return HLT;
    }

    if (strcmp(str, "IN") == 0){
        return IN;
    }

    if(strcmp(str, "OUT") == 0){
        return OUT;
    }

    if (strcmp(str, "PUSH") == 0){
        return PUSH;
    }

    if (strcmp(str, "POP") == 0){
        return POP;
    }

    if(strcmp(str, "ADD") == 0){
        return ADD;
    }

    if(strcmp(str, "SUB") == 0){
        return SUB;
    }

    if(strcmp(str, "MUL") == 0){
        return MUL;
    }

    if(strcmp(str, "DIV") == 0){
        return DIV;
    }

    if (strcmp(str, "POW") == 0){
        return POW;
    }
    
    if(strcmp(str, "JMP") == 0){
        return JMP;
    }
    
    if(strcmp(str, "JA") == 0){
        return JA;
    }
    
    if(strcmp(str, "JAE") == 0){
        return JAE;
    }
    
    if(strcmp(str, "JB") == 0){
        return JB;
    }
    
    if(strcmp(str, "JBE") == 0){
        return JBE;
    }
    
    if(strcmp(str, "JE") == 0){
        return JE;
    }
    
    if(strcmp(str, "JNE") == 0){
        return JNE;
    }

    if(strcmp(str, "JSUN") == 0){
        return JSUN;
    }
    
    if (strcmp(str, "DRAW") == 0){
        return DRAW;
    }

    if(strcmp(str, "CALL") == 0){
        return CALL;
    }

    if(strcmp(str, "RET") == 0){
        return RET;
    }

    if(strcmp(str, "FACT") == 0){
        return FACT;
    }

    if(strcmp(str, "FIB") == 0){
        return FIB;
    }

    else{
        return cmdError;
    }
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
        case RET:
            return RET;
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
