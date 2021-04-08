#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    printf("Soy el Productor 1\n");
    int32_t numero, t = 1;

    while(t){
        numero = rand();
        printf("El numero enviado es %d\n",numero);
    }
}