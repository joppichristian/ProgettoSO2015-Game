#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "moduli/server.h"
#include "moduli/client.h"

int startServer(char * n_max,char * max,char * n_win,char * win);

int main(int argc,char** argv)
{       
    int esito;
    
    if(argc< 2)
    {
            perror("Arguments Error: use <server|client> --optional_args\n");
            exit(1);
    }
   
    //Legge argomento obbligatorio e decide se il processo corrente sarà il server o un client del gioco.
    if((strcmp(argv[1],"server"))==0){
        if(argc == 4){
            if((strcmp(argv[2],"--max"))==0){
                if((esito = startServer(argv[2],argv[3],"","")) != 0)
                {
                    printf("%i",esito);
                    exit(1);
                }
            }
            else if((strcmp(argv[2],"--win"))==0)
            {
                if((esito = startServer("","",argv[2],argv[3])) != 0)
                {
                    printf("%i",esito);
                    exit(1);
                }
            }
        }
        else if(argc==6)
        {
            if((strcmp(argv[2],"--max"))==0)
            {
                if((esito = startServer(argv[2],argv[3],argv[4],argv[5])) != 0)
                {
                    printf("%i",esito);
                    exit(1);
                }
            }
            else if((strcmp(argv[2],"--win"))==0){
                if((esito = startServer(argv[4],argv[5],argv[2],argv[3])) != 0)
                {
                    printf("%i",esito);
                    exit(1);
                }
            }
        }
        else
        {
            if((esito = startServer("","","","")) != 0)
            {
                perror(esito);
                exit(1);
            }
        }
        printf("Il server è stato creato\n");
    }
    else if((strcmp(argv[1],"client"))==0){
        init_client();
    }
    else{
        perror("Arguments Error: use <server|client> --optional_args\n");
        exit(1);
    }
}

int startServer(char * n_max,char * max,char * n_win,char * win)   
{
    printf("%s,%s,%s,%s\n",n_max,max,n_win,win);
    int massimo=10;
    int win_ptg = 10;
    if((strcmp(n_max,"--max"))==0)
        massimo = atoi(max);
    if((strcmp(n_win,"--win"))==0)
        win_ptg = atoi(win);
    return init(massimo,win_ptg);
}
