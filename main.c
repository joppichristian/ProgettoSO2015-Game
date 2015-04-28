#include <stdio.h>
#include <unistd.h>
#include <error>


int main(int argc,char** argv)
{       
        if(argc!=2)
        {
                perror("Error: using %s <server|client>",argv[0]);
                exit(1);
        }

        exit(0);
}
