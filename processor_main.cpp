#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

//#define PROCESSOR_DEBUG_MODE

#ifdef PROCESSOR_DEBUG_MODE
    #define PROCESSOR_DEBUG(...) __VA_ARGS__
#else
    #define PROCESSOR_DEBUG(...)
#endif

#include "stack.hpp"
#include "strhandler.hpp"
#include "config.hpp"

const int VRAMptr = 64;

enum drawMode{
    consoleBin = 1,
    consoleChar = 2,
    graphicsRaylib = 3
};

struct processor_t{
    stack_t *stackFrame;
    stack_t *labelRecursion;

    int *code;
    int codeLen;
    int IP;

    int Registers[4];

    int *RAM;
    int VRAMptr;
    int width;
    int hight;
};



void ProcessorConstructor(processor_t *APM, int width, int hight);
void LoadFile(processor_t *APM, const char *fileInName);
int GetCodeLen(const char *fileName);
void Processing(processor_t *APM);
void ProcessorDestructor(processor_t *APM);
void DoInstruction(processor_t *APM);



void cmdIN(processor_t *APM);
void cmdOUT(processor_t *APM);
void cmdPUSH(processor_t *APM);
void cmdPUSHR(processor_t *APM);
void cmdPOPR(processor_t *APM);
void cmdPUSHM(processor_t *APM);
void cmdPOPM(processor_t *APM);
void cmdPUSHRM(processor_t *APM);
void cmdPOPRM(processor_t *APM);
void cmdADD(processor_t *APM);
void cmdSUB(processor_t *APM);
void cmdMUL(processor_t *APM);
void cmdDIV(processor_t *APM);
void cmdPOW(processor_t *APM);
void cmdJMP(processor_t *APM);
void cmdJA(processor_t *APM);
void cmdJAE(processor_t *APM);
void cmdJB(processor_t *APM);
void cmdJBE(processor_t *APM);
void cmdJE(processor_t *APM);
void cmdJNE(processor_t *APM);
void cmdJSUN(processor_t *APM);
void cmdDRAW(processor_t *APM);
void cmdCALL(processor_t *APM);
void cmdRET(processor_t *APM);
void cmdFACT(processor_t *APM);
void cmdFIB(processor_t *APM);

void _cmdDrawConsoleBin(processor_t *APM);
void _cmdDrawConsoleChar(processor_t *APM);

int _cmdFactorial(int num);
int _cmdFibonacci(int num);



int main(){

    processor_t APM = {};

    PROCESSOR_DEBUG(printf("debug1\n");)

    ProcessorConstructor(&APM, 10, 10);

    PROCESSOR_DEBUG(printf("debug2\n");)

    LoadFile(&APM, "code.bin");

    PROCESSOR_DEBUG(printf("debug3\n");)

    Processing(&APM);

    PROCESSOR_DEBUG(printf("debug4\n");)

    ProcessorDestructor(&APM);

    PROCESSOR_DEBUG(printf("debug5\n");)

    return 0;
}

void ProcessorConstructor(processor_t *APM, const int width, const int hight){
    assert(APM != NULL);

    APM->VRAMptr = VRAMptr;
    APM->width = width;
    APM->hight = hight;

    APM->RAM = (int *)calloc(APM->VRAMptr + APM->width * APM->hight, sizeof(int));
    if(APM->RAM == NULL){
        assert(0);
    }

    APM->stackFrame = StackConstructor(globalMinCapacity, "stackFrame");
    APM->labelRecursion = StackConstructor(globalMinCapacity, "labelRecursion");
}

void LoadFile(processor_t *APM, const char *fileInName){
    assert(APM != NULL);
    assert(fileInName != NULL);

    APM->codeLen = GetCodeLen(fileInName);

    APM->code = (int *)calloc(APM->codeLen, sizeof(int));
    if(APM->code == NULL){
        assert(0);
    }

    FILE *binaryFp = fopen(fileInName, "rb");

    fread(APM->code, sizeof(int), APM->codeLen, binaryFp);

    fclose(binaryFp);
}

int GetCodeLen(const char *fileName){
    assert(fileName != NULL);

    struct stat fileStat = {};

    if(stat(fileName, &fileStat) == -1){
        return -1;
    }

    return fileStat.st_size / sizeof(int);
}

void Processing(processor_t *APM){
    assert(APM != NULL);

    while(APM->code[APM->IP]){
        PROCESSOR_DEBUG(
            printf("IP = %d\n", APM->IP);
            printf("Cmd = %d\n", APM->code[APM->IP]);
        )
        DoInstruction(APM);
    }
}

