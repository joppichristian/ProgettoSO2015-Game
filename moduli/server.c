#include "server.h"
#include "utilities.h"


//-------------------- DEFINIZIONE FUNZIONI ------------------------

//inizializza il server impostando il numero massimo di giocatori e il punteggio di vittoria
void init(int massimo,int ptg_vittoria){
    lock = 0; //semaforo per modifica di variabili condivise
    fine = 0; //semaforo per modifica di variabili condivise
    strcpy(domanda,"");
    risposta = 0;
    
        //controlla che il numero di giocatori sia al massimo 10
    if(massimo < 1 || massimo > 10)
        printMessage("valore MAX non consentito MIN:1 MAX:10","error"); 
    MAX = massimo;
    
        //controlla che il punteggio sia min 10 e max 100
    if(ptg_vittoria < 10 || ptg_vittoria > 100)
        printMessage("valore WIN non consentito MIN:10 MAX:100","error"); 
    WIN = ptg_vittoria;
   
    
    if(signal(SIGINT, signal_handler) ==SIG_ERR)                        
        //Gestisco le Interrupt <CTRL-C> richiamando il signal_handler 
    {
        printMessage("SERVER ERROR","error");
        exit(-1);
    }
    
    //Creo la FIFO che comunica daClientVersoServer per l'inserimento dei giocatori in partita se non è già stata creata ( server già esistente ), in quel caso stama un messaggio di errore
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
        char tmp[255];   //Preparo la stringa da stampare all'apertura del server
        char tmp2[50];   //che contiene il numero max di giocatori ammessi e il 
        strcpy(tmp,"Il server viene avviato, si accetta un massimo di ");     //punteggio di vittoria.
        sprintf(tmp2,"%d",MAX);
        strcat(tmp,tmp2);
        strcat(tmp,"giocatori e si vince con un punteggio di ");
        sprintf(tmp2,"%d",WIN);
        strcat(tmp,tmp2);
        printMessage(tmp,"log");    //Stampo la stringa appena creata
        pthread_join(THREAD_CONN, NULL);
        
    }
    
     
    
    
    
    unlink("fifo_player");
}

