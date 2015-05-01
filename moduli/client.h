#ifndef CLIENT_H
#define CLIENT_H

#include <fcntl.h>

//FIFO giocatori
int FIFO_player[2];

//FIFO partita
int FIFO_game[2];

//FILE Mode FIFO
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int init_client();

void ascoltaServer();

#endif