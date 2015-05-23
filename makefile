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
all: clean
	-@gcc -std=c99 -pthread -w src/main.c src/moduli/client.c src/moduli/client.h src/moduli/server.c src/moduli/server.h src/moduli/utilities.h src/moduli/utilities.c


#bin genera i binari compilati eseguibili dentro una cartella bin
bin: clean
	-@mkdir bin
	-@gcc -std=c99 -pthread -c src/moduli/client.c -o bin/client.o
	-@gcc -std=c99 -pthread -c src/moduli/server.c -o bin/server.o
	-@gcc -std=c99 -pthread -c src/moduli/utilities.c -o bin/utilities.o 
	-@gcc -std=c99 -pthread -w src/main.c bin/server.o bin/client.o bin/utilities.o -o bin/multiplayer.out



#assets genera dei file in input dentro la cartella assets
assets: clean


#test genera i binari e gli assets e lancia l’eseguibile per provare i file di input generati da assets
test: clean






#clean elimina i file temporanei, binari ed assets
.PHONY: clean
clean:
	-@rm -r -f bin
    
    
    
    
    
#classifica
#score: bin
#-@src/bin/main.out | grep "<PLAYER>\t<SCORE>\n"



#-@ls bin || mkdir bin
#if ! [ -d bin ]; then mkdir bin; fi
