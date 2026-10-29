#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

//#define DEBUG_MODE
#define CANARY_PROTECTION
#define HASH_PROTECTION

#ifdef DEBUG_MODE
    #define DEBUG(...) __VA_ARGS__
#else
    #define DEBUG(...)
#endif

#define INT_MODE
//#define LL_INT_MODE
//#define DOUBLE_MODE

#ifdef INT_MODE
    typedef int stackType;
#elifdef LL_INT_MODE
    typedef ssize_t stackType
#elifdef DOUBLE_MODE
    typedef double stackType
#endif

#define StackConstructor(capacity, ...) _StackConstructor(capacity, DEBUG(__VA_ARGS__,) __FILE__, __FUNCTION__, __LINE__)
#define StackVerify(stk) _StackVerify(stk, __FILE__, __FUNCTION__, __LINE__)
#define StackPrintf(stk) _StackPrintf(stk, __FILE__, __FUNCTION__, __LINE__)
#define StackPush(stk, elem) _StackPush(stk, elem, __FILE__, __FUNCTION__, __LINE__)
#define StackPop(stk, elem) _StackPop(stk, elem, __FILE__, __FUNCTION__, __LINE__)
#define StackDestructor(stk) _StackDestructor(stk, __FILE__, __FUNCTION__, __LINE__)
#define StackDump(stk) _StackDump(stk, STACK_IS_OK, __FILE__, __FUNCTION__, __LINE__)

static ssize_t globalDebugHashSum;

const ssize_t globalFiasco = 0xF1A5C0;    //15836608

const ssize_t globalMinCapacity = 5;

#ifdef CANARY_PROTECTION

    const stackType canaryL = 48099263;   //10110111011110111110111111
    const stackType canaryR = 86118464;  //101001000100001000001000000

#endif

enum stack_status{
    NOT_INITIALIZED = 0,
    INITIALIZED = 1,
    DESTROYED = 2
};

enum ErrorSuccess{
    RETURN_SUCCESS = 0,
    RETURN_ERROR = 1
};

enum stackErrorList{
    STACK_IS_OK = 0,
    STACK_WASNT_INITIALIZED = 1,
    STACK_WAS_DESTROYED_YET = 2,
    NULL_PTR_ON_DATA = 3,
    NULL_PTR_ON_STACK = 4,
    SIZE_OVER_CAPACITY = 5,
    SIZE_BELOW_ZERO = 6,
    CAPACITY_LOWER_MINIMAL = 7,
    CAPACITY_BELOW_ZERO = 8

#ifdef CANARY_PROTECTION

    , CANARY_DATA_LEFT_ATTACKED = 9,
    CANARY_DATA_RIGHT_ATTACKED = 10,
    CANARY_STACK_LEFT_ATTACKED = 11,
    CANARY_STACK_RIGHT_ATTACKED = 12

#endif

#ifdef HASH_PROTECTION

    , HASH_WAS_CHANGED = 13

#endif
};

struct stack_t;

stack_t *_StackConstructor(ssize_t capacity,
                      DEBUG( const char *crName,) const char *crFile,
                      const char *crFunc, ssize_t crLine);
void _StackDestructor(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine);

ErrorSuccess _StackVerify(const stack_t *stk, const char *verFile, const char *verFunc, ssize_t verLine);
stackErrorList StackCheckErrors(const stack_t *stk);
void _StackPrintf(const stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine);
void _StackDump(const stack_t *stk, stackErrorList error, const char *dmpFile, const char *dmpFunc, ssize_t dmpLine);
const char *GetLineStatus(stack_status status);
const char *GetLineError(stackErrorList status);


void _StackPush(stack_t *stk, stackType *elem, const char *callFile, const char *callFunc, ssize_t callLine);
void _StackPop(stack_t *stk, stackType *elem, const char *callFile, const char *callFunc, ssize_t callLine);

void StackResizeUp(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine);
void StackResizeDown(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine);

#ifdef HASH_PROTECTION

    ssize_t StackHashCounter(const stack_t *stk);

#endif

//TODO - resizedown
//NOTE - done

//TODO - stackdump
//NOTE - completed

//TODO - define debug
//NOTE - completed

//TODO - verificator
//NOTE - completed

//TODO - verificator for constructor
//ANCHOR - ASK DED
//NOTE - nahui nado

//TODO - message in console: look in log file
//NOTE - completed

//TODO - struct after main 
//ANCHOR - ASK DED
//NOTE - completed

//TODO - canary protection - in progress
//NOTE - completed

//TODO - hash protection
//NOTE - completed

