#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define X 2

int32_t main(){
    printf("Soy el Productor 1\n");
    int32_t numero;

    while(1){
        numero = rand();
        printf("El numero enviado es %d\n",numero);
        sleep(1/X);
    }
}