#default/senza target stampa nel terminale le i comandi accettati dal programma e dal makefile

default:
	@echo ""
	@echo "MULTIPLAYER GAME"
	@echo ""
	@echo ""
	@echo "MAKEFILE COMMANDS:"
	@echo "you can use the following options:"
	@echo ""
	@echo "-without target it generates a list with description of make options"
	@echo "-"make bin" generates executable binary in an automatically generated folder named "bin" "
	@echo "-"make assets" generates possible input files in an automatically generated folder named "assets" "
	@echo "-"make test" generates binary files and assets and runs executable for testing generated input files"
	@echo "-"make clean" deletes temporary files, binary files and assets"
	@echo ""
	@echo ""


#all compila tutti i file

all:
	gcc ../main.c ../moduli/client.c ../moduli/client.h ../moduli/server.c ../moduli/server.h ../moduli/utilities.h ../moduli/utilities.c


#bin genera i binari compilati eseguibili dentro una cartella bin
bin:
	#make clean
	if [ -a bin ]; then mkdir bin; fi;
	gcc -c -o /bin/game.o moduli/*.c moduli/*.h main.c


#assets genera dei file in input dentro la cartella assets
assets:
	#make clean


#test genera i binari e gli assets e lancia l’eseguibile per provare i file di input generati da assets
test:
	#make clean


#clean elimina i file temporanei, binari ed assets
clean:
	rm fifo_player

