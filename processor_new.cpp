#include <stdio.h>
#include <time.h>

#include "stack.hpp"
#include "strhandler.hpp"
#include "config.hpp"

const int VRAM = 64;
const int width = 80;
const int hight = 32;

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

    int Registers[5];

    int RAM[VRAM + width * hight];
};




void cmdIN(processor_t *APM);
void cmdOUT(processor_t *APM);
void cmdPUSH(processor_t *APM);
void cmdPOP(processor_t *APM);
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

void ProcessorConstructor(processor_t *APM){

    APM->stackFrame = StackConstructor(globalMinCapacity, "stackFrame");

    APM->labelRecursion = StackConstructor(globalMinCapacity, "labelRecursion");
}

void LoadFile(processor_t *APM, const char *fileInName){

    strInfo_t env = StrInfoConstructor(fileInName, "out.txt");

    OpenFile(&env);

    CreateCode(APM, &env);
    
    StrInfoDestructor(&env);
}


void Processing(processor_t *APM){

    while(APM->code[APM->IP]){
        DoInstruction(APM);
    }
}

void ProcessorDestructor(processor_t *APM){
    StackDestructor(APM->stackFrame);

    StackDestructor(APM->labelRecursion);
}

void CreateCode(processor_t *APM, strInfo_t *env){
    GetCodeLen(APM, env);    

    CreateCodeList(APM, env);
}

void GetCodeLen(processor_t *APM, strInfo_t *env){
    int temp = 0;
    int skip = 0;
    int index = 0;
    char* ptr = env->buffer;

    while(sscanf(ptr, "%d %n", &temp, &skip)){

        index++;

        ptr += skip;
    }

    APM->codeLen = index;

}

void CreateCodeList(processor_t *APM, strInfo_t *env){

    APM->code = (int *)calloc(APM->codeLen, sizeof(int));

    int temp = 0;
    int skip = 0;
    int index = 0;
    char* ptr = env->buffer;

    while(sscanf(ptr, "%d %n", &temp, &skip)){

        APM->code[index] = temp;
        index++;

        ptr += skip;
    }
}

void DoInstruction(processor_t *APM){
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

void cmdIN(processor_t *APM){
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

/*
void cmdPOP(processor_t *APM){
    int *pop = (int *)malloc(sizeof(int));
    StackPop(APM->stackFrame, pop);

    APM->IP++;

    free(pop);
}
*/

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

void _cmdDrawConsoleChar(processor_t *APM){
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