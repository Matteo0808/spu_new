#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <math.h>
#include "C:\raylib\raylib\src\raylib.h"

//#define PROCESSOR_DEBUG_MODE

#ifdef PROCESSOR_DEBUG_MODE
    #define PROCESSOR_DEBUG(...) __VA_ARGS__
#else
    #define PROCESSOR_DEBUG(...)
#endif

#include "config.hpp"
#include "stack.hpp"

const size_t global_VRAMptr = 64;
const size_t global_windowWidth = 1280;
const size_t global_windowHeight = 960;



enum processor_errorList{
    PROCESSOR_NO_ERROR = 0,
    PROCESSOR_ERROR_RAM_ALLOCATION,
    PROCESSOR_ERROR_FILESIZE,
    PROCESSOR_ERROR_CODE_ALLOCATION,
    PROCESSOR_ERROR_BINARY_OPEN,
    PROCESSOR_ERROR_BINARY_FSEEK,
    PROCESSOR_ERROR_BINARY_FREAD,
    PROCESSOR_ERROR_INVALID_COMMAND,
    PROCESSOR_ERROR_INVALID_INPUT,
    PROCESSOR_ERROR_ZERO_DIVISION,
    PROCESSOR_ERROR_INVALID_ARGUMENT
};

enum drawMode{
    consoleBin = 1,
    consoleChar = 2,
    graphicRaylibBW = 3,
    graphicRaylibRGB = 4
};

struct processor_t{
    stack_t *stack;
    stack_t *labelRecursion;

    elem_t *code;
    size_t codeLen;
    int IP;

    int Registers[4];

    elem_t *RAM;
    size_t VRAMptr;
    size_t width;
    size_t height;
};



processor_errorList ProcessorConstructor(processor_t *APM, const size_t width, const size_t height);
processor_errorList LoadFile(processor_t *APM, const char *fileInName);
ssize_t GetCodeLen(const char *fileName);
processor_errorList Processing(processor_t *APM);
processor_errorList DoInstruction(processor_t *APM);
processor_errorList ProcessorDestructor(processor_t *APM);

elem_t GetArgument(processor_t *APM);

processor_errorList cmdIN(processor_t *APM);
processor_errorList cmdOUT(processor_t *APM);
processor_errorList cmdPUSH(processor_t *APM);
processor_errorList cmdPUSHR(processor_t *APM);
processor_errorList cmdPOPR(processor_t *APM);
processor_errorList cmdPUSHM(processor_t *APM);
processor_errorList cmdPOPM(processor_t *APM);
processor_errorList cmdPUSHRM(processor_t *APM);
processor_errorList cmdPOPRM(processor_t *APM);
processor_errorList cmdADD(processor_t *APM);
processor_errorList cmdSUB(processor_t *APM);
processor_errorList cmdMUL(processor_t *APM);
processor_errorList cmdDIV(processor_t *APM);
processor_errorList cmdPOW(processor_t *APM);
processor_errorList cmdSQRT(processor_t *APM);
processor_errorList cmdJMP(processor_t *APM);
processor_errorList cmdJA(processor_t *APM);
processor_errorList cmdJAE(processor_t *APM);
processor_errorList cmdJB(processor_t *APM);
processor_errorList cmdJBE(processor_t *APM);
processor_errorList cmdJE(processor_t *APM);
processor_errorList cmdJNE(processor_t *APM);
processor_errorList cmdJSUN(processor_t *APM);
processor_errorList cmdDRAW(processor_t *APM);
processor_errorList cmdCALL(processor_t *APM);
processor_errorList cmdRET(processor_t *APM);
processor_errorList cmdFACT(processor_t *APM);
processor_errorList cmdFIB(processor_t *APM);

void _cmdDrawConsoleBin(processor_t *APM);
void _cmdDrawConsoleChar(processor_t *APM);
void _cmdDrawGraphicWindowBW(processor_t *APM);
void _cmdDrawGraphicWindowRGB(processor_t *APM);

int _cmdFactorial(int num);
int _cmdFibonacci(int num);

void ProcessorErrorProcessing(processor_errorList err, processor_t *APM);

