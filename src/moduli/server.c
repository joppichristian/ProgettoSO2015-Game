#include "server.h"
#include "utilities.h"


//-------------------- DEFINIZIONE FUNZIONI ------------------------

//inizializza il server impostando il numero massimo di giocatori e il punteggio di vittoria
void init(int massimo,int ptg_vittoria){
    lock = 0; //variabile impostata a 1 appena un client risponde giusto 
    fine = 0; 
    strcpy(domanda,"");
    risposta = 0;
    
        //controlla che il numero di giocatori sia al massimo 10
    if(massimo < 1 || massimo > 10)
        printMessage("--MAX value not allowed, MIN:1 MAX:10","error"); 
    MAX = massimo;
    
        //controlla che il punteggio sia min 10 e max 100
    if(ptg_vittoria < 10 || ptg_vittoria > 100)
        printMessage("--WIN value not allowed, MIN:10 MAX:100","error"); 
    WIN = ptg_vittoria;
   
    
    if(signal(SIGINT, signal_handler) ==SIG_ERR)                        
        //Gestisco le Interrupt <CTRL-C> richiamando il signal_handler 
    {
        printMessage("SERVER ERROR","error");
        exit(-1);
    }
    
     if(signal(SIGHUP, signal_handler) == SIG_ERR)                        
        //Gestisco le QUIT tramite chiusura terminale richiamando il signal_handler 
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
        char tmp[255];   //Preparo la stringa da stampare all'apertura del server
        char tmp2[50];   //che contiene il numero max di giocatori ammessi e il 
        strcpy(tmp,"Starting server.. \nThe maximum number of player accepted is ");     //punteggio di vittoria.
        sprintf(tmp2,"%d",MAX);
        strcat(tmp,tmp2);
        strcat(tmp," and the winning score is ");
        sprintf(tmp2,"%d",WIN);
        strcat(tmp,tmp2);
        printMessage(tmp,"log");    //Stampo la stringa appena creata
        printMessage("\nWaiting for players..","log");
        sleep(1);
        if((FIFO_player = open("fifo_player",O_RDONLY))<0)
            printMessage("Error opening FIFO", "error");
        pthread_create(&THREAD_CONN,NULL,(void*)&listenPlayer,NULL);
        pthread_join(THREAD_CONN, NULL);
        
    }
    
    unlink("fifo_player");
}


//funzione che gestisce l'entrata di nuovi giocatori  

