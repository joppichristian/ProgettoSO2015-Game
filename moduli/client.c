#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "utilities.h"
#include <sys/errno.h>
#include <string.h>
#include <stdlib.h>


//funzione init che verifica l'esistenza di un server e in caso esista scrive nella FIFO il relativo pid e aspetta la risposta del server
void init_client(){
    
    char BUFFER[255];
    strcpy(pathFIFOtoC,"");
    strcpy(pathFIFOtoS,"");
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
    //CONTROLLO SE IL SERVER é PRONTO PER ACCOGLIERE ALTRE CONNESSIONI.
    if (strcmp(BUFFER,"NO")==0){
        printMessage("SERVER IS FULL", "warning");
        unlink(tmp_pid);
        exit(1);
    } 
    unlink(tmp_pid);
    pthread_create(&THascolto,NULL,(void*)&ascoltaServer,NULL);
    pthread_join(THascolto, NULL);
    
}

//funzione che mostra in output la domanda posta dal server e attende che l'utente inserisca una risposta per poi inviarla
//FIFO_game[0] è la fifo da server verso client, FIFO_game[1] è la fifo da client verso server
void *ascoltaServer(){
    pthread_t THREAD_LETTURA;
    char BUFFER[1000];
    char _domanda[10];
    char tmp_pid [20];
    char risposta[10];
    sprintf(tmp_pid,"%d",getpid());
    strcat(tmp_pid,"fifo_game_toC");
    strcpy(pathFIFOtoC,tmp_pid);
    printMessage("Sto ascoltanto le domande dal server","warning");
    FIFO_game[0] = open(tmp_pid, O_RDONLY );
    while(1){
        read(FIFO_game[0], BUFFER, sizeof(BUFFER));
            if (strlen(BUFFER) != 0){
                pthread_kill(&THREAD_LETTURA,SIGSTOP);
                //CONTROLLO CHE IL MESSAGGIO NON SIA UNA COMUNICAZIONE DEL SERVER MA UNA DOMANDA!
                if(((int)BUFFER[0]) > 47 && (((int)BUFFER[0]) < 58)) {
                    
                    strcpy(_domanda,BUFFER);
                    printMessage(_domanda,"confirm");
                    pthread_create(&THREAD_LETTURA,NULL,(void*)&QuestANDAnsw,_domanda);
                    
                    
                }
                else if((int)BUFFER[0] == 60)                           //Controllo se il messaggio in arrivo inizia col carattere < 
                {                                                       //cioè sto leggendo la classifica finale
                    printMessage("Classifica:","confirm");
                    printMessage(BUFFER,"confirm");
                    unlink(pathFIFOtoC);
                    unlink(pathFIFOtoS);
                    pthread_exit(NULL);
                }
                else
                {
                    //HO DATO LA RISPOSTA SBAGLIATA!
                    if(strcmp(BUFFER,"NO")==0){
                        printMessage("Risposta sbagliata","error"); 
                        pthread_create(&THREAD_LETTURA,NULL,(void*)&QuestANDAnsw,_domanda);
                    }
                    else
                    {
                        printMessage(BUFFER,"warning");
                    }
                    
                }
                strcpy(BUFFER,"\0");
            }
    }    
}

//mostra in output la domanda e attende la risposta dell'utente

void* QuestANDAnsw(char *domanda){  //prima di passare domanda mettere \n
    char *risposta = (char*)malloc(4*sizeof(char));
    char *tmp = (char*) malloc(20*sizeof(char));
    fgets(risposta,sizeof(risposta),stdin);
    strcat(risposta,"\0");
    sprintf(tmp,"%d",getpid());
    strcat(tmp,"fifo_game_toS");
    if(strlen(pathFIFOtoS)==0)
        strcpy(pathFIFOtoS,tmp); 
    FIFO_game[1] = open(tmp, O_WRONLY);
    write(FIFO_game[1],risposta, sizeof(risposta));
    free(risposta);
    free(tmp);
}



