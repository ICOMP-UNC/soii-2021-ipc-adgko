#include "../include/recursos.h"

int32_t main(){
    printf("Soy el Productor 3\n");
    char buffer[TAM];

    while(1){
        double load_avg[1];
        getloadavg(load_avg, 1);
        sprintf(buffer, "%f", load_avg[0]);
        //printf("%s",buffer);
        sleep(1/Z);
    }

    return 0;
}