int main(){

    processor_errorList err = PROCESSOR_NO_ERROR;
    processor_t APM = {};

    err = ProcessorConstructor(&APM, global_windowWidth, global_windowHeight);
    if(err){
        ProcessorErrorProcessing(err, &APM);
        return RETURN_ERROR;
    }

    err = LoadFile(&APM, "code.bin");
    if(err){
        ProcessorErrorProcessing(err, &APM);
        return RETURN_ERROR;
    }

    err = Processing(&APM);
    if(err){
        ProcessorErrorProcessing(err, &APM);
        return RETURN_ERROR;
    }

    ProcessorDestructor(&APM);

    return RETURN_SUCCESS;
}

processor_errorList ProcessorConstructor(processor_t *APM, const size_t width, const size_t height){
    assert(APM != NULL);

    APM->VRAMptr = global_VRAMptr;
    APM->width = width;
    APM->height = height;

    APM->RAM = (elem_t *)calloc(APM->VRAMptr + APM->width * APM->height, sizeof(elem_t));
    if(APM->RAM == NULL){
        return PROCESSOR_ERROR_RAM_ALLOCATION;
    }

    APM->stack = StackConstructor(global_minCapacity, "stack");
    APM->labelRecursion = StackConstructor(global_minCapacity, "labelRecursion");

    return PROCESSOR_NO_ERROR;
}

processor_errorList LoadFile(processor_t *APM, const char *fileInName){
    assert(APM != NULL);
    assert(fileInName != NULL);

    APM->codeLen = GetCodeLen(fileInName);
    if(APM->codeLen == -1){
        return PROCESSOR_ERROR_FILESIZE;
    }

    APM->code = (elem_t *)calloc(APM->codeLen, sizeof(elem_t));
    if(APM->code == NULL){
        return PROCESSOR_ERROR_CODE_ALLOCATION;
    }

    FILE *binaryFp = fopen(fileInName, "rb");
    if(binaryFp == NULL){
        return PROCESSOR_ERROR_BINARY_OPEN;
    }

    if(fseek(binaryFp, global_signatureLength, SEEK_SET) != 0){
        fclose(binaryFp);
        return PROCESSOR_ERROR_BINARY_FSEEK;
    }

    if(fread(APM->code, sizeof(elem_t), APM->codeLen, binaryFp) <= 0){
        fclose(binaryFp);
        return PROCESSOR_ERROR_BINARY_FREAD;
    }

    fclose(binaryFp);

    return PROCESSOR_NO_ERROR;
}

ssize_t GetCodeLen(const char *fileName){
    assert(fileName != NULL);

    struct stat fileStat = {};

    if(stat(fileName, &fileStat) == -1){
        return -1;
    }

    return (fileStat.st_size - global_signatureLength) / sizeof(elem_t);
}

processor_errorList Processing(processor_t *APM){
    assert(APM != NULL);

    processor_errorList err = PROCESSOR_NO_ERROR;

    while(APM->code[APM->IP]){
        PROCESSOR_DEBUG(
            printf("IP = %d\n", APM->IP);
            printf("Cmd = %lld\n", APM->code[APM->IP]);
        )
        err = DoInstruction(APM);
        if(err){
            return err;
        }
    }

    return PROCESSOR_NO_ERROR;
}

processor_errorList ProcessorDestructor(processor_t *APM){
    assert(APM != NULL);

    APM->codeLen = 0;
    APM->width = 0;
    APM->height = 0;
    APM->VRAMptr = 0;
    APM->IP = 0;

    if(APM->code != NULL){
        free(APM->code);
    }
    if(APM->RAM != NULL){
        free(APM->RAM);
    }    

    if(APM->stack->status == INITIALIZED){
        StackDestructor(APM->stack);
    }
    if(APM->labelRecursion->status == INITIALIZED){
        StackDestructor(APM->labelRecursion);
    }

    return PROCESSOR_NO_ERROR;
}

