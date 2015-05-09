all:
	gcc main.c moduli/client.c moduli/client.h moduli/server.c moduli/server.h moduli/utilities.h moduli/utilities.c
	rm fifo_player

