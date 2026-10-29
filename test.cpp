#include <stdio.h>

int main(){
    const char str[10] = "123]";

    char ext = 0;

    int r = 0;

    char hui[10] = "";

    int check = sscanf(str, "%d]%c", &r, &ext);

    printf("check = %d; hui = [%d]", check, r);
}