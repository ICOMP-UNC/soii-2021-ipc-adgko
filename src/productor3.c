#include "../include/recursos.h"

int32_t main(){
    printf("Soy el Productor 3\n");
    char buffer[TAM];
    if(buffer == NULL){
        printf("%sError alocando memoria%s\n",KRED,KNRM);
		    exit(1);
	}
    while(1){
        double load_avg[1];
        getloadavg(load_avg, 1);
        sprintf(buffer, "%f", load_avg[0]);
        //printf("%s",buffer);
        send_to_queue((long) ID_PROD3, &buffer[0]);
        sleep(1/Z);
    }

    return 0;
}