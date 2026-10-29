#include <stdio.h>
#include <time.h>

#include "stack.hpp"

#define ONE_LINE_BUFFER

#include "strhandler.hpp"
#include "config.hpp"

const int VRAM = 64;
const int width = 17; //TODO - move in struct processor
const int hight = 8;

enum drawMode{
    consoleBin = 1,
    consoleChar = 2,
    graphicsRaylib = 3
};

struct processor{
    stack_t *stackFrame;
    stack_t *labelRecursion;

    int *code;
    int codeLen;
    int IP;

    int Registers[5];

    int RAM[VRAM + width * hight]; //    64 + 17x8 покачто
};



void ProcessorConstructor(processor *APM);
void ProcessorDestructor(processor *APM);


void LoadFile(processor *APM, const char *fileName);
void CreateCode(processor *APM, strInfo *programInfo);
void GetCodeLen(processor *APM, strInfo *programInfo);
void CreateCodeList(processor *APM, strInfo *programInfo);

void Processing(processor *APM);
void DoInstruction(processor *APM);

void cmdIN(processor *APM);
void cmdOUT(processor *APM);
void cmdPUSH(processor *APM);
void cmdPOP(processor *APM);
void cmdPUSHR(processor *APM);
void cmdPOPR(processor *APM);
void cmdPUSHM(processor *APM);
void cmdPOPM(processor *APM);
void cmdPUSHRM(processor *APM);
void cmdPOPRM(processor *APM);
void cmdADD(processor *APM);
void cmdSUB(processor *APM);
void cmdMUL(processor *APM);
void cmdDIV(processor *APM);
void cmdPOW(processor *APM);
void cmdJMP(processor *APM);
void cmdJA(processor *APM);
void cmdJAE(processor *APM);
void cmdJB(processor *APM);
void cmdJBE(processor *APM);
void cmdJE(processor *APM);
void cmdJNE(processor *APM);
void cmdJSUN(processor *APM);
void cmdDRAW(processor *APM);
void cmdCALL(processor *APM);
void cmdRET(processor *APM);
void cmdFACT(processor *APM);
void cmdFIB(processor *APM);

void _cmdDrawConsoleBin(processor *APM);
void _cmdDrawConsoleChar(processor *APM);

int _cmdFactorial(int num);
int _cmdFibonacci(int num);



//ASK - str labels?

//ASK - temp var?



//TODO - refactor translator
//TODO - str labels
//TODO - do binary
//TODO - refactor all


int main(){

    processor APM = {};

    ProcessorConstructor(&APM);

    LoadFile(&APM, "code.txt");

DEBUG(
    for(int i = 0; i < APM->codeLen; i++){
        printf("instruction %d: [%d]\n", i, APM->code[i]);
    }
)

    Processing(&APM);

    ProcessorDestructor(&APM);

    return 0;
}


void LoadFile(processor *APM, const char *fileName){

    strInfo env = {};

    //env = (strInfo *)calloc(1, sizeof(strInfo));

    env.fileName = fileName;

    OpenInFile(env);

    ReadInFile(env);

    CreateCode(APM, env);

    free(env);
}

void ProcessorConstructor(processor *APM){    

    APM->IP = 0;

    APM->codeLen = 0;

    APM->code = NULL;

    APM->stackFrame = StackConstructor(globalMinCapacity, "stackFrame");

    APM->labelRecursion = StackConstructor(globalMinCapacity, "labelRecursion");
}

void CreateCode(processor *APM, strInfo *programInfo){

    GetCodeLen(APM, programInfo);    

    CreateCodeList(APM, programInfo);

}

void GetCodeLen(processor *APM, strInfo *programInfo){
    int temp = 0;
    int skip = 0;
    int index = 0;
    char* ptr = programInfo->buffer;

    while(sscanf(ptr, "%d %n", &temp, &skip)){

        index++;

        ptr += skip;
    }

    APM->codeLen = index;

}

void CreateCodeList(processor *APM, strInfo *programInfo){

    APM->code = (int *)calloc(APM->codeLen, sizeof(int));

    int temp = 0;
    int skip = 0;
    int index = 0;
    char* ptr = programInfo->buffer;

    while(sscanf(ptr, "%d %n", &temp, &skip)){

        APM->code[index] = temp;
        index++;

        ptr += skip;
    }
}

void Processing(processor *APM){

    while(APM->code[APM->IP]){
        DoInstruction(APM);
    }

}

