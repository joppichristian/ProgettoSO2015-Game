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
    char* tmp;
    int FIFO_player_ANSW;
    int THREAD_GAME [MAX];
    while(1){
        strcpy(BUFFER,"");
        read(FIFO_player,BUFFER,255);
        if(strlen(BUFFER)!=0){
            char tmp [30];
            strcpy(tmp,BUFFER);
            strcat(BUFFER,"fifo_player");
            if((FIFO_player_ANSW = open(BUFFER,O_WRONLY))<0)
                {
                    perror("Errore di apertura FIFO ANSW");
                    unlink("fifo_player");
                    pthread_exit(NULL);
                }
            if(ACTIVE_PLAYER < MAX)
            {

                printf("HO LETTO E CREATO LA FIFO %s GIOCATORI ISCRITTI: %i \n",BUFFER,ACTIVE_PLAYER);
                write(FIFO_player_ANSW,"OK\0",3);

                //MUTEX LOCK
                pthread_mutex_lock(&PLAYER_MUTEX);
                
                strcpy(players[ACTIVE_PLAYER].pid,tmp);
                players[ACTIVE_PLAYER].punteggio = 0;
                
                //Creo la FIFO per la risposta al server
                strcat(tmp,"fifo_game_toS");
                mkfifo(tmp,FILE_MODE);
                if((players[ACTIVE_PLAYER].FIFO_game[0] = open(BUFFER,O_RDONLY | O_NONBLOCK))<0)
                    perror("FIFO_toS");
                //
                
                strcpy(tmp,players[ACTIVE_PLAYER].pid);
                 printf("BUFFER: %s\n",tmp);
                //Creo la FIFO per l'invio della domanda al client
                strcat(tmp,"fifo_game_toC");
                mkfifo(tmp,FILE_MODE);
                players[ACTIVE_PLAYER].FIFO_game[1] = open(BUFFER,O_WRONLY | O_NONBLOCK);
                //
                
                pthread_create(&THREAD_GAME[ACTIVE_PLAYER],NULL,(void*)&gestioneASKandANS,NULL);
                ACTIVE_PLAYER++;
                
                //MUTEX UNLOCK
                pthread_mutex_unlock(&PLAYER_MUTEX);
                
               
               
                
                //CREO THREAD PER INVIO DOMANDE E ATTESA RISPOSTE CLIENT
                
                
                
            }
            else
            {
                printf("CODA PIENA");
                write(FIFO_player_ANSW,"NO\0",3);
            }
        }

    }
    pthread_exit(NULL);
    

}


void *gestioneASKandANS(){
    //CREA DOMANDA
    //INVIA DOMANDA 
    //CICLO
    //ASPETTA RISPOSTA
    //CONTROLLA RISPOSTA
        //SE SI --> BLOCCO GLI ALTRI E AUMENTO IL PUNTEGGIO E RICOMiNCIO
    //FINE CICLO
}



//-----------------------------------------------------------------------