//TODO - switchi
//NOTE - completed 

//TODO - code style
//NOTE - completed

//TODO - obertki
//NOTE completed

//TODO - dump typedef

struct stack_t{
    
#ifdef CANARY_PROTECTION

    stackType canaryStackLeft;

#endif

    DEBUG(const char *createName;)

    const char *createFile;
    const char *createFunc;
    ssize_t createLine;
    
#ifdef CANARY_PROTECTION

    stackType canaryLeft;
    stackType *data;
    stackType canaryRight;

#else

    stackType *data;

#endif

#ifdef HASH_PROTECTION

    ssize_t hash;

#endif

    ssize_t size;
    ssize_t capacity;
    stack_status status;

#ifdef CANARY_PROTECTION

    stackType canaryStackRight;

#endif
};

#ifdef HASH_PROTECTION

ssize_t StackHashCounter(const stack_t *stk){

    ssize_t hash = 5381;

    char *temp = (char *)stk;

    for(int i = 0; i < sizeof(stack_t); i++){
        hash = hash + (hash << 5) + *(temp + i);
    }
    
    return hash;
}

#endif

stack_t *_StackConstructor(ssize_t capacity,
                      DEBUG( const char *crName,) const char *crFile,
                      const char *crFunc, ssize_t crLine){

    stack_t *stk  = (stack_t *)calloc(1, sizeof(stack_t));
    assert(stk != NULL);

    DEBUG(stk->createName = crName;)

    stk->createFile = crFile;
    stk->createFunc = crFunc;
    stk->createLine = crLine;
    
    //printf("pizda\n");

#ifdef CANARY_PROTECTION

    stk->canaryStackLeft = canaryL;

    stk->canaryStackRight = canaryR;

    stackType *temp = (stackType *)calloc(capacity + 2, sizeof(ssize_t));
    assert(temp != NULL);

    temp[0] = canaryL;

    temp[capacity + 1] = canaryR;

    stk->data = temp + 1;

#else

    stk->data = (stackType *)calloc(capacity, sizeof(stackType));

#endif

    stk->capacity = capacity;
    stk->size = 0;
    stk->status = INITIALIZED;

#ifdef HASH_PROTECTION

    stk->hash = 0;

    stk->hash = StackHashCounter(stk);

    globalDebugHashSum = stk->hash;

    //printf("%lld\n", stk->hash);

#endif

    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, crFile, crFunc, crLine);
    
    if(err){
        printf("Error in StackConstructor(), check log file\n");
        return NULL;
    }

    return stk;
}


ErrorSuccess _StackVerify(const stack_t *stk, const char *verFile, const char *verFunc, ssize_t verLine){
    
    stackErrorList error = StackCheckErrors(stk);

    if(error){
        _StackDump(stk, error, verFile, verFunc, verLine);
        exit(1);
        return RETURN_ERROR;
    }
    return RETURN_SUCCESS;
}

stackErrorList StackCheckErrors(const stack_t *stk){
    if(stk == NULL){
        return NULL_PTR_ON_STACK;
    }

    if(stk->status == NOT_INITIALIZED){
        return STACK_WASNT_INITIALIZED;
    }

    if(stk->status == DESTROYED){
        return STACK_WAS_DESTROYED_YET;
    }

    if(stk->data == NULL){
        return NULL_PTR_ON_DATA;
    }

    if(stk->size < 0){
        return SIZE_BELOW_ZERO;
    }

#ifdef CANARY_PROTECTION

    if(stk->canaryStackLeft != canaryL){
        return CANARY_STACK_LEFT_ATTACKED;
    }

    if(stk->canaryStackRight != canaryR){
        return CANARY_STACK_RIGHT_ATTACKED;
    }

    if(stk->data[-1] != canaryL){
        return CANARY_DATA_LEFT_ATTACKED;
    }

    if(stk->data[stk->capacity] != canaryR){
        return CANARY_DATA_RIGHT_ATTACKED;
    }

#endif

    if(stk->capacity < globalMinCapacity){
        return CAPACITY_LOWER_MINIMAL;
    }
    
    if(stk->capacity <= 0){
        return CAPACITY_BELOW_ZERO;
    }
    
    if(stk->size > stk->capacity + 1){
        return SIZE_OVER_CAPACITY;
    }

#ifdef HASH_PROTECTION

    stack_t stkTmp = *stk;

    stkTmp.hash = 0;

    ssize_t hashTmp = StackHashCounter(&stkTmp);
    if(stk->hash != hashTmp){
        printf("%lld, %lld\n", stk->hash, hashTmp);

        return HASH_WAS_CHANGED;
    }

#endif

    else{
        return STACK_IS_OK;
    }
}





