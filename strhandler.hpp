#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

//#define STRHANDLER_DEBUG_MODE

#ifdef STRHANDLER_DEBUG_MODE
    #define STRHANDLER_DEBUG(...) __VA_ARGS__
#else
    #define STRHANDLER_DEBUG(...)
#endif

enum strhandler_errorList{
    STRHANDLER_NO_ERROR = 0,
    STRHANDLER_ERROR_FILESIZE,
    STRHANDLER_ERROR_BUFFER_CREATE,
    STRHANDLER_ERROR_FILEIN_OPEN,
    STRHANDLER_ERROR_FILEIN_READ,
    STRHANDLER_ERROR_ARRAY_MEMORY_ALLOCATION,
    STRHANDLER_ERROR_FILEOUT_OPEN,
    STHANDLER_ERROR_OUTPUTBUFFER_OPENFILE,
    STRHANDLER_ERROR_OUTPUTBUFFER_WRITE,

};

struct line_t{
    const char* line;
    size_t len;
};

struct strInfo_t{
    const char* fileInName;
    const char* fileOutName;
    FILE *fileOut;

    char *buffer;
    size_t bufferSize;

    line_t *arrLines;
    size_t arrLinesSize;

    int error;
};



void StrInfoConstructor(strInfo_t *env, const char *fileInName, const char *fileOutName);
void StrInfoDestructor(strInfo_t *env);

strhandler_errorList TextProcessing(strInfo_t *env, const char *fileInName);
strhandler_errorList TextProcessingAsm(strInfo_t *env, const char *fileInName);

strhandler_errorList OpenFile(strInfo_t *env);
strhandler_errorList BufferProcessing(strInfo_t *env);
strhandler_errorList BufferProcessingAsm(strInfo_t *env);

strhandler_errorList OpenOutputFile(FILE **fileOut, const char *fileOutName);
ssize_t GetFileSize(const char *fileName);
char *CreateBuffer(size_t bufSize);
strhandler_errorList ReadFileInBuffer(const char *fileName, char **buffer, size_t bufSize);
size_t ChangeEndlinesToNulls(char *buffer, size_t bufSize);
size_t ChangeEndlinesAndBackspacesToNulls_AndSkipComments(char *buffer, size_t bufSize);
void SkipComments(char *buffer, size_t *index);
void ChangeNullsToEndlines(char *buffer, size_t bufSize);
strhandler_errorList CreateArrayOfLines(strInfo_t *env);
void BufferToLinesFragmentation(line_t *arrLines, char *buffer, size_t bufSize);
void GetLensOfLines(line_t *arrLines, size_t arrSize);

strhandler_errorList OutputBuffer(strInfo_t *env);
void StrDump(strInfo_t *env);


strhandler_errorList TextProcessing(strInfo_t *env, const char *fileInName){
    assert(fileInName != NULL);
    
    strhandler_errorList err = STRHANDLER_NO_ERROR;

    StrInfoConstructor(env, fileInName, "out.txt");

    err = OpenFile(env);
    if(err){
        return err;
    }

    err = BufferProcessing(env);
    if(err){
        return err;
    }

    return STRHANDLER_NO_ERROR;
}

strhandler_errorList TextProcessingAsm(strInfo_t *env, const char *fileInName){
    assert(env != NULL);
    assert(fileInName != NULL);

    strhandler_errorList err = STRHANDLER_NO_ERROR;

    STRHANDLER_DEBUG(printf("fileIn = [%s]\n", fileInName);)
    STRHANDLER_DEBUG(printf("text_debug0\n");)

    StrInfoConstructor(env, fileInName, "out.txt");

    STRHANDLER_DEBUG(printf("text_debug1\n");)

    err = OpenFile(env);
    if(err){
        return err;
    }

    // err = OutputBuffer(env);
    // if(err){
    //     return err;
    // }

    STRHANDLER_DEBUG(printf("text_debug2\n");)

    err = BufferProcessingAsm(env);
    if(err){
        return err;
    }

    STRHANDLER_DEBUG(printf("text_debug3\n");)

    return STRHANDLER_NO_ERROR;
}

