#ifndef SERVER_H
#define SERVER_H

#include <fcntl.h>
#include <pthread.h>
//-----------------DEFINIZIONI VARIABILI E STRUTTURE --------------------------

//STRUTTURA CHE DEFINISCE UN PLAYER CON PID, PUNTEGGIO e RELATIVE FIFO
struct PLAYER{
    char pid [30];
    int punteggio;
    int FIFO_game [2]; 
};

//Numero massimo di giocatori collegati al server 
int MAX;

//Punteggio di vittoria MIN=10 e MAX=100
int WIN;

//Giocatori attivi sul server
int ACTIVE_PLAYER;

//Lista dei giocatori attivi
struct PLAYER players[10];

//FIFO Giocatori
int FIFO_player;

//FILE Mode FIFO
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


//ID thread connessioni
pthread_t THREAD_CONN;

//MUTEX sui player
pthread_mutex_t PLAYER_MUTEX;

//ULTIMA DOMANDA INVIATA
char domanda [10];

//ULTIMA RISPOSTA CALCOLATA
int risposta;

//SEMAFORO CHE BLOCCA LE RISPOSTE GIUSTE NEL CASO NE SIA GIA STATA DATA UNA CORRETTA
int lock;


//------------------------------------------------------------------------------

//-----------------DEFINIZIONE FIRME FUNZIONI server.c-------------------------

int init(int massimo,int ptg_vittoria);

void *listenPlayer();

void *gestioneASKandANS();

void makeAsk();

//----------------------------------------------------------------------------


#endif