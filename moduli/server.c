#include <stdio.h>
#include <unistd.h>
#include "server.h"
#include "utilities.h"
#include <sys/errno.h>
#include <stdlib.h>
#include <string.h>


//-------------------- DEFINIZIONE FUNZIONI ------------------------

//inizializza il server impostando il numero massimo di giocatori e il punteggio di vittoria
int init(int massimo,int ptg_vittoria){
    lock = 0;
    strcpy(domanda,"");
    strcpy(risposta,"");
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
            printMessage("Error creating FIFO player.\n", "error");
            exit(1);
        }
        //ERRNO != EEXIST
        printMessage("FIFO player already exists.\n", "error");
    }
    else
    {
        if((FIFO_player = open("fifo_player",O_RDONLY))<0)
            printMessage("Error opening FIFO.\n", "error");
            printMessage("FIFO successfully created.\n", "confirm"); 
        pthread_create(&THREAD_CONN,NULL,(void*)&listenPlayer,NULL);
        pthread_join(THREAD_CONN, NULL);
        
    }
    unlink("fifo_player");
    return 0;
}

void *listenPlayer(){
    printMessage("Entering listener player\n","confirm");
    char BUFFER [255];
    char* tmp;
    int FIFO_player_ANSW;
    pthread_t THREAD_GAME [MAX];
    while(1){
        strcpy(BUFFER,"");
        read(FIFO_player,BUFFER,255);
        if(strlen(BUFFER)!=0){
            char tmp [30];
            strcpy(tmp,BUFFER);
            strcat(BUFFER,"fifo_player");
            if((FIFO_player_ANSW = open(BUFFER,O_WRONLY))<0)
                {
                    printMessage("Error opening FIFO ANSW.\n", "error");
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
                if((players[ACTIVE_PLAYER].FIFO_game[0] = open(tmp,O_RDONLY | O_NONBLOCK))<0)
                    perror("FIFO_toS");
                //
                printf("ho creato la FIFO in lett . %s\n",tmp); 
                strcpy(tmp,players[ACTIVE_PLAYER].pid);
                 printf("BUFFER: %s\n",tmp);
                //Creo la FIFO per l'invio della domanda al client
                strcat(tmp,"fifo_game_toC");
                mkfifo(tmp,FILE_MODE);
                printf("ho creato la FIFO in scrit . %s\n",tmp); 
                players[ACTIVE_PLAYER].FIFO_game[1] = open(tmp,O_WRONLY);
                //
                
                //CREA DOMANDA SE NON E' GIA' STATA FATTA
                if(strlen(domanda) ==0)
                    makeAsk();
                
                //CREO THREAD PER INVIO DOMANDE E ATTESA RISPOSTE CLIENT
                pthread_create(&THREAD_GAME[ACTIVE_PLAYER],NULL,(void*)&gestioneASKandANS,ACTIVE_PLAYER);
                ACTIVE_PLAYER++;
                
                //MUTEX UNLOCK
                pthread_mutex_unlock(&PLAYER_MUTEX);
                
            }
            else
            {
                printMessage("QUEUE IS FULL.\n","error");
                write(FIFO_player_ANSW,"NO\0",3);
            }
        }

    }
    pthread_exit(NULL);
    

}


void *gestioneASKandANS(int giocatore){
   //INVIA DOMANDA
    printMessage(domanda,"warning");
    printf("%d\n",giocatore);
    char _risposta [30];
    write(players[giocatore].FIFO_game[1],domanda,sizeof(domanda));
    //CICLO
    while(1){
        //ASPETTA RISPOSTA
        read(players[giocatore].FIFO_game[0],_risposta,255);
        if(strlen(_risposta)!=0){
            //CONTROLLA RISPOSTA
            //SE SI --> BLOCCO GLI ALTRI E AUMENTO IL PUNTEGGIO E RICOMiNCIO
            if(strcmp(_risposta,risposta) ==0 && lock != 1)
            {
                //Segnalo che un client ha risposto giusto!
                lock = 1;
                printf("Il giocatore %s ha risposto giusto \n",players[giocatore].pid);
                players[giocatore].punteggio +=1;
                makeAsk();
                //Invio la nuova domanda a tutti gli altri client!
                for(int i=0;i<ACTIVE_PLAYER;i++)
                     write(players[i].FIFO_game[1],domanda,sizeof(domanda));
            }
            else
            {
                write(players[giocatore].FIFO_game[1],"NO\0",3);
                printf("Il giocatore %s ha risposto sbagliato \n",players[giocatore].pid);
            }
            lock=0;
        }
        strcpy(_risposta,"");
    }
    

    
    //FINE CICLO
}

void makeAsk(){
    int int_a = getRandom();
    int int_b = getRandom();
    sprintf(risposta,"%d",int_a + int_b);
    char a [3]; 
    char b [3];
    sprintf(a,"%d",int_a);
    strcpy(domanda,a);
    strcat(domanda,"+");
    sprintf(b,"%d",int_b);
    strcat(domanda,b);
}



//-----------------------------------------------------------------------

