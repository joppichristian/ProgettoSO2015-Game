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
        printMessage("Server does not exist.", "error"); //non c'è server
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
        printMessage("SERVER IS FULL.", "warning");
        unlink(tmp_pid);
        exit(1);
    } 
    printMessage("Connected!", "log");
    unlink(tmp_pid);
    pthread_create(&THascolto,NULL,(void*)&ascoltaServer,NULL);
    pthread_join(THascolto, NULL);
    
}

//funzione che mostra in output la domanda posta dal server e attende che l'utente inserisca una risposta per poi inviarla
//FIFO_game[0] è la fifo da server verso client, FIFO_game[1] è la fifo da client verso server
void *ascoltaServer(){
    
    char BUFFER[255];
    char _domanda[6];
    char tmp_pid [20];
    char risposta[10];
    sprintf(tmp_pid,"%d",getpid());
    strcat(tmp_pid,"fifo_game_toC");
    printMessage("Sto ascoltanto le domande dal server","warning");
    FIFO_game[0] = open(tmp_pid, O_RDONLY );
    while(1){
        while((read(FIFO_game[0], BUFFER, sizeof(BUFFER)))==0);
            if (strlen(BUFFER) != 0){
                //CONTROLLO CHE IL MESSAGGIO NON SIA UNA COMUNICAZIONE DEL SERVER MA UNA DOMANDA!
                if(((int)BUFFER[0]) > 47 && (((int)BUFFER[0]) < 58)) {
                    strcpy(_domanda,BUFFER);
                    char * risposta = QuestANDAnsw(_domanda);
                    printMessage(tmp_pid,"log");
                    sprintf(tmp_pid,"%d",getpid());
                    strcat(tmp_pid,"fifo_game_toS");
                    FIFO_game[1] = open(tmp_pid, O_WRONLY);
                    write(FIFO_game[1],risposta, sizeof(risposta));
                }
                else
                {
                    //HO DATO LA RISPOSTA SBAGLIATA!
                    if(strcmp(BUFFER,"NO")==0){
                        printMessage("Risposta sbagliata","error"); 
                        char * risposta = QuestANDAnsw(_domanda);
                        write(FIFO_game[1],risposta, sizeof(risposta));
                    }
                    //HO DATO LA RISPOSTA GIUSTA!
                    /*else if(strcmp(BUFFER,"SI")==0)
                    {
                        printMessage("Risposta corretta e veloce! Complimenti!","confirm"); 
                    }*/
                    else
                    {
                        printMessage(BUFFER,"warning");
                    }
                    
                }
                strcpy(BUFFER,"");
            }
    }    
}

//mostra in output la domanda e attende la risposta dell'utente

char* QuestANDAnsw(char *domanda){  //prima di passare domanda mettere \n
    printMessage(domanda,"confirm");
    char *risposta = (char*)malloc(3*sizeof(char));
    scanf("%s",risposta);
    //printf("%s",risposta);
    return risposta;
}
