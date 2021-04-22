#include "../include/recursos.h"

int32_t main(){
    printf("%sCLI en linea%s\n",KGRN,KNRM);

    char buffer[TAM];
   	while(1){
        memset(buffer, 0, TAM);			// con esto limpio el buffer del comando anterior
        fgets(buffer,TAM,stdin); 			// pido comando
        if(buffer[0] != '\n'){				//si en el buffer no hay una nueva linea, envía
            printf("%s\n",buffer);
            send_to_queue((long) ID_CLI, &buffer[0]);
            printf("BBBBBB\n");
        }else{
            break;
        }
    }
}

