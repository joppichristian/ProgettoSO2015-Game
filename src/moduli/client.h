#ifndef CLIENT_H
#define CLIENT_H

#include <fcntl.h>
#include <pthread.h>
#include <sys/errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
//-----------------DEFINIZIONI VARIABILI E STRUTTURE --------------------------
//FIFO giocatori
int FIFO_player_CL[2];

//FIFO partita
int FIFO_game[2];


//FILE Mode FIFO
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

//Thread di ascolto
pthread_t THascolto;

//-----------------------------------------------------------------------------

//-----------------DEFINIZIONE FIRME FUNZIONI client.c-------------------------

void init_client();

void *ascoltaServer();

void* QuestANDAnsw(char * domanda);

static void signal_handler();

//-----------------------------------------------------------------------------
#endif