void *listenPlayer(){
    printMessage("Entering listener player..\nGame is starting..\n","confirm");
    char BUFFER [255];
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
            if(JOINED_PLAYER < MAX)
            {
                write(FIFO_player_ANSW,"OK\0",3);
                
                //assegno valori alle variabili dei giocatori
                strcpy(players[JOINED_PLAYER].pid,tmp);
                players[JOINED_PLAYER].punteggio = MAX-JOINED_PLAYER;
                players[JOINED_PLAYER].ritirato = 1;
                
                
                //Creo la FIFO per la risposta al server
                strcat(tmp,"fifo_game_toS");
                mkfifo(tmp,FILE_MODE);
                strcpy(pathFIFO_ToS[JOINED_PLAYER],tmp);
                if((players[JOINED_PLAYER].FIFO_game[0] = open(tmp,O_RDONLY | O_NONBLOCK))<0)
                    perror("FIFO_toS");
                //
                
                strcpy(tmp,players[JOINED_PLAYER].pid);
                
                //Creo la FIFO per l'invio della domanda al client
                strcat(tmp,"fifo_game_toC");
                mkfifo(tmp,FILE_MODE);
                strcpy(pathFIFO_ToC[JOINED_PLAYER],tmp);
                players[JOINED_PLAYER].FIFO_game[1] = open(tmp,O_WRONLY);
                //
                
                //CREA DOMANDA SE NON E' GIA' STATA FATTA
                if(strlen(domanda) ==0)
                    makeAsk();
                
                //CREO THREAD PER INVIO DOMANDE E ATTESA RISPOSTE CLIENT
                pthread_create(&THREAD_GAME[JOINED_PLAYER],NULL,(void*)&gestioneASKandANS,JOINED_PLAYER);
                
                JOINED_PLAYER++;
                ONLINE_PLAYER++;                
                printMessage("\nNew player joined the game!","log");
                char message_tmp[255];
                sprintf(message_tmp,"%d players online",ONLINE_PLAYER);
                printMessage(message_tmp,"log");         
            }
            else
            {
                printMessage("QUEUE IS FULL","error");
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
    char _risposta [60];
    char message [50];
    char message_tmp[255];
    players[giocatore].ritirato = 0;
    write(players[giocatore].FIFO_game[1],domanda,sizeof(domanda));
    //CICLO
    while(players[giocatore].punteggio < WIN ){
        strcpy(message,"");
        //ASPETTA RISPOSTA
        read(players[giocatore].FIFO_game[0],_risposta,sizeof(risposta));
        
        if(strlen(_risposta)!=0){
            //Controllo se il messaggio in arrivo inizia col carattere S cioè STOP (client chiuso)
            if((int)_risposta[0] == 83)                 
            {
                //CANCELLO LE FIFO CHE GESTISCONO IL GIOCO
                unlink(pathFIFO_ToS[giocatore]);
                unlink(pathFIFO_ToC[giocatore]);
                players[giocatore].ritirato = 1;
                ONLINE_PLAYER--;
                printMessage("Player left the game!","warning");
                
                sprintf(message_tmp,"%d players online", ONLINE_PLAYER);
                printMessage(message_tmp,"log");
                strcpy(message_tmp,"");
                
                if(JOINED_PLAYER == MAX && ONLINE_PLAYER ==0){
                    printMessage("Server is busy and all players are offline...game over!","warning");
                    fine = 1;
                }
                pthread_exit(NULL);
                
            }
            else if( (int)_risposta[0] > 47 && (int)_risposta[0] < 58)
            {
                int tmp =atoi(_risposta);
                //CONTROLLA RISPOSTA
                //SE SI --> BLOCCO GLI ALTRI E AUMENTO IL PUNTEGGIO E INVIO NUOVA DOMANDA
                if(tmp == risposta && lock != 1)
                {
                    //Segnalo che un client ha risposto giusto!
                    lock = 1;
                    //comunico che la risposta è corretta
                    sprintf(message_tmp,"Player %s answered correctly \n",players[giocatore].pid);
                    printMessage(message_tmp,"confirm");
                    strcpy(message_tmp,"");
                    //aumento punteggio
                    players[giocatore].punteggio +=1;
                    
                     
                    sprintf(message,"YES, right answer! Your score is %d! ",players[giocatore].punteggio);
                    write(players[giocatore].FIFO_game[1],message,sizeof(message)); //segnalo risposta giusta
                    
                    //controllo che il punteggio sia minore rispetto a WIN (punteggio di vittoria)
                    if(players[giocatore].punteggio < WIN){
                        makeAsk();
                        sleep(1);
                        //Invio la nuova domanda a tutti gli altri client!
                        for(int i=0;i<JOINED_PLAYER;i++){
                            if(players[i].ritirato != 1)
                                write(players[i].FIFO_game[1],domanda,sizeof(domanda));
                        }
                    }
                }
                else  //se la risposta è sbagliata
                {
                    players[giocatore].punteggio-=1; //diminuisco il punteggio
                    if(players[giocatore].punteggio == 0)
                    {
                        players[giocatore].ritirato = 1; 
                        write(players[giocatore].FIFO_game[1],"E\0",2); //segnalo eliminazione
                        sprintf(message_tmp,"Player %s lost the game!\n", players[giocatore].pid);
                        printMessage(message_tmp,"warning");
                        unlink(pathFIFO_ToS[giocatore]);
                        unlink(pathFIFO_ToC[giocatore]);
                        ONLINE_PLAYER--;
                        pthread_exit(NULL);
                        
                    }
                    sprintf(message,"NO, wrong answer! Your score is %d! ",players[giocatore].punteggio);
                    write(players[giocatore].FIFO_game[1],message,sizeof(message)); //segnalo risposta sbagliata
                    sprintf(message_tmp,"Player %s answered wrongly \n",players[giocatore].pid);
                    printMessage(message_tmp,"error");
                    strcpy(message_tmp,"");
                    
                   
                }
                lock=0;
            }
        }
        
        strcpy(_risposta,"");
    }
    //quando viene raggiunto il punteggio di vittoria stampo la classifica
    char classifica [2000];
    strcpy(classifica,makeClassifica());
    //scrivo la classifica da mandare
    sleep(1);
    for(int i=0;i<JOINED_PLAYER;i++)
        if(players[i].ritirato != 1)
            write(players[i].FIFO_game[1],classifica,255*JOINED_PLAYER);

    fine = 1;
    unlink("fifo_player");
    
    for(int i=0;i<JOINED_PLAYER;i++){
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
    strcat(domanda,"\0");
}

//funzione che crea la classifica da stampare al termine della partita
char* makeClassifica()
{
    char *classifica = (char*)malloc((sizeof(char)*20)*(JOINED_PLAYER+3));
    char tmp [4];
    strcpy(classifica,"<PLAYER>\t<SCORE>\n  ");
    orderClassifica();
    //concatena pid e punteggio raggiunto dal giocatore
    for(int i=0;i<JOINED_PLAYER;i++)
    {
        strcat(classifica,players[i].pid);
        strcat(classifica,"\t\t  ");
        sprintf(tmp,"%d",players[i].punteggio);
        strcat(classifica,tmp);
        strcat(classifica,"\n  ");
    }

    strcat(classifica,"\n\0");
    strcat(classifica,">>>>>>>>>THE WINNER IS PLAYER ");
    strcat(classifica,players[0].pid);
    strcat(classifica,"!!<<<<<<<<<\n\0");
    strcat(classifica,"\n\0");
    return classifica;
}

//gestore dei segnali...viene richiamato quando viene inviato un INTERRUPT o un QUIT del terminale
static void signal_handler(){
    printMessage("\nServer unexpectly closed","warning");
    unlink("fifo_player");
    for(int i=0;i<JOINED_PLAYER;i++)
        write(players[i].FIFO_game[1],"STOP\0",5);
    fine=1;
    for(int i=0;i<JOINED_PLAYER;i++){
        unlink(pathFIFO_ToC[i]);
        unlink(pathFIFO_ToS[i]);
    }
    exit(-1);
}

//funzione che scambia i valori
void swap (int a, int b){
    struct PLAYER tmp;
    tmp = players[a];
    players[a]=players[b];
    players[b]=tmp;
}

//funzione che ordina la classifica in modo descrescente secondo il punteggio
void orderClassifica(){
    
    for (int i=0;i<JOINED_PLAYER;i++){
        for (int j=i; j<JOINED_PLAYER; j++){
            if (players[i].punteggio < players[j].punteggio){
                swap (i,j);
            }
        }
    }
}



//-----------------------------------------------------------------------

