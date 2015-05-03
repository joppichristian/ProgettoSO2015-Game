#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include <sys/errno.h>
#include <string.h>
#include <stdlib.h>


//funzione init che verifica l'esistenza di un server e in caso esista scrive nella FIFO il relativo pid e aspetta la risposta del server
int init_client(){
    
    char BUFFER[255];
    if((FIFO_player_CL[0] = open("fifo_player",O_WRONLY | O_NONBLOCK))<0)
    {
        perror("ERROR:"); //non c'è server
        exit(1);
        
    }
    char pid [20];
    char tmp_pid [20];
    sprintf(tmp_pid,"%d",getpid());
    sprintf(pid,"%d",getpid());
    strcat(tmp_pid,"fifo_player");
    printf("%s\n",pid);
    mkfifo(tmp_pid,FILE_MODE);
    write(FIFO_player_CL[0],pid, sizeof(pid));
    FIFO_player_CL[1] = open(tmp_pid,O_RDONLY);
    while(read(FIFO_player_CL[1], BUFFER, 255)==0);
    printf("%s\n",BUFFER);
    //CONTROLLO SE IL SERVER é PRONTO PER ACCOGLIERE ALTRE CONNESSIONI.
    if (strcmp(BUFFER,"NO")==0){
        printf("SERVER AL COMPLETO");
        unlink(tmp_pid);
        exit(1);
    } 
    printf("Connesso!\n");
    unlink(tmp_pid);
    
    
}

//funzione che mostra in output la domanda posta dal server e attende che l'utente inserisca una risposta pe rpoi inviarla
//FIFO_game[0] è la fifo da server verso client, FIFO_game[1] è la fifo da client verso server
void *ascoltaServer(){
    char BUFFER[255];
    open("fifo_game_toC", getpid() , O_RDONLY | O_NONBLOCK);
    while(1){
        read(FIFO_game[0], BUFFER, sizeof(BUFFER));
        printf("%s\n", BUFFER);
        dup2(0,FIFO_game[1]);
    }
    
}