#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "moduli/server.h"
#include "moduli/client.h"
#include "moduli/utilities.h"

void startServer(char * n_max,char * max,char * n_win,char * win);

int main(int argc,char** argv)
{       
    int esito;
    if(argc< 2)
    {
            printMessage("Arguments Error: use <server|client> --optional_args\n", "error");
            exit(1);
    }
   
    //Legge argomento obbligatorio e decide se il processo corrente sarà il server o un client del gioco.
    if((strcmp(argv[1],"server"))==0){
        if(argc == 4){
            if((strcmp(argv[2],"--max"))==0){
                startServer(argv[2],argv[3],"","");
            }
            else if((strcmp(argv[2],"--win"))==0)
            {
                startServer("","",argv[2],argv[3]);
             
            }
        }
        else if(argc==6)
        {
            if((strcmp(argv[2],"--max"))==0)
            {
                startServer(argv[2],argv[3],argv[4],argv[5]);
            }
            else if((strcmp(argv[2],"--win"))==0){
                startServer(argv[4],argv[5],argv[2],argv[3]);
               
            }
        }
        else
        {
            startServer("","","","");
        }
        printf("Il server è stato creato\n");
    }
    else if((strcmp(argv[1],"client"))==0){
        init_client();
    }
    else{
        printMessage("Arguments Error: use <server|client> --optional_args\n", "error");
        exit(1);
    }
}

void startServer(char * n_max,char * max,char * n_win,char * win)   
{
    int massimo=10;
    int win_ptg = 50;
    if((strcmp(n_max,"--max"))==0)
        massimo = atoi(max);
    if((strcmp(n_win,"--win"))==0)
        win_ptg = atoi(win);
    init(massimo,win_ptg);
}
