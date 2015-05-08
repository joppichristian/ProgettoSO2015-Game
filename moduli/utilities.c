#include "utilities.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


void printMessage(char messages, char type){
    //switch (type){
      //  case "error": printf(ANSI_COLOR_RED     "This text is RED!"     ANSI_COLOR_RESET "\n");  //inserisco messaggio al posto della stringa

        
        if (strcmp(type,"error")==0){
            printf (strcat("ANSI_COLOR_RED",strcat(messages,"ANSI_COLOR_RESET\n")));
        }
        else if (strcmp(type,"warning")==0){
            printf (strcat("ANSI_COLOR_YELLOW",strcat(messages,"ANSI_COLOR_RESET\n")));
        }
        else if (strcmp(type,"confirm")==0){
            printf (strcat("ANSI_COLOR_GREEN",strcat(messages,"ANSI_COLOR_RESET\n")));
    }
}