processor_errorList DoInstruction(processor_t *APM){
    assert(APM != NULL);

    switch (APM->code[APM->IP]){
        case HLT: return PROCESSOR_NO_ERROR;
        case IN:
            cmdIN(APM);
            return PROCESSOR_NO_ERROR;
        case OUT:
            cmdOUT(APM);
            return PROCESSOR_NO_ERROR;
        case PUSH:
            cmdPUSH(APM);
            return PROCESSOR_NO_ERROR;
        case PUSHR:
            cmdPUSHR(APM);
            return PROCESSOR_NO_ERROR;
        case POPR:
            cmdPOPR(APM);
            return PROCESSOR_NO_ERROR;
        case PUSHM:
            cmdPUSHM(APM);
            return PROCESSOR_NO_ERROR;
        case POPM:
            cmdPOPM(APM);
            return PROCESSOR_NO_ERROR;
        case PUSHRM:
            cmdPUSHRM(APM);
            return PROCESSOR_NO_ERROR;
        case POPRM:
            cmdPOPRM(APM);
            return PROCESSOR_NO_ERROR;
        case ADD:
            cmdADD(APM);
            return PROCESSOR_NO_ERROR;
        case SUB:
            cmdSUB(APM);
            return PROCESSOR_NO_ERROR;
        case MUL:
            cmdMUL(APM);
            return PROCESSOR_NO_ERROR;
        case DIV:
            cmdDIV(APM);
            return PROCESSOR_NO_ERROR;
        case POW:
            cmdPOW(APM);
            return PROCESSOR_NO_ERROR;
        case SQRT:
            cmdSQRT(APM);
            return PROCESSOR_NO_ERROR;        
        case DRAW:
            cmdDRAW(APM);
            return PROCESSOR_NO_ERROR;
        case JMP:
            cmdJMP(APM);
            return PROCESSOR_NO_ERROR;        
        case JA:
            cmdJA(APM);
            return PROCESSOR_NO_ERROR;        
        case JAE:
            cmdJAE(APM);
            return PROCESSOR_NO_ERROR;
        case JB:
            cmdJB(APM);
            return PROCESSOR_NO_ERROR;
        case JBE:
            cmdJBE(APM);
            return PROCESSOR_NO_ERROR;
        case JE:
            cmdJE(APM);
            return PROCESSOR_NO_ERROR;
        case JNE:
            cmdJNE(APM);
            return PROCESSOR_NO_ERROR;
        case JSUN:
            cmdJSUN(APM);
            return PROCESSOR_NO_ERROR;
        case CALL:
            cmdCALL(APM);
            return PROCESSOR_NO_ERROR;
        case RET:
            cmdRET(APM);
            return PROCESSOR_NO_ERROR;
        case FACT:
            cmdFACT(APM);
            return PROCESSOR_NO_ERROR;
        case FIB:
            cmdFIB(APM);
            return PROCESSOR_NO_ERROR;
        default:
            return PROCESSOR_ERROR_INVALID_COMMAND;
    }
}

int GetArgument(processor_t *APM){
    return APM->code[APM->IP + 1];
}

processor_errorList cmdIN(processor_t *APM){

    elem_t in = 0;
    if(scanf("%d", &in) == 1){    // for typedef int elem_t
        StackPush(APM->stack, in);
        APM->IP++;
        return PROCESSOR_NO_ERROR;
    }
    else{
        return PROCESSOR_ERROR_INVALID_INPUT;
    }
}