void StrInfoConstructor(strInfo_t *env, const char *fileInName, const char *fileOutName){
    assert(fileInName != NULL);
    assert(fileOutName != NULL);

    env->fileInName = fileInName;
    env->fileOutName = fileOutName;
    env->fileOut = NULL;

    env->arrLines = NULL;
    env->arrLinesSize = 0;

    env->buffer = NULL;
    env->bufferSize = 0;

    env->error = 0;
}

strhandler_errorList OpenFile(strInfo_t *env){
    assert(env != NULL);

    ssize_t fileSize = GetFileSize(env->fileInName);
    if(fileSize <= 0){
        return STRHANDLER_ERROR_FILESIZE;
    }
    else{
        env->bufferSize = (size_t)fileSize;
    }

    env->buffer = CreateBuffer(env->bufferSize);
    if(env->buffer == NULL){
        return STRHANDLER_ERROR_BUFFER_CREATE;
    }

    strhandler_errorList err = ReadFileInBuffer(env->fileInName, &(env->buffer), env->bufferSize);
    if(err){
        return err;
    }

    env->buffer[env->bufferSize] = '\n';

    env->bufferSize++;

    STRHANDLER_DEBUG(printf("buffer after reading = [%s]\n", env->buffer);)

    return STRHANDLER_NO_ERROR;
   
}

strhandler_errorList BufferProcessing(strInfo_t *env){
    assert(env != NULL);

    strhandler_errorList err = STRHANDLER_NO_ERROR;

    env->arrLinesSize = ChangeEndlinesToNulls(env->buffer, env->bufferSize);

    err = CreateArrayOfLines(env);
    if(err){
        return err;
    }
    return STRHANDLER_NO_ERROR;
}

strhandler_errorList BufferProcessingAsm(strInfo_t *env){
    assert(env != NULL);

    strhandler_errorList err = STRHANDLER_NO_ERROR;

    STRHANDLER_DEBUG(printf("text_proc_debug0\n");)

    env->arrLinesSize = ChangeEndlinesAndBackspacesToNulls_AndSkipComments(env->buffer, env->bufferSize);

    STRHANDLER_DEBUG(printf("arrLinesSize = [%d]\n", env->arrLinesSize);)

    STRHANDLER_DEBUG(printf("text_proc_debug1\n");)

    err = CreateArrayOfLines(env);
    if(err){
        return err;
    }
    STRHANDLER_DEBUG(printf("text_proc_debug3\n");)

    return STRHANDLER_NO_ERROR;
}

void StrInfoDestructor(strInfo_t *env){
    assert(env != NULL);
    
    env->fileInName = "";
    env->fileOutName = "";
    env->fileOut = NULL;
    env->arrLinesSize = 0;
    env->bufferSize = 0;

    env->error = -1;

    if(env->arrLines != NULL){
        free(env->arrLines);
    }
    if((env->buffer - 1) != NULL){
        free(env->buffer - 1);
    }
}



strhandler_errorList OpenOutputFile(FILE **fileOut, const char *fileOutName){
    assert(fileOut != NULL);
    assert(fileOutName != NULL);

    if((*fileOut = fopen(fileOutName, "a")) == NULL){
        return STRHANDLER_ERROR_FILEOUT_OPEN;
    }

    return STRHANDLER_NO_ERROR;
}

ssize_t GetFileSize(const char *fileName){
    assert(fileName != NULL);
    struct stat fileStat = {};

    if(stat(fileName, &fileStat) == -1){
        return -1;
    }

    return fileStat.st_size;
}

char *CreateBuffer(size_t bufSize){
    char *tempBuffer = (char *)calloc(bufSize + 2, sizeof(char));

    if(tempBuffer == NULL){
        return NULL;
    }
    
    return tempBuffer + 1;
}

