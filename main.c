#include <stdio.h>
#include <unistd.h>
#include <error>


int main(int argc,char** argv)
{       
    if(argc< 2)
    {
            perror("Arguments Error: use %s <server|client> --optional_args",argv[0]);
            exit(1);
    }
   
    #Legge argomento obbligatorio e decide se il processo corrente sarà il server o un client del gioco.
    switch(argv[1]){
        case "server": 
            break;
        case "client":
            break;
        default:
            perror("Arguments Error: use %s <server|client> --optional_args",argv[0]);
            exit(1);
    }
    
    exit(0);
}
