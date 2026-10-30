#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>



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



strInfo_t TextProcessing(const char *fileInName);
strInfo_t TextProcessingAsm(const char *fileInName);

strInfo_t StrInfoConstructor(const char *fileInName, const char *fileOutName);
void OpenFile(strInfo_t *env);
void BufferProcessing(strInfo_t *env);
void BufferProcessingAsm(strInfo_t *env);
void StrInfoDestructor(strInfo_t *env);

void OpenOutputFile(FILE **fileOut, const char *fileOutName);
void TextOutput(strInfo_t *env);
size_t GetFileSize(const char *fileName);
char *CreateBuffer(size_t bufSize);
void ReadFileInBuffer(const char *fileName, char **buffer, size_t bufSize);
size_t ChangeEndlinesToNulls(char *buffer, size_t bufSize);
size_t ChangeEndlinesAndBackspacesToNulls_AndSkipComments(char *buffer, size_t bufSize);
void ChangeNullsToEndlines(char *buffer, size_t bufSize);
void CreateArrayOfLines(strInfo_t *env);
void BufferToLinesFragmentation(line_t *arrLines, char *buffer, size_t bufSize);
void GetLensOfLines(line_t *arrLines, size_t arrSize);

void OutputBuffer(strInfo_t *env);
void StrDump(strInfo_t *env);


strInfo_t TextProcessing(const char *fileInName){
    assert(fileInName != NULL);
    
    strInfo_t env = StrInfoConstructor(fileInName, "out.txt");

    OpenFile(&env);

    BufferProcessing(&env);

    return env;
}

strInfo_t TextProcessingAsm(const char *fileInName){
    assert(fileInName != NULL);

    printf("fileIn = [%s]\n", fileInName);

    printf("text_debug0\n");

    strInfo_t env = StrInfoConstructor(fileInName, "out.txt");

    printf("text_debug1\n");

    OpenFile(&env);

    printf("text_debug2\n");

    BufferProcessingAsm(&env);

    printf("text_debug3\n");

    return env;
}


strInfo_t StrInfoConstructor(const char *fileInName, const char *fileOutName){
    assert(fileInName != NULL);
    assert(fileOutName != NULL);

    strInfo_t env = {};

    env.fileInName = fileInName;
    env.fileOutName = fileOutName;
    env.fileOut = NULL;

    env.arrLines = NULL;
    env.arrLinesSize = 0;

    env.buffer = NULL;
    env.bufferSize = 0;

    env.error = 0;

    return env;
}

void OpenFile(strInfo_t *env){
    assert(env != NULL);

    env->bufferSize = GetFileSize(env->fileInName);

    env->buffer = CreateBuffer(env->bufferSize);

    ReadFileInBuffer(env->fileInName, &(env->buffer), env->bufferSize);

    env->buffer[env->bufferSize] = '\n';

    env->bufferSize++;

    printf("buffer after reading = [%s]\n", env->buffer);
   
}

void BufferProcessing(strInfo_t *env){
    assert(env != NULL);

    env->arrLinesSize = ChangeEndlinesToNulls(env->buffer, env->bufferSize);

    CreateArrayOfLines(env);
}

void BufferProcessingAsm(strInfo_t *env){
    assert(env != NULL);

    printf("text_proc_debug0\n");

    env->arrLinesSize = ChangeEndlinesAndBackspacesToNulls_AndSkipComments(env->buffer, env->bufferSize);

    OutputBuffer(env);

    printf("arrLinesSize = [%d]\n", env->arrLinesSize);

    printf("text_proc_debug1\n");

    CreateArrayOfLines(env);

    printf("text_proc_debug3\n");
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



void OpenOutputFile(FILE **fileOut, const char *fileOutName){
    assert(fileOut != NULL);
    assert(fileOutName != NULL);

    if((*fileOut = fopen(fileOutName, "a")) == NULL){
        assert(0);
    }
}

void TextOutput(strInfo_t *env){
    assert(env != NULL);

    ChangeNullsToEndlines(env->buffer, env->bufferSize);

    if(fwrite(env->buffer, sizeof(char), env->bufferSize, env->fileOut) <= 0){
        assert(0);
    }
}

size_t GetFileSize(const char *fileName){
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

void ReadFileInBuffer(const char *fileName, char **buffer, size_t bufSize){
    assert(fileName != NULL);
    assert(buffer != NULL);

    int fileInDescriptor = open(fileName, O_BINARY);
    if (read(fileInDescriptor, *buffer, bufSize) <= 0){
        assert(0);
    }
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
        if(buffer[index] == ';'){
            while(buffer[index] != '\n'){
                index++;
            }
        }

        // if((buffer[index] == ' ') || (buffer[index] == '\r') || (buffer[index] == '\n')){
        //     endlCounter++;
        //     buffer[index] = '\0';
        //     while(((index + 1) < bufSize) && (buffer[index + 1] == buffer[index])){
        //         buffer[index + 1] = '\0';
        //         index++;
        //     }
        // }
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
            while(((index + 1) < bufSize) && (buffer[index + 1] == '\n')){
                buffer[index + 1] = '\0';
                index++;
            }
        }
    }

    return endlCounter;
}

void ChangeNullsToEndlines(char *buffer, size_t bufSize){
    assert(buffer != 0);

    for(size_t index = 0; index < bufSize; index++){
        if(buffer[index] == '\0'){
            buffer[index] = '\n'; 
        }
    }
}

void CreateArrayOfLines(strInfo_t *env){
    assert(env != NULL);

    printf("text_proc_create_arr_debug0\n");

    env->arrLines = (line_t *)calloc(env->arrLinesSize, sizeof(line_t));

    printf("text_proc_create_arr_debug1\n");

    BufferToLinesFragmentation(env->arrLines, env->buffer, env->bufferSize);

    printf("text_proc_create_arr_debug2\n");

    GetLensOfLines(env->arrLines, env->arrLinesSize);

    printf("text_proc_create_arr_debug3\n");
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

    printf("text_proc_create_arr_get_length_debug0\n");
    for(size_t index = 0; index < arrSize; index++){
        // printf("i = [%d]\n", index);
        // printf("str[%d] = [%s]\n", index, arrLines[index].line);
        if(index == (arrSize - 1)){
            printf("last str: [%s]\n", arrLines[28].line);
            arrLines[index].len = strlen(arrLines[index].line);
            printf("text_proc_create_arr_get_length_debug1\n");
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



void OutputBuffer(strInfo_t *env){
    assert(env != NULL);

    FILE *outText = fopen("output_buffer.txt", "w");
    fwrite(env->buffer, sizeof(char), env->bufferSize, outText);
    fclose(outText);
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