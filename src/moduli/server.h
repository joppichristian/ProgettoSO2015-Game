#ifndef SERVER_H
#define SERVER_H

#include <fcntl.h>
#include <pthread.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
//-----------------DEFINIZIONI VARIABILI E STRUTTURE --------------------------

//STRUTTURA CHE DEFINISCE UN PLAYER CON PID, PUNTEGGIO e RELATIVE FIFO
struct PLAYER{
    char pid [30];
    int punteggio;
    int FIFO_game [2]; 
    int ritirato;
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

//Giocatori online
int ONLINE_PLAYER;

//FILE Mode FIFO
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


//ID thread connessioni
pthread_t THREAD_CONN;


//ID thread gioco
pthread_t THREAD_GAME [10];

//ULTIMA DOMANDA INVIATA
char domanda [10];

//ULTIMA RISPOSTA CALCOLATA
int risposta;

//SEMAFORO CHE BLOCCA LE RISPOSTE GIUSTE NEL CASO NE SIA GIA STATA DATA UNA CORRETTA
int lock;

//1 QUANDO IL GIOCO TERMINA --> IMPONE AL THREAD DI ASCOLTO GIOCATORI DI TERMINARE
int fine;

//PATH FIFO_GAME TO S dei vari CLIENT
char pathFIFO_ToS[10][20];

//PATH FIFO_GAME TO C dei vari CLIENT
char pathFIFO_ToC[10][20];



//------------------------------------------------------------------------------

//-----------------DEFINIZIONE FIRME FUNZIONI server.c-------------------------

void init(int massimo,int ptg_vittoria);

void *listenPlayer();

void *gestioneASKandANS();

void makeAsk();

char* makeClassifica();

void swap();

void orderClassifica();

static void signal_handler();
//----------------------------------------------------------------------------


#endif