void ProcessorDestructor(processor_t *APM){
    assert(APM != NULL);

    APM->codeLen = 0;
    APM->width = 0;
    APM->hight = 0;
    APM->VRAMptr = 0;
    APM->IP = 0;

    if(APM->code != NULL){
        free(APM->code);
    }
    if(APM->RAM != NULL){
        free(APM->RAM);
    }    

    StackDestructor(APM->stackFrame);
    StackDestructor(APM->labelRecursion);
}

void DoInstruction(processor_t *APM){
    assert(APM != NULL);

    switch (APM->code[APM->IP]){
        case cmdError:
            assert(0);

        case HLT:
            exit(0);

        case IN:
            cmdIN(APM);
            return;

        case OUT:
            cmdOUT(APM);
            return;

        case PUSH:
            cmdPUSH(APM);
            return;

        case PUSHR:
            cmdPUSHR(APM);
            return;

        case POPR:
            cmdPOPR(APM);
            return;

        case PUSHM:
            cmdPUSHM(APM);
            return;

        case POPM:
            cmdPOPM(APM);
            return;

        case PUSHRM:
            cmdPUSHRM(APM);
            return;

        case POPRM:
            cmdPOPRM(APM);
                return;

        case ADD:
            cmdADD(APM);
            return;

        case SUB:
            cmdSUB(APM);
            return;

        case MUL:
            cmdMUL(APM);
            return;

        case DIV:
            cmdDIV(APM);
            return;

        case POW:
            cmdPOW(APM);
            return;
        
        case DRAW:
            cmdDRAW(APM);
            return;

        case JMP:
            cmdJMP(APM);
            return;
        
        case JA:
            cmdJA(APM);
            return;
        
        case JAE:
            cmdJAE(APM);
            return;

        case JB:
            cmdJB(APM);
            return;

        case JBE:
            cmdJBE(APM);
            return;

        case JE:
            cmdJE(APM);
            return;

        case JNE:
            cmdJNE(APM);
            return;

        case JSUN:
            cmdJSUN(APM);
            return;

        case CALL:
            cmdCALL(APM);
            return;

        case RET:
            cmdRET(APM);
            return;

        case FACT:
            cmdFACT(APM);
            return;

        case FIB:
            cmdFIB(APM);
            return;

        default:
            abort();
    }
}

void cmdIN(processor_t *APM){
    int in = 0;
    if(scanf("%d", &in) == 1){
        StackPush(APM->stackFrame, &in);
        APM->IP++;
    }
    else{
        printf("Invalid input\n");
        assert(0);
    }
}

void cmdOUT(processor_t *APM){   
    int out = 0;
    StackPop(APM->stackFrame, &out);

    printf("<<%d>>\n", out);

    APM->IP++;
}

void cmdPUSH(processor_t *APM){
    StackPush(APM->stackFrame, &(APM->code[APM->IP + 1]));

    APM->IP += 2;
}

void cmdPUSHR(processor_t *APM){

    StackPush(APM->stackFrame, &(APM->Registers[APM->code[APM->IP + 1]]));

    APM->IP += 2;
}

void cmdPOPR(processor_t *APM){

    StackPop(APM->stackFrame, &(APM->Registers[APM->code[APM->IP + 1]]));

    APM->IP += 2;
}

void cmdPUSHM(processor_t *APM){

    StackPush(APM->stackFrame, &(APM->RAM[APM->code[APM->IP + 1]]));

    APM->IP += 2;
}

void cmdPOPM(processor_t *APM){

    StackPop(APM->stackFrame, &(APM->RAM[APM->code[APM->IP + 1]]));

    APM->IP += 2;
}

void cmdPUSHRM(processor_t *APM){

    StackPush(APM->stackFrame, &(APM->RAM[APM->Registers[APM->code[APM->IP + 1]]]));

    APM->IP += 2;
}

void cmdPOPRM(processor_t *APM){

    StackPop(APM->stackFrame, &(APM->RAM[APM->Registers[APM->code[APM->IP + 1]]]));

    APM->IP += 2;
}

void cmdADD(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    int add = elem1 + elem2;
    StackPush(APM->stackFrame, &add);

    APM->IP++;
}

void cmdSUB(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    int sub = elem1 - elem2;
    StackPush(APM->stackFrame, &sub);

    APM->IP++;
}

void cmdMUL(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    int mul = elem1 * elem2;
    StackPush(APM->stackFrame, &mul);

    APM->IP++;
}

