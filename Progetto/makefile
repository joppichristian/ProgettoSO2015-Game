#default stampa nel terminale le i comandi accettati dal programma e dal makefile

default:
	@echo ""
	@echo "MULTIPLAYER GAME"
	@echo ""
	@echo ""
	@echo "ACCEPTED COMMANDS:"
	@echo ""
	@echo "./a.out server --max <max> --win <win>"
	@echo "Allows to insert the max number of player and the max winning points"
	@echo ""
	@echo ""
	@echo "MAKEFILE COMMANDS:"
	@echo "you can use the following options:"
	@echo ""
	@echo "-without target it generates a list with description of make options"
	@echo "-"make bin" generates executable binary in an automatically generated folder named "bin" "
	@echo "-"make assets" generates possible input files in an automatically generated folder named "assets" "
	@echo "-"make test" generates binary files and assets and runs executable for testing generated input files"
	@echo "-"make clean" deletes temporary files, binary files and assets (automatically recalled by every other instruction)"
	@echo ""
	@echo ""

#all compila tutti i file

all:
	#gcc main.c moduli/client.c moduli/client.h moduli/server.c moduli/server.h moduli/utilities.h moduli/utilities.c
	gcc ../main.c ../moduli/client.c ../moduli/client.h ../moduli/server.c ../moduli/server.h ../moduli/utilities.h ../moduli/utilities.c

bin:
	if [ -a bin ]; then mkdir bin; fi;
	gcc -c -o /bin/game.o moduli/*.c moduli/*.h main.c

assets:

test:

clean:
	rm fifo_player

