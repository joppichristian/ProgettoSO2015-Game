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
    
    //Creo la FIFO che comunica daClientVersoServer per l'inserimento dei giocatori in partita se non è già stata creata ( server già esistente ) 
    if((FIFO_player = open("fifo_player",O_RDONLY | O_NONBLOCK))<0)
    {
        if(ERRNO == EEXIST)
        {
            mkfifo("fifo_player",FILE_MODE);
            printf("CREATA\n");
            //ERRNO != EEXIST
            if((FIFO_player = open("fifo_player_toS",O_RDONLY | O_NONBLOCK))<0)
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
    char BUFFER [255];
    int FIFO_player_ANSW;
    
    while(1){
        
        read(FIFO_player,BUFFER,sizeof(BUFFER));
        if((FIFO_player_ANSW = open(strcat("fifo_player",BUFFER),O_WRONLY | O_NONBLOCK))<0)
                perror(ERRNO);
        if(ACTIVE_PLAYER < MAX)
        {
            player[ACTIVE_PLAYER] = new player();
            player[ACTIVE_PLAYER].pid = atoi(BUFFER);
            player[ACTIVE_PLAYER].punteggio = 0;
            mkfifo(strcat("fifo_game_toS",BUFFER),FILE_MODE);
            mkfifo(strcat("fifo_game_toC",BUFFER),FILE_MODE);
            if((player[ACTIVE_PLAYER].FIFO_game[0] = open(("fifo_game_toS",BUFFER),O_RDONLY | O_NONBLOCK))<0)
                perror(ERRNO);
            if((player[ACTIVE_PLAYER].FIFO_game[1] = open(("fifo_game_toC",BUFFER),O_WRONLY | O_NONBLOCK))<0)
                perror(ERRNO);
            write(FIFO_player_ANSW,"OK\0",3);
            //CREO THREAD PER INVIO DOMANDE E ATTESA RISPOSTE CLIENT
            ACTIVE_PLAYER++;
        }
        else
        {
            write(FIFO_player_ANSW,"-1\0",2);
        }
    }

}


void gestioneASKandANS(){
    
}



//-----------------------------------------------------------------------

