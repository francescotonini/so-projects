#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <tools.h>
#include <constants.h>

void syserr(char *prog, char *msg)
{
    char *error = " error: ";

    write(STDERR, prog, strsize(prog));
    write(STDERR, error, strsize(error));
    write(STDERR, msg, strsize(msg));
    perror("system error");
    exit(1);
}

void println(char *str) {
    char *strWithNewline = strcct(str, "\n");
    write(STDOUT, strWithNewline, strsize(strWithNewline));

    free(strWithNewline);
}

void printerr(char *str) {
    char *strWithNewline = strcct(str, "\n");
    write(STDERR, strWithNewline, strsize(strWithNewline));

    free(strWithNewline);
}

int strsize(char *str) {
    int n;
    for (n = 0; str[n] != '\0'; n++);
    return n;
}

char *itoa(int n) {
    int tmp = n;
    int digits = 0;
    int i;
    do {
        digits++;
    } while((tmp /= 10) != 0);

    char *result = (char *)malloc((sizeof(char) * digits) + 1);
    for (i = digits - 1; i >= 0; i--) {
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

    int i;
    for (i = 0; i < len1; i++, newStringIndex++) {
        result[newStringIndex] = str1[i];
    }

    for (i = 0; i < len2; i++, newStringIndex++) {
        result[newStringIndex] = str2[i];
    }

    result[newStringIndex] = '\0';
    return result;
}

void strcp(char *str1, char *str2) {
    int i;
    for (i = 0; str2[i] != '\0'; i++) {
        str1[i] = str2[i];
    }

    str1[i] = '\0';
}

char *utoh(unsigned value) {
    char *new_string = (char *)malloc(sizeof(char) * 8);

    int i;
    for(i = 7; i >= 0; i--){
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
