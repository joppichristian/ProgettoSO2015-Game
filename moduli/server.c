#include <stdio.h>
#include <unistd.h>
#include "server.h"
#include <sys/errno.h>
#include <stdlib.h>
#include <string.h>

pthread_attr_t attr;

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
    if(mkfifo("fifo_player",FILE_MODE))
    {
        if(errno!= EEXIST)
        {
            perror("Errore creazione FIFO player");
            exit(1);
        }
        //ERRNO != EEXIST
         perror("Già esistente");
    }
    else
    {
        if((FIFO_player = open("fifo_player",O_RDONLY))<0)
            perror("Errore di apertura FIFO");
        printf("FIFO CREATA\n");
        pthread_create(&THREAD_CONN,NULL,(void*)&listenPlayer,NULL);
        pthread_join(THREAD_CONN, NULL);
        
    }
    unlink("fifo_player");
    return 0;
}

void *listenPlayer(){
    printf("Entro nel listener giocatori\n");
    char BUFFER [255];
    int tmp;
    int FIFO_player_ANSW;
    while(1){
        strcpy(BUFFER,"");
        while(read(FIFO_player,BUFFER,255)==0);
        char* tmp = BUFFER;
        strcat(BUFFER,"fifo_player");
        printf("%s\n",BUFFER);;
        if((FIFO_player_ANSW = open(BUFFER,O_WRONLY))<0)
            {
                perror("Errore santo dio");
                unlink("fifo_player");
                pthread_exit(NULL);
            }
        if(ACTIVE_PLAYER < MAX)
        {
           
            printf("HO LETTO E CREATO LA FIFO %s GIOCATORI ISCRITTI: %i \n",BUFFER,ACTIVE_PLAYER);
            write(FIFO_player_ANSW,"OK\0",3);
            players[ACTIVE_PLAYER].pid = tmp;
            players[ACTIVE_PLAYER].punteggio = 0;
            write(FIFO_player_ANSW,"OK\0",3);
            /*strcpy(BUFFER,tmp);
            strcat(BUFFER,"fifo_game_toS");
            mkfifo(BUFFER,FILE_MODE);
            if((players[ACTIVE_PLAYER].FIFO_game[0] = open(BUFFER,O_RDONLY))<0)
                perror("FIFO in entrata");
            strcpy(BUFFER,tmp);
            strcat(BUFFER,"fifo_game_toC")
            mkfifo(BUFFER,FILE_MODE);
            if((players[ACTIVE_PLAYER].FIFO_game[1] = open(BUFFER,O_WRONLY))<0)
                perror("FIFO in uscita");
           
            */
            //CREO THREAD PER INVIO DOMANDE E ATTESA RISPOSTE CLIENT
            ACTIVE_PLAYER++;
        }
        else
        {
            printf("CODA PIENA");
            write(FIFO_player_ANSW,"NO\0",3);
        }
    }
    
    pthread_exit(NULL);

}


void gestioneASKandANS(){
    
}



//-----------------------------------------------------------------------

