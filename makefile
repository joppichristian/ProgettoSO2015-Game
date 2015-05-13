default:
	@echo "MULTIPLAYER GAME"
	@echo ""
	@echo "ACCEPTED COMMANDS:"  //dobbiamo scegliere e inserire i comandi del gioco
	@echo ""
	@echo ""
	@echo "MAKEFILE COMMANDS:"
	@echo "you can use the following options:"
	@echo ""
	@echo "-without target it generates a brief description and a list of make options"
	@echo "-"make bin" generates compiled executable binary in a folder (automatically generated) named "bin" that contains only executable files"
	@echo "-"make assets" generates possible input files "
	@echo "	in an automatically generated folder named "assets""
	@echo "-"make test" generates binary files and assets "
	@echo "	and runs executable for testing generated input files"
	@echo "-"make clean" deletes temporary files, binary files "
	@echo "	and assets (automatically recalled by every other instruction)"

all:
	gcc main.c moduli/client.c moduli/client.h moduli/server.c moduli/server.h moduli/utilities.h moduli/utilities.c
bin:
	gcc -c -o /luisazurlo/GitHub/ProgettoSO2015-Game/bin/

assets:

test:

clean:
	rm fifo_player

