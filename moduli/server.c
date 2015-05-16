#include <stdio.h>
#include <unistd.h>
#include "server.h"
#include "utilities.h"
#include <sys/errno.h>
#include <stdlib.h>
#include <string.h>


//-------------------- DEFINIZIONE FUNZIONI ------------------------

//inizializza il server impostando il numero massimo di giocatori e il punteggio di vittoria
void init(int massimo,int ptg_vittoria){
    lock = 0;
    strcpy(domanda,"");
    risposta = 0;
    if(massimo < 1 || massimo > 10)
        printMessage("valore MAX non consentito MAX:10","error"); 
    MAX = massimo;
    if(ptg_vittoria < 10 || ptg_vittoria > 100)
        printMessage("valore WIN non consentito MAX:50","error"); 
    WIN = ptg_vittoria;
    
    //Creo la FIFO che comunica daClientVersoServer per l'inserimento dei giocatori in partita se non è già stata creata ( server già esistente ) 
    if(mkfifo("fifo_player",FILE_MODE))
    {
        if(errno!= EEXIST)
        {
            printMessage("Error creating FIFO player", "error");
            exit(1);
        }
        //ERRNO != EEXIST
        printMessage("Server already exists", "error");
        exit(1);
    }
    else
    {
        if((FIFO_player = open("fifo_player",O_RDONLY))<0)
            printMessage("Error opening FIFO", "error");
        pthread_create(&THREAD_CONN,NULL,(void*)&listenPlayer,NULL);
        pthread_join(THREAD_CONN, NULL);
        
    }
    unlink("fifo_player");
}

void *listenPlayer(){
    printMessage("Entering listener player\n","confirm");
    char BUFFER [255];
    char* tmp;
    int FIFO_player_ANSW;
    pthread_t THREAD_GAME [MAX];
    while(fine==0){
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

                printf("HO LETTO E CREATO LA FIFO %s GIOCATORI ISCRITTI: %i \n",BUFFER,ACTIVE_PLAYER+1);
                write(FIFO_player_ANSW,"OK\0",3);

                //MUTEX LOCK
                pthread_mutex_lock(&PLAYER_MUTEX);
                
                strcpy(players[ACTIVE_PLAYER].pid,tmp);
                players[ACTIVE_PLAYER].punteggio = MAX-ACTIVE_PLAYER;
                
                //Creo la FIFO per la risposta al server
                strcat(tmp,"fifo_game_toS");
                mkfifo(tmp,FILE_MODE);
                if((players[ACTIVE_PLAYER].FIFO_game[0] = open(tmp,O_RDONLY | O_NONBLOCK))<0)
                    perror("FIFO_toS");
                //
                
                strcpy(tmp,players[ACTIVE_PLAYER].pid);
                
                //Creo la FIFO per l'invio della domanda al client
                strcat(tmp,"fifo_game_toC");
                mkfifo(tmp,FILE_MODE);
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
    unlink("fifo_player");
    pthread_exit(NULL);
    

}


void *gestioneASKandANS(int giocatore){
   //INVIA DOMANDA
    printMessage(domanda,"warning");
    
    char _risposta [30];
    
    write(players[giocatore].FIFO_game[1],domanda,sizeof(domanda));
    //CICLO
    while(players[giocatore].punteggio < WIN){
        //ASPETTA RISPOSTA
        while(read(players[giocatore].FIFO_game[0],_risposta,sizeof(risposta))==0);
        
        if(strlen(_risposta)!=0){
            
            printMessage(_risposta,"error");
            int tmp =atoi(_risposta);
            //CONTROLLA RISPOSTA
            //SE SI --> BLOCCO GLI ALTRI E AUMENTO IL PUNTEGGIO E RICOMiNCIO
            if(tmp == risposta && lock != 1)
            {
                //Segnalo che un client ha risposto giusto!
                lock = 1;
                printf("Il giocatore %s ha risposto giusto \n",players[giocatore].pid);
                players[giocatore].punteggio +=1;
                if(players[giocatore].punteggio < WIN){
                    makeAsk();
                    //Invio la nuova domanda a tutti gli altri client!
                    for(int i=0;i<ACTIVE_PLAYER;i++)
                        write(players[i].FIFO_game[1],domanda,sizeof(domanda));
                    printf("PID: %s PUNT: %d\n",players[giocatore].pid,players[giocatore].punteggio);
                }
            }
            else
            {
                players[giocatore].punteggio-=1;
                write(players[giocatore].FIFO_game[1],"NO\0",3);
                printf("Il giocatore %s ha risposto sbagliato \n",players[giocatore].pid);
            }
            lock=0;
        }
        
        strcpy(_risposta,"");
    }
    char *classifica = (char*)malloc((sizeof(char)*20)*(ACTIVE_PLAYER+2));
    classifica = makeClassifica();
    printf("Classifica: \n%s",classifica);
    pthread_mutex_lock(&PLAYER_MUTEX);
    for(int i=0;i<ACTIVE_PLAYER;i++)
        write(players[i].FIFO_game[1],classifica,sizeof(classifica));
    pthread_mutex_unlock(&PLAYER_MUTEX);
    free(classifica);
    fine = 1;
    printMessage("Il gioco è terminato","warning");
    pthread_exit(NULL);
}

void makeAsk(){
    srand(time(0));
    int int_a = rand() % 100;
    int int_b = rand() % 100;
    risposta = int_a + int_b;
    char a [4]; 
    char b [4];
    sprintf(a,"%d",int_a);
    strcpy(domanda,a);
    strcat(domanda,"+");
    sprintf(b,"%d",int_b);
    strcat(domanda,b);
}

char* makeClassifica()
{
    char *classifica = (char*)malloc((sizeof(char)*20)*(ACTIVE_PLAYER+2));
    char tmp [4];
    strcpy(classifica,"<PLAYER>\t<PUNTEGGIO>\n  ");
    pthread_mutex_lock(&PLAYER_MUTEX);
    for(int i=0;i<ACTIVE_PLAYER;i++)
    {
        strcat(classifica,players[i].pid);
        strcat(classifica,"\t\t    ");
        sprintf(tmp,"%d",players[i].punteggio);
        strcat(classifica,tmp);
        strcat(classifica,"\n  ");
    }
    strcat(classifica,"\0");
    pthread_mutex_unlock(&PLAYER_MUTEX);
    return classifica;
}


//-----------------------------------------------------------------------

