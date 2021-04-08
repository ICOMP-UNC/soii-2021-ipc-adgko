#include "../include/recursos.h"

#define SERVER_OK 			0
#define SERVER_ER 			1
#define linea_free_memory   1


int32_t main(){

    printf("Soy el Productor 2\n");

    while(1){
        
        char buffer[TAM];
        char * path = "/proc/meminfo";
        FILE* file = fopen(path, "r");
        if( file == NULL)
        {
            char mensaje[TAM];
            sprintf(mensaje, "ERROR: %s - %s\n", path, strerror(errno));
            printf("%s", mensaje);
            exit(1);
        }

        char linea[TAM];
        for( int i = 0; i <= linea_free_memory; i++)
        fgets(linea, TAM, file);

        char * aux = strtok(linea, ":");
        aux = strtok(NULL, ":");
        int buffer_index = 0;
        for( size_t i = 0; i < strlen(aux); i++ )
        {
            if( aux[i] == 'k' )
            {
                buffer[buffer_index] = '\0';
                break;
            }
            else if( aux[i] != ' ')
            {
                buffer[buffer_index] = aux[i];
                buffer_index++;
            }
        }
        fclose(file);

        long meminfo = strtol(buffer, NULL, 10);
        meminfo = meminfo / 1024;
        sprintf(buffer, "%ld MB", meminfo);
        //printf("%s\n",buffer);
        sleep(1/Y);
        }

    return 0;
}