strhandler_errorList ReadFileInBuffer(const char *fileName, char **buffer, size_t bufSize){
    assert(fileName != NULL);
    assert(buffer != NULL);

    int fileInDescriptor = open(fileName, O_BINARY);
    if(fileInDescriptor == -1){
        return STRHANDLER_ERROR_FILEIN_OPEN;
    }
    if (read(fileInDescriptor, *buffer, bufSize) <= 0){
        return STRHANDLER_ERROR_FILEIN_READ;
    }

    return STRHANDLER_NO_ERROR;
}

size_t ChangeEndlinesToNulls(char *buffer, size_t bufSize){
    assert(buffer != NULL);

    size_t endlCounter = 0;

    for(size_t index = 0; index < bufSize; index++){
        if(buffer[index] == '\r'){
            buffer[index] = '\0';
        }
        if(buffer[index] == '\n'){
            endlCounter++;

            buffer[index] = '\0';
            while(((index + 1) < bufSize) && (buffer[index + 1] == '\n')){
                buffer[index + 1] = '\0';
                index++;
            }
        }
    }
    return endlCounter;
}

size_t ChangeEndlinesAndBackspacesToNulls_AndSkipComments(char *buffer, size_t bufSize){
    assert(buffer != NULL);

    size_t endlCounter = 0;

    for(size_t index = 0; index < bufSize; index++){

        if((buffer[index - 1] == '\0') && (buffer[index] == ';')){
            STRHANDLER_DEBUG(printf("skip full line\n");)
            SkipComments(buffer, &index); //skip full line comment
            while(((index) < bufSize) && 
                  ((buffer[index] == '\n' || buffer[index] == '\r') || buffer[index] == ' ')){
                buffer[index] = '\0';
                index++;
            }
            index--; 
            continue;
        }
        else if((buffer[index - 1] != '\0') && (buffer[index] == ';')){
            STRHANDLER_DEBUG(printf("skip comment after\n");)
            endlCounter++;
            SkipComments(buffer, &index); //skip comment after instruction line
            while(((index) < bufSize) && 
                  ((buffer[index] == '\n' || buffer[index] == '\r') || buffer[index] == ' ')){
                buffer[index] = '\0';
                index++;
            }
            index--;
            continue;
        }
        
        if(buffer[index] == ' '){
            endlCounter++;
            buffer[index] = '\0';
            while(((index + 1) < bufSize) && (buffer[index + 1] == ' ')){
                buffer[index + 1] = '\0';
                index++;
            }
        }
        if(buffer[index] == '\r'){
            buffer[index] = '\0';
        }
        if(buffer[index] == '\n'){
            endlCounter++;
            buffer[index] = '\0';
            while(((index + 1) < bufSize) && ((buffer[index + 1] == '\n' || buffer[index + 1] == '\r'))){
                buffer[index + 1] = '\0';
                index++;
            }
        }
    }

    return endlCounter;
}

void SkipComments(char *buffer, size_t *index){
    STRHANDLER_DEBUG(printf("found ;\n");)
    while(buffer[*index] != '\n'){
        STRHANDLER_DEBUG(printf("skip\n");)
        buffer[*index] = '\0';
        (*index)++;
    }
    return;    
}

void ChangeNullsToEndlines(char *buffer, size_t bufSize){
    assert(buffer != 0);

    for(size_t index = 0; index < bufSize; index++){
        if(buffer[index] == '\0'){
            buffer[index] = '\n'; 
        }
    }
}

