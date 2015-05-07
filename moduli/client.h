#ifndef CLIENT_H
#define CLIENT_H

#include <fcntl.h>
//-----------------DEFINIZIONI VARIABILI E STRUTTURE --------------------------
//FIFO giocatori
int FIFO_player_CL[2];

//FIFO partita
int FIFO_game[2];

//FILE Mode FIFO
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
//-----------------------------------------------------------------------------

//-----------------DEFINIZIONE FIRME FUNZIONI client.c-------------------------

int init_client();

void *ascoltaServer();

char* QuestANDAnsw(char domanda);

//-----------------------------------------------------------------------------
#endif