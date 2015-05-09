#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "utilities.h"
#include <sys/errno.h>
#include <string.h>
#include <stdlib.h>


//funzione init che verifica l'esistenza di un server e in caso esista scrive nella FIFO il relativo pid e aspetta la risposta del server
int init_client(){
    
    char BUFFER[255];
    if((FIFO_player_CL[0] = open("fifo_player",O_WRONLY | O_NONBLOCK))<0)
    {
        printMessage("Server does not exist.\n", "error");perror("ERROR:"); //non c'è server
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
        printMessage("SERVER IS FULL.\n", "warning");
        unlink(tmp_pid);
        exit(1);
    } 
    printMessage("Connected!\n", "log");
    unlink(tmp_pid);
    pthread_create(&THascolto,NULL,(void*)&ascoltaServer,NULL);
    pthread_join(THascolto, NULL);
    
}

//funzione che mostra in output la domanda posta dal server e attende che l'utente inserisca una risposta per poi inviarla
//FIFO_game[0] è la fifo da server verso client, FIFO_game[1] è la fifo da client verso server
void *ascoltaServer(){
    char BUFFER[255];
    char tmp_pid [20];
    char risposta[10];
    sprintf(tmp_pid,"%d",getpid());
    strcat(tmp_pid,"fifo_game_toC");
    open(tmp_pid, O_RDONLY);
    while(1){
        read(FIFO_game[0], BUFFER, sizeof(BUFFER));
            if (strcmp(BUFFER,"0") != 0){
            char * risposta = QuestANDAnsw(BUFFER);
            strcat(tmp_pid,"fifo_game_toC");
            open(tmp_pid,"fifo_game_toS", O_WRONLY);
            write(FIFO_game[1],risposta, sizeof(risposta));
            }
    }    
}

//mostra in output la domanda e attende la risposta dell'utente

char* QuestANDAnsw(char *domanda){  //prima di passare domanda mettere \n
    printMessage(domanda,"confirm");
    char * risposta;
    scanf("%s",risposta);
    return risposta;
}
