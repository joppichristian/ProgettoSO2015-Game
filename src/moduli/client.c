#include "client.h"
#include "utilities.h"


/*
funzione init_client che verifica l'esistenza di un server e nel caso in cui esista scrive nella FIFO il relativo pid e aspetta la risposta del server altrimenti restituisce un messaggio d'errore
*/

void init_client(){
    
    char BUFFER[255];
    if((FIFO_player_CL[0] = open("fifo_player",O_WRONLY | O_NONBLOCK))<0)
        //verifica che esista fifo_player provando ad aprirla, in quel caso allora esiste già il server che l'ha creata
    {
        printMessage("Server does not exist.", "error"); //non c'è server
        exit(1);
        
    }
    
    if(signal(SIGINT, signal_handler) == SIG_ERR)                        
        //Gestisco le Interrupt <CTRL-C> richiamando il signal_handler 
    {
        printMessage("SERVER ERROR","error");
        exit(-1);
    }
    
    
    char pid [20];
    char tmp_pid [20];
    sprintf(tmp_pid,"%d",getpid());
    sprintf(pid,"%d",getpid());
    strcat(tmp_pid,"fifo_player");
    mkfifo(tmp_pid,FILE_MODE);
    write(FIFO_player_CL[0],pid, sizeof(pid));
    FIFO_player_CL[1] = open(tmp_pid,O_RDONLY);
    while(read(FIFO_player_CL[1], BUFFER, 255)==0);
    //CONTROLLO SE IL SERVER é PRONTO PER ACCOGLIERE ALTRE CONNESSIONI. Se nel buffer trova la stringa "NO" vuol dire che il numero massimo di giocatori è stato raggiunto e quindi stampa un messaggio d'errore
    if (strcmp(BUFFER,"NO")==0){
        printMessage("SERVER IS FULL", "warning");
        unlink(tmp_pid);
        exit(1);
    } 
    unlink(tmp_pid);
    //altrimenti crea un thread e rimane in ascolto
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
    char *pathToS = (char*) malloc(20*sizeof(char));
    sprintf(tmp_pid,"%d",getpid());
    strcat(tmp_pid,"fifo_game_toC");
    printMessage("I am listening to questions from server..","warning");
    FIFO_game[0] = open(tmp_pid, O_RDONLY );
    
    sprintf(pathToS,"%d",getpid());
    strcat(pathToS,"fifo_game_toS");
    FIFO_game[1] = open(pathToS, O_WRONLY);
    
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
                else if((int)BUFFER[0] == 60)                           
                    //Controllo se il messaggio in arrivo inizia col carattere < 
                {                                                       
                    //cioè sto leggendo la classifica finale
                    printf("--------------------\n");
                    printMessage("Rank:","confirm");
                    printMessage(BUFFER,"confirm");
                    pthread_exit(NULL);
                }
                else if((int)BUFFER[0] == 83)                           
                    //Controllo se il messaggio in arrivo inizia col carattere S
                {                                                       
                    //cioè sto leggendo che il server è stato chiuso (STOP)
                    printMessage("Server interrupted","warning");
                    pthread_exit(NULL);
                }
                else
                {
                    //HO DATO LA RISPOSTA SBAGLIATA!
                    if(strcmp(BUFFER,"NO")==0){
                        printMessage("Wrong answer","error");
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

/*
funzione che prende in input la striga domanda, la mostra in output e attende la risposta dell'utente tramite fgets e la scrive in FIFO_game[1]
*/

void* QuestANDAnsw(char *domanda){
    char *risposta = (char*)malloc(4*sizeof(char));
    fgets(risposta,sizeof(risposta),stdin);
    strcat(risposta,"\0");
    write(FIFO_game[1],risposta, sizeof(risposta));
    free(risposta);
    return NULL;
}

//Se viene richiamata stampa il messaggio di warning e rimuove il client dalla lista giocatori
static void signal_handler(){
    printMessage("\nThe client closed the session\n","warning");
    write(FIFO_game[1],"STOP\n",5);
//    printMessage("\nIl CLIENT DI GIOCO VIENE CHIUSO","warning");
//    write(FIFO_game[1],"STOP\0",5);
    exit(-1);
}



