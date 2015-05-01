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
        if((FIFO_player = open("fifo_player",O_RDONLY | O_NONBLOCK))<0)
            perror("Errore di apertura FIFO");
        printf("FIFO CREATA\n");
        pthread_create(&THREAD_CONN,NULL,(void*)&listenPlayer,NULL);
        pthread_join(THREAD_CONN, NULL);
    }
    else
        perror("Già esistente");
    return 0;
}

void *listenPlayer(){
    printf("Entro nel listener giocatori");
    char BUFFER [255];
    char * tmp;
    int FIFO_player_ANSW;
    while(1){
        printf("CICLO WHILE\n");
        while(read(FIFO_player,BUFFER,sizeof(BUFFER))==0);
        printf("%s\n",BUFFER);
        mkfifo(strcat(BUFFER,"fifo_player\0"),FILE_MODE);
        if((FIFO_player_ANSW = open(strcat(BUFFER,"fifo_player\0"),O_WRONLY | O_NONBLOCK))<0)
        {
                perror("Errore santo dio");
             pthread_exit(NULL);
        }
        printf("HO LETTO E CREATO LA \n");
        if(ACTIVE_PLAYER < MAX)
        {
            players[ACTIVE_PLAYER].pid = atoi(BUFFER);
            players[ACTIVE_PLAYER].punteggio = 0;
            mkfifo(strcat("fifo_game_toS",BUFFER),FILE_MODE);
            mkfifo(strcat("fifo_game_toC",BUFFER),FILE_MODE);
            if((players[ACTIVE_PLAYER].FIFO_game[0] = open(strcat("fifo_game_toS",BUFFER),O_RDONLY | O_NONBLOCK))<0)
                perror("DznxlW");
            if((players[ACTIVE_PLAYER].FIFO_game[1] = open(strcat("fifo_game_toC",BUFFER),O_WRONLY | O_NONBLOCK))<0)
                perror("Sada");
            write(FIFO_player_ANSW,"OK\0",3);
            //CREO THREAD PER INVIO DOMANDE E ATTESA RISPOSTE CLIENT
            ACTIVE_PLAYER++;
        }
        else
        {
            write(FIFO_player_ANSW,"-1\0",2);
        }
    }
    
    pthread_exit(NULL);

}


void gestioneASKandANS(){
    
}



//-----------------------------------------------------------------------

