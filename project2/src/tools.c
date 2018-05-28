#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "tools.h"
#include <unistd.h>

void syserr(char *prog, char *msg)
{
    fprintf(stderr, "%s - error: %s\n",prog, msg);
    perror("system error");
    exit(1);
}

void println(char *str) {
    int nOfChar = strsize(str);
    write(STDOUT, str, nOfChar);
    write(STDOUT, "\n", 1);
}

void printerr(char *str) {
    int nOfChar = 0;
    for (int i = 0; str[i] != '\0'; i++, nOfChar++);
    write(STDERR, str, nOfChar);
    write(STDERR, "\n", 1);
}

int strsize(char *str) {
    int n;
    for (n = 0; str[n] != '\0'; n++);
    return n;
}

char *itoa(int n) {
    int tmp = n;
    int digits = 0;
    do {
        digits++;
    } while((tmp /= 10) != 0);

    char *result = (char *)malloc((sizeof(char) * digits) + 1);
    for (int i = digits - 1; i >= 0; i--) {
        int digit = (n % 10);
        result[i] = (char)(digit + '0');
        n /= 10;
    }

    result[digits] = '\0';
    return result;
}

char *strcct(char *str1, char *str2) {
    int len1 = strsize(str1);
    int len2 = strsize(str2);
    int newStringSize = len1 + len2 + 1;
    int newStringIndex = 0;
    char *result = (char *)malloc(sizeof(char) * newStringSize);

    for (int i = 0; i < len1; i++, newStringIndex++) {
        result[newStringIndex] = str1[i];
    }

    for (int i = 0; i < len2; i++, newStringIndex++) {
        result[newStringIndex] = str2[i];
    }

    result[newStringIndex] = '\0';
    return result;
}

void strcp(char *str1, char *str2) {
    int i;
    for(i = 0; *str2 != '\0'; str1++, str2++, i++){
        *str1 = *str2;
    }
    i++;
    *str1++ = '\0';
}

char *utoh(unsigned value) {
    unsigned n = value;
    int digits = 0;
    while(n != 0){
        n = n / 16;
        digits++;
    }

    char *new_string = (char *)malloc(sizeof(char) * 8);

    int i = 0;
    for(i = digits - 1; i >= 0; i--){
        if((value % 16) <= 9){
            new_string[i] = (char) (value % 16) + '0';
        }
        else{
            new_string[i] = (char) ((value % 16) - 10) + 'A';
        }
        value = value/16;
    }

    return new_string; 
}
