all:
	gcc main.c moduli/server.c moduli/server.h moduli/client.c moduli/client.h
	rm fifo_player