void _StackDump(const stack_t *stk, stackErrorList error, const char *dmpFile, const char *dmpFunc, ssize_t dmpLine){
    
    FILE *fp = fopen("stack_dump.log", "a");

    fputs("Hello! StackDump is working!\n", fp);
    fprintf(fp, "Function was called by %s() at %s:%lld\n", dmpFunc, dmpFile, dmpLine);
    
    if(error){
        fprintf(fp, "StackDump was called with error code: %s\n", GetLineError(error));
    }

    DEBUG(printf("file0\n");)

    if(error == NULL_PTR_ON_STACK){
        fputs("StackDump detected critical error: ptr on stack variable = 0\n", fp);
        fputs("Program won't be working correctly\n", fp);
        
        fclose(fp);
        exit(1);
    }

    DEBUG(printf("file1\n");)

    if(stk->status == NOT_INITIALIZED){
        fputs("Stack wasn't initialized\n", fp);
        
        fclose(fp);
        exit(1);
    }

#ifdef CANARY_PROTECTION

    if(error == CANARY_STACK_LEFT_ATTACKED){
        fprintf(fp, "Left stack canary was attecked:\n");
        fprintf(fp, "Left stack canary = %d at [%p] <-- have ot be %d\n",
                stk->canaryStackLeft, &(stk->canaryStackLeft), canaryL);
        fprintf(fp, "Right stack canary = %d at [%p] <-- have ot be %d\n",
                stk->canaryStackRight, &(stk->canaryStackRight), canaryR);
        exit(1);
    }

    if(error == CANARY_DATA_LEFT_ATTACKED){
        fprintf(fp, "Left data canary was attecked:\n");
        fprintf(fp, "Left stack canary = %d at [%p] <-- have ot be %d\n",
                stk->canaryStackLeft, &(stk->canaryStackLeft), canaryL);
        fprintf(fp, "Right stack canary = %d at [%p] <-- have ot be %d\n",
                stk->canaryStackRight, &(stk->canaryStackRight), canaryR);
        exit(1);
    }

    if(error == CANARY_DATA_RIGHT_ATTACKED){
        fprintf(fp, "Right data canary was attecked:\n");
        fprintf(fp, "Left stack canary = %d at [%p] <-- have ot be %d\n",
                stk->canaryStackLeft, &(stk->canaryStackLeft), canaryL);
        fprintf(fp, "Right stack canary = %d at [%p] <-- have ot be %d\n",
                stk->canaryStackRight, &(stk->canaryStackRight), canaryR);
        exit(1);
    }

    if(error == CANARY_STACK_RIGHT_ATTACKED){
        fprintf(fp, "Right stack canary was attecked:\n");
        fprintf(fp, "Left stack canary = %d at [%p] <-- have ot be %d\n",
                stk->canaryStackLeft, &(stk->canaryStackLeft), canaryL);
        fprintf(fp, "Right stack canary = %d at [%p] <-- have ot be %d\n",
                stk->canaryStackRight, &(stk->canaryStackRight), canaryR);
        exit(1);
    }
#endif

    DEBUG(printf("file2\n");)

#ifdef HASH_PROTECTION

    if(error == HASH_WAS_CHANGED){ 
        fprintf(fp, "Hash sum = %lld <-- have to be %lld\n", stk->hash, globalDebugHashSum);
        fclose(fp);
        exit(1);
    }

#endif

#ifdef DEBUG_MODE

    fprintf(fp, "stack_t \"%s\" (located at %p) created by %s() at %s:%lld\n{\n",
            stk->createName, stk, stk->createFunc, stk->createFile, stk->createLine);
#else

    fprintf(fp, "stack_t (located at %p) created by %s() at %s:%lld\n{\n",
            stk, stk->createFunc, stk->createFile, stk->createLine);

#endif

    fprintf(fp, "\tstack_status = %s\n", GetLineStatus(stk->status));

#ifdef CANARY_PROTECTION

    fprintf(fp, "\tLeft Canary  = %d at [%p] <-- have to be 48099263\n", stk->canaryStackLeft, &(stk->canaryStackLeft));
    fprintf(fp, "\tRight Canary = %d at [%p] <-- have to be 86118464\n", stk->canaryStackRight, &(stk->canaryStackRight));

#endif 

    fprintf(fp, "\tsize = %lld\n\tcapacity = %lld\n\tdata = %p\n\t{\n", stk->size, stk->capacity, stk->data);

    DEBUG(printf("file3\n");)

    if(error == NULL_PTR_ON_DATA){
        fputs("Sorry, can't show elements of stack, cause ptr on stack = 0" , fp);

        fclose(fp);
        exit(1);
    }

    DEBUG(printf("file4\n");)

    for(ssize_t index = 0; index < stk->capacity + 2; index++){

        if(index == 0){
            fprintf(fp, "\t\t[%p]\t[%lld] = %d <-- canaryLeft (have to be 48099263)\n", stk->data + index - 1, index - 1, stk->data[index - 1]);
            continue;
        }

        if(index < stk->size + 1){
            fprintf(fp, "\t\t[%p]\t*[%lld] = %d\n", stk->data + index - 1, index - 1, stk->data[index - 1]);
            continue;
        }

        if(stk->size + 1 <= index && index < stk->capacity + 1){
            //fprintf(fp, "%lld", stk->capacity);
            fprintf(fp, "\t\t[%p]\t [%lld] = %d\n", stk->data + index - 1, index - 1, stk->data[index - 1]);
            continue;
        }

        else{
            fprintf(fp, "\t\t[%p]\t [%lld] = %d <-- canaryRight(have to be 86118464)\n", stk->data + index - 1, index - 1, stk->data[index - 1]);
            continue;
        }
    }

    fputs("\t}\n}\n\n\n", fp);
    fclose(fp);
}




