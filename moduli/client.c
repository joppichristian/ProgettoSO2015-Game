#include <stdio.h>
#include <unistd.h>
#include "client.h"




//funzione init che verifica l'esistenza di un server e in caso esista scrive nella FIFO il relativo pid e aspetta la risposta del server
int init_client(){
    char BUFFER[255];
    if((FIFO_player[0] = open("fifo_player",O_WRONLY | O_NONBLOCK))<0)
    {
        if(ERRNO == EEXIST)
        {
            return -3;  //non c'è server
        }
        else {
            int pid = getpid();
            write(FIFO_player[0], pid, sizeof(pid));
            mkfifo(strcat("fifo_player",pid), FILE_MODE);
            FIFO_player[1] = open(strcat("fifo_player",pid),O_RDONLY | O_NONBLOCK);
            read(FIFO_player[1], BUFFER, sizeof(BUFFER));
            if (atoi(BUFFER)==-1){
                return -3;
            } 
            printf("Connesso!");
            unlink(strcat("fifo_player",pid));
        }        
    }
}

//funzione che mostra in output la domanda posta dal server e attende che l'utente inserisca una risposta pe rpoi inviarla
//FIFO_game[0] è la fifo da server verso client, FIFO_game[1] è la fifo da client verso server
void ascoltaServer(){
    char BUFFER[255];
    open("fifo_game_toC", getpid() , O_RDONLY | O_NONBLOCK);
    while(1){
        read(FIFO_game[0], BUFFER, sizeof(BUFFER));
        printf("%s\n", BUFFER);
        dup2(0,FIFO_game[1]);
    }
}