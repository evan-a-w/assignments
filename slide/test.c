#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int main(void) {
    char cmd[200] = "./slide";
    system(cmd);
    char *cpy = "16";
    strcpy(cmd, cpy);

    char *cpy = "0 3 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "1 2 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "2 0 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "2 1 4";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "3 1 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "4 1 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "5 1 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "6 1 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "7 1 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "3 2 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "4 3 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "5 4 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "6 5 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "7 6 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "0 1 1";
    strcpy(cmd, cpy);
    system(cmd);
    char *cpy = "1 1 1";
    strcpy(cmd, cpy);
    system(cmd);
}
