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
    int nOfChar = 0;
    for (int i = 0; str[i] != '\0'; i++, nOfChar++);
    write(STDOUT, str, nOfChar);
    write(STDOUT, "\n", 1);
}

void printerr(char *str) {
    int nOfChar = 0;
    for (int i = 0; str[i] != '\0'; i++, nOfChar++);
    write(STDERR, str, nOfChar);
    write(STDERR, "\n", 1);
}