void DoInstruction(processor *APM){
    switch (APM->code[APM->IP]){
        case ERR:
            abort();

        case HLT:
            abort();

        case IN:
            cmdIN(APM);
            return;

        case OUT:
            cmdOUT(APM);
            return;

        case PUSH:
            cmdPUSH(APM);
            return;

        case POP:
            cmdPOP(APM);
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

void cmdIN(processor *APM){
    int in = 0;
    if(scanf("%d", &in) == 1){
        StackPush(APM->stackFrame, &in);
        APM->IP++;
    }
    else{
        printf("Invalid input\n");
        abort();
    }
}

void cmdOUT(processor *APM){   
    int out = 0;
    StackPop(APM->stackFrame, &out);

    printf("<<%d>>\n", out);

    APM->IP++;
}

void cmdPUSH(processor *APM){
    StackPush(APM->stackFrame, &(APM->code[APM->IP + 1]));

    APM->IP += 2;
}

void cmdPOP(processor *APM){
    int *pop = (int *)malloc(sizeof(int));
    StackPop(APM->stackFrame, pop);

    APM->IP++;

    free(pop);
}

void cmdPUSHR(processor *APM){

    StackPush(APM->stackFrame, &(APM->Registers[APM->code[APM->IP + 1]]));

    APM->IP += 2;
}

void cmdPOPR(processor *APM){

    StackPop(APM->stackFrame, &(APM->Registers[APM->code[APM->IP + 1]]));

    APM->IP += 2;
}

void cmdPUSHM(processor *APM){

    StackPush(APM->stackFrame, &(APM->RAM[APM->code[APM->IP + 1]]));

    APM->IP += 2;
}

void cmdPOPM(processor *APM){

    StackPop(APM->stackFrame, &(APM->RAM[APM->code[APM->IP + 1]]));

    APM->IP += 2;
}

void cmdPUSHRM(processor *APM){

    StackPush(APM->stackFrame, &(APM->RAM[APM->Registers[APM->code[APM->IP + 1]]]));

    APM->IP += 2;
}

void cmdPOPRM(processor *APM){

    StackPop(APM->stackFrame, &(APM->RAM[APM->Registers[APM->code[APM->IP + 1]]]));

    APM->IP += 2;
}

void cmdADD(processor *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    int add = elem1 + elem2;
    StackPush(APM->stackFrame, &add);

    APM->IP++;
}

void cmdSUB(processor *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    int sub = elem1 - elem2;
    StackPush(APM->stackFrame, &sub);

    APM->IP++;
}

void cmdMUL(processor *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    int mul = elem1 * elem2;
    StackPush(APM->stackFrame, &mul);

    APM->IP++;
}

void cmdDIV(processor *APM){
    int elem2 = 0;
    StackPop(APM->stackFrame, &elem2);

    int elem1 = 0;
    StackPop(APM->stackFrame, &elem1);

    int div = elem1 / elem2;
    StackPush(APM->stackFrame, &div);

    APM->IP++;
}

void cmdPOW(processor *APM){
    int elem = 0;
    StackPop(APM->stackFrame, &elem);

    int res = 1;

    for(int i = 0; i < APM->code[APM->IP + 1]; i++){
        res *= elem;
    }

    StackPush(APM->stackFrame, &res);

    APM->IP += 2;
}

void cmdJMP(processor *APM){
    APM->IP = APM->code[APM->IP + 1];
}

void cmdJA(processor *APM){
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

void cmdJAE(processor *APM){
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

void cmdJB(processor *APM){
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

void cmdJBE(processor *APM){
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

void cmdJE(processor *APM){
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

void cmdJNE(processor *APM){
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

void cmdJSUN(processor *APM){
    
    time_t varTime = time(NULL);

    tm *locTime = localtime(&varTime);
    
    if(locTime->tm_wday == 0){
        APM->IP = APM->code[APM->IP + 1];
    }
    else{
        APM->IP += 2;
    }
}
void cmdDRAW(processor *APM){
    if(APM->code[APM->IP + 1] == consoleBin){
        _cmdDrawConsoleBin(APM);
    }

    APM->IP += 2;
}

void cmdCALL(processor *APM){

    int newIP = APM->IP + 2;

    StackPush(APM->labelRecursion, &newIP);
    
    cmdJMP(APM);
}

void cmdRET(processor *APM){

    StackPop(APM->labelRecursion, &(APM->IP));
}

void cmdFACT(processor *APM){
    int elem = 0;
    StackPop(APM->stackFrame, &elem);

    int fact = _cmdFactorial(elem);
    StackPush(APM->stackFrame, &fact);

    APM->IP++;

}

void cmdFIB(processor *APM){
    int elem = 0;
    StackPop(APM->stackFrame, &elem);

    int fib = _cmdFibonacci(elem);
    StackPush(APM->stackFrame, &fib);

    APM->IP++;
}

void _cmdDrawConsoleBin(processor *APM){
    for(int  i = 0; i < hight; i++){
        for(int j = 0; j < width; j++){
            if(APM->RAM[VRAM + width * i + j] == 0){            
                printf(".");
            }
            else{
                printf("#");
            }
        }
        printf("\n");
    }
}

void _cmdDrawConsoleChar(processor *APM){
    for(int  i = 0; i < hight; i++){
        for(int j = 0; j < width; j++){
            printf("%c", APM->RAM[VRAM + width * i + j]);
        }
        printf("\n");
    }
}

int _cmdFactorial(int num){
    if(num == 1){
        return 1;
    }
    else{
        return num * _cmdFactorial(num - 1);
    }
}

int _cmdFibonacci(int num){
    if(num == 1){
        return 1;
    }
    if(num == 2){
        return 1;
    }
    else{
        return _cmdFibonacci(num - 1) + _cmdFibonacci(num - 2);
    }
}

void ProcessorDestructor(processor *APM){

    StackDestructor(APM->stackFrame);

    StackDestructor(APM->labelRecursion);
}