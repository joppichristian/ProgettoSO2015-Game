#include "utilities.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


void printMessage(char messages [], char type[]){ 
    char * tmp;
        if (strcmp(type,"error")==0){
            tmp=(char *)malloc(sizeof(char)*(strlen(ANSI_COLOR_RED)+strlen(ANSI_COLOR_RESET)+strlen(messages) +1));
            strcpy(tmp,ANSI_COLOR_RED);
            strcat(tmp,messages);
            strcat(tmp,ANSI_COLOR_RESET);
            printf ("%s\n",tmp);
        }
        else if (strcmp(type,"warning")==0){
            tmp=(char *)malloc(sizeof(char)*(strlen(ANSI_COLOR_YELLOW)+strlen(ANSI_COLOR_RESET)+strlen(messages) +1));
            strcpy(tmp,ANSI_COLOR_YELLOW);
            strcat(tmp,messages);
            strcat(tmp,ANSI_COLOR_RESET);
            printf ("%s\n",tmp);
        }
        else if (strcmp(type,"confirm")==0){
           tmp=(char *)malloc(sizeof(char)*(strlen(ANSI_COLOR_GREEN)+strlen(ANSI_COLOR_RESET)+strlen(messages) +1));
            strcpy(tmp,ANSI_COLOR_BLUE);
            strcat(tmp,messages);
            strcat(tmp,ANSI_COLOR_RESET);
            printf ("%s\n",tmp);
        }
    free(tmp);
}
int getRandom(){
    srand(time(NULL));
    int r = rand() % 100;
    return r;
}