processor_errorList cmdOUT(processor_t *APM){   

    elem_t out = 0;
    StackPop(APM->stack, &out);

    printf("<<%d>>\n", out); // for typedef int elem_t

    APM->IP++;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdPUSH(processor_t *APM){

    elem_t arg = GetArgument(APM);
    StackPush(APM->stack, arg);

    APM->IP += 2;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdPUSHR(processor_t *APM){

    elem_t arg = GetArgument(APM);
    StackPush(APM->stack, APM->Registers[arg]);

    APM->IP += 2;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdPOPR(processor_t *APM){

    elem_t arg = GetArgument(APM);
    StackPop(APM->stack, &(APM->Registers[arg]));

    APM->IP += 2;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdPUSHM(processor_t *APM){

    elem_t arg = GetArgument(APM);
    StackPush(APM->stack, APM->RAM[arg]);

    APM->IP += 2;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdPOPM(processor_t *APM){

    elem_t arg = GetArgument(APM);
    StackPop(APM->stack, &(APM->RAM[arg]));

    APM->IP += 2;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdPUSHRM(processor_t *APM){

    elem_t arg = GetArgument(APM);
    StackPush(APM->stack, APM->RAM[APM->Registers[arg]]);

    APM->IP += 2;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdPOPRM(processor_t *APM){

    elem_t arg = GetArgument(APM);
    StackPop(APM->stack, &(APM->RAM[APM->Registers[arg]]));

    APM->IP += 2;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdADD(processor_t *APM){

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    elem_t add = elem1 + elem2;
    StackPush(APM->stack, add);

    APM->IP++;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdSUB(processor_t *APM){

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    elem_t sub = elem1 - elem2;
    StackPush(APM->stack, sub);

    APM->IP++;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdMUL(processor_t *APM){

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    elem_t mul = elem1 * elem2;
    StackPush(APM->stack, mul);

    APM->IP++;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdDIV(processor_t *APM){

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    if(elem2 == 0){
        return PROCESSOR_ERROR_ZERO_DIVISION;
    }

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    elem_t div = elem1 / elem2;
    StackPush(APM->stack, div);

    APM->IP++;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdPOW(processor_t *APM){

    elem_t arg = GetArgument(APM);
    elem_t elem = 0;
    StackPop(APM->stack, &elem);

    elem_t res = 1;

    if(arg == 0){
        StackPush(APM->stack, res);
        APM->IP += 2;
        return PROCESSOR_NO_ERROR;
    }

    if(arg > 0){
        for(size_t i = 0; i < arg; i++){
            res *= elem;
        }
        StackPush(APM->stack, res);
        APM->IP += 2;
        return PROCESSOR_NO_ERROR;
    }
    else{
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }
}

processor_errorList cmdSQRT(processor_t *APM){
    
    elem_t elem = 0;
    StackPop(APM->stack, &elem);

    elem_t res = (elem_t)(sqrt(elem));
    StackPush(APM->stack, res);

    APM->IP++;
    
    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdJMP(processor_t *APM){

    elem_t arg = GetArgument(APM);
    if(arg < 0){
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }  
    APM->IP = arg;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdJA(processor_t *APM){

    elem_t arg = GetArgument(APM);
    if(arg < 0){
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    if(elem1 > elem2){
        APM->IP = arg;
    }
    else{
        APM->IP += 2;
    }
    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdJAE(processor_t *APM){

    elem_t arg = GetArgument(APM);
    if(arg < 0){
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    if(elem1 >= elem2){
        APM->IP = arg;
    }
    else{
        APM->IP += 2;
    }
    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdJB(processor_t *APM){

    elem_t arg = GetArgument(APM);
    if(arg < 0){
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    if(elem1 < elem2){
        APM->IP = arg;
    }
    else{
        APM->IP += 2;
    }
    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdJBE(processor_t *APM){

    elem_t arg = GetArgument(APM);
    if(arg < 0){
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    if(elem1 <= elem2){
        APM->IP = arg;
    }
    else{
        APM->IP += 2;
    }
    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdJE(processor_t *APM){

    elem_t arg = GetArgument(APM);
    if(arg < 0){
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    if(elem1 == elem2){
        APM->IP = arg;
    }
    else{
        APM->IP += 2;
    }
    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdJNE(processor_t *APM){

    elem_t arg = GetArgument(APM);
    if(arg < 0){
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }

    elem_t elem2 = 0;
    StackPop(APM->stack, &elem2);

    elem_t elem1 = 0;
    StackPop(APM->stack, &elem1);

    if(elem1 != elem2){
        APM->IP = arg;
    }
    else{
        APM->IP += 2;
    }
    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdJSUN(processor_t *APM){

    elem_t arg = GetArgument(APM);
    if(arg < 0){
        return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }
    
    time_t varTime = time(NULL);
    tm *locTime = localtime(&varTime);
    
    if(locTime->tm_wday == 0){
        APM->IP = arg;
    }
    else{
        APM->IP += 2;
    }
    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdDRAW(processor_t *APM){

    elem_t arg = GetArgument(APM);

    switch(arg){
        case consoleBin:
            _cmdDrawConsoleBin(APM);
            APM->IP += 2;
            return PROCESSOR_NO_ERROR;
        case consoleChar:
            _cmdDrawConsoleChar(APM);
            APM->IP += 2;
            return PROCESSOR_NO_ERROR;
        case graphicRaylibBW:
            _cmdDrawGraphicWindowBW(APM);
            APM->IP += 2;
            return PROCESSOR_NO_ERROR;
        case graphicRaylibRGB:
            _cmdDrawGraphicWindowRGB(APM);
            APM->IP += 2;
            return PROCESSOR_NO_ERROR;
        default:
            return PROCESSOR_ERROR_INVALID_ARGUMENT;
    }
}

processor_errorList cmdCALL(processor_t *APM){

    int newIP = APM->IP + 2;
    StackPush(APM->labelRecursion, newIP);
    
    processor_errorList err = cmdJMP(APM);
    if(err){
        return err;
    }

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdRET(processor_t *APM){

    StackPop(APM->labelRecursion, &(APM->IP));

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdFACT(processor_t *APM){
    elem_t elem = 0;
    StackPop(APM->stack, &elem);

    elem_t fact = (elem_t)_cmdFactorial(elem);
    StackPush(APM->stack, fact);

    APM->IP++;

    return PROCESSOR_NO_ERROR;
}

processor_errorList cmdFIB(processor_t *APM){
    elem_t elem = 0;
    StackPop(APM->stack, &elem);

    elem_t fib = _cmdFibonacci(elem);
    StackPush(APM->stack, fib);

    APM->IP++;

    return PROCESSOR_NO_ERROR;
}

void _cmdDrawConsoleBin(processor_t *APM){
    for(size_t  i = 0; i < APM->height; i++){
        for(size_t j = 0; j < APM->width; j++){
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
    for(size_t  i = 0; i < APM->height; i++){
        for(size_t j = 0; j < APM->width; j++){
            printf("%c", APM->RAM[APM->VRAMptr + APM->width * i + j]);
        }
        printf("\n");
    }
}
void _cmdDrawGraphicWindowBW(processor_t *APM){
    InitWindow(APM->width, APM->height, "Black and White");

    while(!WindowShouldClose()){
        BeginDrawing();

            ClearBackground(RAYWHITE);

            for(size_t y = 0; y < APM->height; y++){
                for(size_t x = 0; x < APM->width; x++){
                    if(APM->RAM[global_VRAMptr + y * APM->width + x] == 67){
                        DrawPixel(x, y, WHITE);
                    }
                    else{
                        DrawPixel(x, y, BLACK);
                    }
                }
            }

        EndDrawing();
    }

    CloseWindow();
}
void _cmdDrawGraphicWindowRGB(processor_t *APM){//TODO - ask amir
    printf("in progress\n");
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

void ProcessorErrorProcessing(processor_errorList err, processor_t *APM){

    if(APM->code != NULL){
        free(APM->code);
    }
    if(APM->RAM != NULL){
        free(APM->RAM);
    }    

    if(APM->stack->status == INITIALIZED){
        StackDestructor(APM->stack);
    }
    if(APM->labelRecursion->status == INITIALIZED){
        StackDestructor(APM->labelRecursion);
    }

    switch(err){
        case PROCESSOR_ERROR_BINARY_FREAD:
            fprintf(stderr, "Error in fread in binary file\n");
            return;
        case PROCESSOR_ERROR_BINARY_FSEEK:
            fprintf(stderr, "Error in fseek in binary file\n");
            return;
        case PROCESSOR_ERROR_BINARY_OPEN:
            fprintf(stderr, "Error in fopen in binary file\n");
            return;
        case PROCESSOR_ERROR_CODE_ALLOCATION:
            fprintf(stderr, "Error in code memory allocation\n");
            return;
        case PROCESSOR_ERROR_FILESIZE:
            fprintf(stderr, "Error in fstat getting filesize\n");
            return;
        case PROCESSOR_ERROR_INVALID_COMMAND:
            fprintf(stderr, "Invalid command in processing\n");
            return;
        case PROCESSOR_ERROR_RAM_ALLOCATION:
            fprintf(stderr, "Error in RAM allocation\n");
            return;
        default:
            printf("OK\n");
            return;
    }
}