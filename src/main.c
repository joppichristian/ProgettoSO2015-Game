#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "moduli/server.h"
#include "moduli/client.h"
#include "moduli/utilities.h"


void startServer(char * n_max,char * max,char * n_win,char * win);

/*
funzione main che verifica che i parametri passati in fase di esecuzione del programma siano corretti altrimenti restituisce un messaggio di errore
*/

int main(int argc,char** argv)
{       
    int esito;
    if(argc< 2) //verifica che il numero di argomenti passati sia >= 2
    {
            printMessage("Arguments Error: use <server|client> --optional_args\n", "error");
            exit(1);
    }
   
/*
Legge argomento obbligatorio e decide se il processo corrente sarà il server o un client del gioco e in funzione dell'esito richiama la funzione StartServer oppure la funzione init_client. Nel caso in cui nn venga specificato nè client nè server il programma stampa un messaggio d'errore
*/
    
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
    }
    else if((strcmp(argv[1],"client"))==0){
        init_client();
    }
    else{
        printMessage("Arguments Error: use <server|client> --optional_args\n", "error");
        exit(1);
    }
}


/*
funzione che prende come input quattro stringhe e
    - verifica che n_max sia uguale a "--max" e se l'esito è positivo setta max come numero massimo di giocatori
    - verifica che n_win sia uguale a "--win" e se l'esito è positivo setta win come punteggio di vincita
    - richiama la funzione init passando i valori massimo e win_ptg
*/

void startServer(char * n_max,char * max,char * n_win,char * win)   
{
    int massimo=10;
    int win_ptg = 50;
    if((strcmp(n_max,"--max"))==0) //strcmp confronta le due stringhe e restituisce 0 se esse sono uguali
        massimo = atoi(max);
    if((strcmp(n_win,"--win"))==0)
        win_ptg = atoi(win);
    init(massimo,win_ptg);
}