const char *GetLineStatus(stack_status status){
    
    switch(status){

        case NOT_INITIALIZED:
            return "NOT_INITIALIZED";

        case INITIALIZED:
            return "INITIALIZED";

        case DESTROYED:
            return "DESTROYED";

        default:
            return NULL;
    }
}

const char *GetLineError(stackErrorList error){
    
    switch(error){
    
        case STACK_IS_OK:
            return "STACK_IS_OK";

        case STACK_WASNT_INITIALIZED:
            return "STACK_WASNT_INITIALIZED";

        case STACK_WAS_DESTROYED_YET:
            return "STACK_WAS_DESTROYED_YET";

        case NULL_PTR_ON_DATA:
            return "NULL_PTR_ON_DATA";

        case NULL_PTR_ON_STACK:
            return "NULL_PTR_ON_STACK";

        case SIZE_OVER_CAPACITY:
            return "SIZE_OVER_CAPACITY";
        
        case SIZE_BELOW_ZERO:
            return "SIZE_BELOW_ZERO";

        case CAPACITY_LOWER_MINIMAL:
            return "CAPACITY_LOWER_MINIMAL";

        case CAPACITY_BELOW_ZERO:
            return "CAPACITY_BELOW_ZERO";

    #ifdef CANARY_PROTECTION

        case CANARY_DATA_LEFT_ATTACKED:
            return "CANARY_DATA_LEFT_ATTACKED";

        case CANARY_DATA_RIGHT_ATTACKED:
            return "CANARY_DATA_RIGHT_ATTACKED";
        
        case CANARY_STACK_LEFT_ATTACKED:
            return "CANARY_STACK_LEFT_ATTACKED";
            
        case CANARY_STACK_RIGHT_ATTACKED:
            return "CANARY_STACK_RIGHT_ATTACKED";

    #endif

    #ifdef HASH_PROTECTION

        case HASH_WAS_CHANGED:
            return "HASH_WAS_CHANGED";

    #endif

        default:
            return NULL;
    }
}


void _StackPush(stack_t *stk, stackType *elem, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    if(stk->size < stk->capacity){
        stk->data[stk->size] = *elem;
        stk->size++;
    }
    else{
        DEBUG(StackDump(stk);)

        StackResizeUp(stk, callFile, callFunc, callLine);

        stk->data[stk->size] = *elem;
        stk->size++;
    }

#ifdef HASH_PROTECTION

    stk->hash = 0;
    stk->hash = StackHashCounter(stk);    

    globalDebugHashSum = stk->hash;

#endif

    err = _StackVerify(stk, callFile, callFunc, callLine);

    if(err){
        printf("Error in StackPush(), check log file\n");
    }
}

void _StackPop(stack_t *stk, stackType *elem, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;
    
    err = _StackVerify(stk, callFile, callFunc, callLine);

    stk->size--;

    if(4 * stk->size <= stk->capacity && stk->capacity > globalMinCapacity){
        DEBUG(StackDump(stk);)

        StackResizeDown(stk, callFile, callFunc, callLine);
    }

#ifdef HASH_PROTECTION

    stk->hash = 0;
    stk->hash = StackHashCounter(stk);    

    globalDebugHashSum = stk->hash;

#endif

    err = _StackVerify(stk, callFile, callFunc, callLine);

    *elem = stk->data[stk->size];

    if(err){
        printf("Error in StackPop(), check log file\n");
    }

#ifdef HASH_PROTECTION

    stk->hash = 0;
    stk->hash = StackHashCounter(stk);    

    globalDebugHashSum = stk->hash;

#endif

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    if(err){
        printf("Error in StackPop(), check log file\n");
    }
}