void *listenPlayer(){
    printMessage("Entering listener player\n","confirm");
    char BUFFER [255];
    char* tmp;
    int FIFO_player_ANSW;
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
                write(FIFO_player_ANSW,"OK\0",3);

                //MUTEX LOCK
                pthread_mutex_lock(&PLAYER_MUTEX);
                
                strcpy(players[ACTIVE_PLAYER].pid,tmp);
                players[ACTIVE_PLAYER].punteggio = MAX-ACTIVE_PLAYER;
                players[ACTIVE_PLAYER].ritirato = 1;
                //Creo la FIFO per la risposta al server
                strcat(tmp,"fifo_game_toS");
                mkfifo(tmp,FILE_MODE);
                strcpy(pathFIFO_ToS[ACTIVE_PLAYER],tmp);
                if((players[ACTIVE_PLAYER].FIFO_game[0] = open(tmp,O_RDONLY | O_NONBLOCK))<0)
                    perror("FIFO_toS");
                //
                
                strcpy(tmp,players[ACTIVE_PLAYER].pid);
                
                //Creo la FIFO per l'invio della domanda al client
                strcat(tmp,"fifo_game_toC");
                mkfifo(tmp,FILE_MODE);
                strcpy(pathFIFO_ToC[ACTIVE_PLAYER],tmp);
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

//funzione che gestisce domande e risposte

void *gestioneASKandANS(int giocatore){
   //INVIA DOMANDA
    char _risposta [30];
    players[giocatore].ritirato = 0;
    pthread_mutex_lock(&PLAYER_MUTEX);
    write(players[giocatore].FIFO_game[1],domanda,sizeof(domanda));
    pthread_mutex_unlock(&PLAYER_MUTEX);
    //CICLO
    while(players[giocatore].punteggio < WIN){
        //ASPETTA RISPOSTA
        while(read(players[giocatore].FIFO_game[0],_risposta,sizeof(risposta))==0);
        
        if(strlen(_risposta)!=0){
            //Controllo se il messaggio in arrivo inizia col carattere S cioè STOP (client chiuso)
            if((int)_risposta[0] == 83)                 
            {
                //CANCELLO LE FIFO CHE GESTISCONO IL GIOCO
                unlink(pathFIFO_ToS[giocatore]);
                unlink(pathFIFO_ToC[giocatore]);
                players[giocatore].ritirato = 1;
                printMessage("Giocatore perso","error");
                pthread_exit(NULL);
                
            }
            else
            {
                
                printMessage(_risposta,"error");
                int tmp =atoi(_risposta);
                //CONTROLLA RISPOSTA
                //SE SI --> BLOCCO GLI ALTRI E AUMENTO IL PUNTEGGIO E INVIO NUOVA DOMANDA
                if(tmp == risposta && lock != 1)
                {
                    //Segnalo che un client ha risposto giusto!
                    pthread_mutex_lock(&PLAYER_MUTEX);
                    lock = 1;
                    //comunico che la risposta è corretta
                    printf("Il giocatore %s ha risposto giusto \n",players[giocatore].pid);
                    //aumento punteggio
                    players[giocatore].punteggio +=1;
                    //controllo che il punteggio sia minore rispetto a WIN (punteggio di vittoria)
                    if(players[giocatore].punteggio < WIN){
                        makeAsk();

                        //Invio la nuova domanda a tutti gli altri client!
                        for(int i=0;i<ACTIVE_PLAYER;i++)
                            if(players[i].ritirato != 1)
                                write(players[i].FIFO_game[1],domanda,sizeof(domanda));
                        pthread_mutex_unlock(&PLAYER_MUTEX);
                    }
                }
                else  //se la risposta è sbagliata
                {
                    pthread_mutex_lock(&PLAYER_MUTEX); //blocco l'accesso alle variabili che modifico tra lock e unlock
                    players[giocatore].punteggio-=1; //diminuisco il punteggio
                    write(players[giocatore].FIFO_game[1],"NO\0",3); //segnalo risposta sbagliata
                    pthread_mutex_unlock(&PLAYER_MUTEX); //sblocco l'accesso alle variabili che modifico tra lock e unlock
                    printf("Il giocatore %s ha risposto sbagliato \n",players[giocatore].pid);
                }
                lock=0;
            }
        }
        
        strcpy(_risposta,"");
    }
    
    //quando viene raggiunto il punteggio di vittoria stampo la classifica
    char *classifica = (char*)malloc((sizeof(char)*20)*(ACTIVE_PLAYER+2));
    classifica = makeClassifica();
    printf("Classifica: \n%s",classifica);
    pthread_mutex_lock(&PLAYER_MUTEX);
    //scrivo la classifica da mandare
    for(int i=0;i<ACTIVE_PLAYER;i++)
        if(players[i].ritirato != 1)
            write(players[i].FIFO_game[1],classifica,sizeof(classifica));
    pthread_mutex_unlock(&PLAYER_MUTEX);
    free(classifica);
    fine = 1;
    unlink("fifo_player");
    for(int i=0;i<ACTIVE_PLAYER;i++){
        unlink(pathFIFO_ToC[i]);
        unlink(pathFIFO_ToS[i]);
    }
    pthread_exit(NULL);
}

//funzione che crea la domanda unendo in una stringa due interi random
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

//funzione che crea la classifica da stampare al termine della partita
char* makeClassifica()
{
    char *classifica = (char*)malloc((sizeof(char)*20)*(ACTIVE_PLAYER+2));
    char tmp [4];
    strcpy(classifica,"<PLAYER>\t<PUNTEGGIO>\n  ");
    pthread_mutex_lock(&PLAYER_MUTEX);
    //concatena pid e punteggio raggiunto dal giocatore
    for(int i=0;i<ACTIVE_PLAYER;i++)
    {
        strcat(classifica,players[i].pid);
        strcat(classifica,"\t\t    ");
        sprintf(tmp,"%d",players[i].punteggio);
        strcat(classifica,tmp);
        strcat(classifica,"\n  ");
    }
    strcat(classifica,"\n\0");
    pthread_mutex_unlock(&PLAYER_MUTEX);
    return classifica;
}

static void signal_handler(){
    printMessage("\nIL SERVER VIENE CHIUSO","warning");
    unlink("fifo_player");
    pthread_mutex_lock(&PLAYER_MUTEX);
    for(int i=0;i<ACTIVE_PLAYER;i++)
        write(players[i].FIFO_game[1],"STOP\0",5);
    fine=1;
    pthread_mutex_unlock(&PLAYER_MUTEX);
    for(int i=0;i<ACTIVE_PLAYER;i++){
        unlink(pathFIFO_ToC[i]);
        unlink(pathFIFO_ToS[i]);
    }
    exit(-1);
}
//-----------------------------------------------------------------------

