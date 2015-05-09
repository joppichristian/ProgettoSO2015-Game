#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void main () {
    srand(time(NULL));
    int random = rand() %100;
    printf("%d\n",random);
    
    //inserisce dove (?) somma di due numeri casuali
}