void _StackDestructor(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    for(ssize_t index = 0; index <= stk->capacity; index++){
        stk->data[index] = (stackType)globalFiasco;
    }

#ifdef CANARY_PROTECTION

    stk->data[-1] = globalFiasco;
    stk->data[stk->capacity + 1] = globalFiasco;
    free(stk->data - 1);
    stk->data = NULL;

#else

    free(stk->data);
    stk->data = NULL;

#endif

    stk->size = globalFiasco;
    stk->capacity = globalFiasco;
    stk->status = DESTROYED;

    if(err){
        printf("Error in StackDestructor(), check log file\n");
    }   
}



void StackResizeUp(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
#ifdef CANARY_PROTECTION

    stackType *temp = (stackType *)realloc((stk->data - 1), sizeof(stackType) * ((stk->capacity + 1)* 2));
    
    if(temp == NULL){
        printf("It isn't possible to increase stack((\n");

        err = _StackVerify(stk, callFile, callFunc, callLine);

        if(err){
            printf("Error in StackResizeUp(), check log file\n");
        }  
    
        return;
    }

    stk->capacity *= 2;

    temp[0] = canaryL;

    temp[stk->capacity + 1] = canaryR;
    
    stk->data = temp + 1;
    
    DEBUG(StackDump(stk);)

#else

    void *temp = realloc((stk->data), sizeof(stackType) * (stk->capacity* 2));

    if(temp == NULL){
        printf("It wasn't possible to increase stack((\n");

        err = _StackVerify(stk, callFile, callFunc, callLine);

        if(err){
            printf("Error in StackResizeUp(), check log file\n");
        }  

        return;
    }
    stk->data = (stackType *)temp;
    stk->capacity *= 2;

#endif

#ifdef HASH_PROTECTION

    stk->hash = 0;
    stk->hash = StackHashCounter(stk);    

    globalDebugHashSum = stk->hash;

#endif

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    if(err){
        printf("Error in StackResizeUp(), check log file\n");
    }
}

void StackResizeDown(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
#ifdef CANARY_PROTECTION

    stackType *temp = (stackType *)realloc((stk->data - 1), sizeof(stackType) * ((stk->capacity + 2) / 2 + 1));
    
    if(temp == NULL){
        printf("It wasn't possible to increase stack((\n");

        err = _StackVerify(stk, callFile, callFunc, callLine);

        if(err){
            printf("Error in StackResizeDown(), check log file\n");
        }
        return;
    }

    stk->capacity = stk->capacity / 2;

    temp[0] = canaryL;

    temp[stk->capacity + 1] = canaryR;
    
    stk->data = temp + 1;
    
    DEBUG(StackDump(stk);)

#else

    void *temp = realloc(stk->data, sizeof(stackType) * (stk->capacity / 2));
    if(temp == NULL){
        printf("It wasn't possible to reduce stack((\n");

        err = _StackVerify(stk, callFile, callFunc, callLine);

        if(err){
            printf("Error in StackResizeDown(), check log file\n");
        }

        return;
    }

    stk->data = (stackType *)temp;
    stk->capacity /= 2;

#endif

#ifdef HASH_PROTECTION

    stk->hash = 0;
    stk->hash = StackHashCounter(stk);

    globalDebugHashSum = stk->hash;

#endif

    err = _StackVerify(stk, callFile, callFunc, callLine);

    if(err){
        printf("Error in StackResizeDown(), check log file\n");
    }

    return;
}



void _StackPrintf(const stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);

    
    printf("\n===================================================================================\n");

#ifdef CANARY_PROTECTION

    printf("%p\t%p\n", &(stk->canaryLeft), &(stk->canaryRight));

#endif

    printf("size = %d\tcapacity = %d\n", stk->size, stk->capacity);
    
    if (stk->size == 0){
        printf("Stack is empty:(\n");
    }
    
    for(ssize_t i = 0; i < stk->size; i++){
        printf("[%d]\t", stk->data[i]);
    }

    printf("\n===================================================================================\n");
    
    err = _StackVerify(stk, callFile, callFunc, callLine);

    if(err){
        printf("Error in StackPrintf(), check log file\n");
    }

    return;
}

#endif