void cmdDIV(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    int div = elem1 / elem2;
    StackPush(APM->stackFrame, &div);

    APM->IP++;
}

void cmdPOW(processor_t *APM){
    int elem = 0;
    StackPop(APM->stackFrame, &elem);

    int res = 1;

    for(int i = 0; i < APM->code[APM->IP + 1]; i++){
        res *= elem;
    }

    StackPush(APM->stackFrame, &res);

    APM->IP += 2;
}

void cmdJMP(processor_t *APM){
    APM->IP = APM->code[APM->IP + 1];
}

void cmdJA(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    if(elem1 > elem2){
        APM->IP = APM->code[APM->IP + 1];
    }
    else{
        APM->IP += 2;
    }

    StackPush(APM->stackFrame, &elem1);
    StackPush(APM->stackFrame, &elem2);
}

void cmdJAE(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    if(elem1 >= elem2){
        APM->IP = APM->code[APM->IP + 1];
    }
    else{
        APM->IP += 2;
    }

    StackPush(APM->stackFrame, &elem1);
    StackPush(APM->stackFrame, &elem2);
}

void cmdJB(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    if(elem1 < elem2){
        APM->IP = APM->code[APM->IP + 1];
    }
    else{
        APM->IP += 2;
    }

    StackPush(APM->stackFrame, &elem1);
    StackPush(APM->stackFrame, &elem2);
}

void cmdJBE(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    if(elem1 <= elem2){
        APM->IP = APM->code[APM->IP + 1];
    }
    else{
        APM->IP += 2;
    }

    StackPush(APM->stackFrame, &elem1);
    StackPush(APM->stackFrame, &elem2);
}

void cmdJE(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    if(elem1 == elem2){
        APM->IP = APM->code[APM->IP + 1];
    }
    else{
        APM->IP += 2;
    }

    StackPush(APM->stackFrame, &elem1);
    StackPush(APM->stackFrame, &elem2);
}

void cmdJNE(processor_t *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    if(elem1 != elem2){
        APM->IP = APM->code[APM->IP + 1];
    }
    else{
        APM->IP += 2;
    }

    StackPush(APM->stackFrame, &elem1);
    StackPush(APM->stackFrame, &elem2);
}

void cmdJSUN(processor_t *APM){
    
    time_t varTime = time(NULL);

    tm *locTime = localtime(&varTime);
    
    if(locTime->tm_wday == 0){
        APM->IP = APM->code[APM->IP + 1];
    }
    else{
        APM->IP += 2;
    }
}
void cmdDRAW(processor_t *APM){
    if(APM->code[APM->IP + 1] == consoleBin){
        _cmdDrawConsoleBin(APM);
    }

    APM->IP += 2;
}

void cmdCALL(processor_t *APM){

    int newIP = APM->IP + 2;

    StackPush(APM->labelRecursion, &newIP);
    
    cmdJMP(APM);
}

void cmdRET(processor_t *APM){

    StackPop(APM->labelRecursion, &(APM->IP));
}

void cmdFACT(processor_t *APM){
    int elem = 0;
    StackPop(APM->stackFrame, &elem);

    int fact = _cmdFactorial(elem);
    StackPush(APM->stackFrame, &fact);

    APM->IP++;

}

void cmdFIB(processor_t *APM){
    int elem = 0;
    StackPop(APM->stackFrame, &elem);

    int fib = _cmdFibonacci(elem);
    StackPush(APM->stackFrame, &fib);

    APM->IP++;
}

void _cmdDrawConsoleBin(processor_t *APM){
    for(int  i = 0; i < APM->hight; i++){
        for(int j = 0; j < APM->width; j++){
            if(APM->RAM[APM->VRAMptr + APM->width * i + j] == 0){            
                printf(".");
            }
            else{
                printf("#");
            }
        }
        printf("\n");
    }
}

void _cmdDrawConsoleChar(processor_t *APM){
    for(int  i = 0; i < APM->hight; i++){
        for(int j = 0; j < APM->width; j++){
            printf("%c", APM->RAM[APM->VRAMptr + APM->width * i + j]);
        }
        printf("\n");
    }
}

int _cmdFactorial(int num){
    assert(num >= 0);
    if(num == 0){
        return 1;
    }
    if(num == 1){
        return 1;
    }
    else{
        return num * _cmdFactorial(num - 1);
    }
}

int _cmdFibonacci(int num){
    if(num == 0){
        return 0;
    }
    if(num == 1){
        return 1;
    }
    else{
        return _cmdFibonacci(num - 1) + _cmdFibonacci(num - 2);
    }
}