strhandler_errorList CreateArrayOfLines(strInfo_t *env){
    assert(env != NULL);

    STRHANDLER_DEBUG(printf("text_proc_create_arr_debug0\n");)

    env->arrLines = (line_t *)calloc(env->arrLinesSize, sizeof(line_t));
    if(env->arrLines == NULL){
        return STRHANDLER_ERROR_ARRAY_MEMORY_ALLOCATION;
    }

    STRHANDLER_DEBUG(printf("text_proc_create_arr_debug1\n");)

    BufferToLinesFragmentation(env->arrLines, env->buffer, env->bufferSize);

    STRHANDLER_DEBUG(printf("text_proc_create_arr_debug2\n");)

    GetLensOfLines(env->arrLines, env->arrLinesSize);

    STRHANDLER_DEBUG(printf("text_proc_create_arr_debug3\n");)
    
    return STRHANDLER_NO_ERROR;
}

void BufferToLinesFragmentation(line_t *arrLines, char *buffer, size_t bufSize){
    assert(arrLines != NULL);
    assert(buffer != NULL);

    size_t linesIndex = 0;

    for(ssize_t index = -1; index < (ssize_t)(bufSize - 1); index++){
        if(buffer[index] == '\0'){
            while((index + 1) < (ssize_t)bufSize && buffer[index + 1] == '\0'){
                index++;
            }
            arrLines[linesIndex].line = &buffer[index + 1];

            linesIndex++;
        }
    }
}

void GetLensOfLines(line_t *arrLines, size_t arrSize){
    assert(arrLines != NULL);

    STRHANDLER_DEBUG(printf("text_proc_create_arr_get_length_debug\n");)
    for(size_t index = 0; index < arrSize; index++){
        STRHANDLER_DEBUG(printf("i = [%d]\n", index);)
        STRHANDLER_DEBUG(printf("str[%d] = [%s]\n", index, arrLines[index].line);)
        if(index == (arrSize - 1)){
            STRHANDLER_DEBUG(printf("last str: [%s]\n", arrLines[index].line);)
            arrLines[index].len = strlen(arrLines[index].line);
            break;
        }     
        if(arrLines[index].line == 0){
            arrLines[index].len = 0;
        }
        else{
            arrLines[index].len = arrLines[index + 1].line - arrLines[index].line - 1;
        }
    }
}



strhandler_errorList OutputBuffer(strInfo_t *env){
    assert(env != NULL);

    FILE *outText = fopen("output_buffer.txt", "w");
    if(outText == NULL){
        return STHANDLER_ERROR_OUTPUTBUFFER_OPENFILE;
    }

    if(fwrite(env->buffer, sizeof(char), env->bufferSize, outText) <= 0){
        return STRHANDLER_ERROR_OUTPUTBUFFER_WRITE;
    }
    fclose(outText);

    return STRHANDLER_NO_ERROR;
}

void StrDump(strInfo_t *env){
    assert(env != NULL);

    FILE *fileDump = fopen("str_dump.log", "a");

    fputs("StrDump is working!\n", fileDump);
    if(env->error){
        fprintf(fileDump, "StrDump was called with error code: %d!\n", env->error);
    }
    fputs("Struct strInfo_t:\n{\n", fileDump);

    fprintf(fileDump, "\tInput file name: [%s]\n", env->fileInName);
    fprintf(fileDump, "\tOutput file name: [%s]\n", env->fileOutName);
    fprintf(fileDump, "\tOutput file stream ptr: [%p]\n", env->fileOut);
    fprintf(fileDump, "\tBuffer's first 32 symbols: [%32s]\n", env->buffer);
    fprintf(fileDump, "\tBuffer's size: [%d]\n", env->bufferSize);
    fprintf(fileDump, "\tArray of lines ptr: [%p]\n", env->arrLines);
    fprintf(fileDump, "\tFirst line in array: [%s], len = [%d]\n", env->arrLines[0].line, env->arrLines[0].len);
    fprintf(fileDump, "\tLast line in array: [%s], len = [%d]\n",
            env->arrLines[env->arrLinesSize - 1].line, env->arrLines[env->arrLinesSize - 1].len);
    fprintf(fileDump, "\tArray of lines lens: [%d]\n", env->arrLinesSize);

    fputs("}\n", fileDump);

    fclose(fileDump);

}