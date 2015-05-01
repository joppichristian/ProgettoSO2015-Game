#include <stdio.h>
#include <unistd.h>
#include "server.h"
#include <sys/errno.h>



//-------------------- DEFINIZIONE FUNZIONI ------------------------

//inizializza il server impostando il numero massimo di giocatori e il punteggio di vittoria
int init(int massimo,int ptg_vittoria){
    if(massimo < 1 || massimo > 10)
        return -1;
    MAX = massimo;
    if(ptg_vittoria < 10 || ptg_vittoria > 100)
        return -2;
    WIN = ptg_vittoria;
    
    //Creo la FIFO per l'inserimento dei giocatori in partita se non è già stata creata ( server già esistente ) 
    if((FIFO_player = open("fifo_player",O_RDONLY | O_NONBLOCK))<0)
    {
        if(ERRNO == EEXIST)
        {
            mkfifo("fifo_player",FILE_MODE);
            printf("CREATA\n");
            //ERRNO != EEXIST
            if((FIFO_player = open("fifo_player",O_RDONLY | O_NONBLOCK))<0)
                return -3;
        }
        else
            return -3;
    }
    else
        return -4;
    return 0;
}

void listenPlayer(){
    while(1){
        if(
    }

}



//-----------------------------------------------------------------------

