#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"
#include <string.h>


int main(void){
    char str1[] = "hello";
    char str2[] = "World";
    char str3[12];
    strcpy(str3, str1);
    printf("after the strcpy with str1: %s\n", str3);
    strcat(str3, str2);
    printf("after the strcat with str2: %s\n", str3);
    return 0;
}