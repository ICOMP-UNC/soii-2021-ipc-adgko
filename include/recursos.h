
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define TAM 1024

/*
    Valores empleados para el calculo de tasa de envío de mensajes
    en los productores
*/
#define X 3
#define Y 2
#define Z 4

/*
    etiquetas de los productores para la cola de mensajes
*/
#define ID_PROD1 1
#define ID_PROD2 2
#define ID_PROD3 3

/*
	Variables empleadas para imprimir en colores
*/
#define KNRM  					"\x1B[0m"	//normal
#define KRED  					"\x1B[31m"	//rojo
#define KGRN  					"\x1B[32m"	//verde
#define KYEL  					"\x1B[33m"	//amarillo
#define KBLU  					"\x1B[34m"	//azul
#define KMAG  					"\x1B[35m"	//magenta
#define KCYN  					"\x1B[36m"	//cyan
#define KWHT  					"\x1B[37m"	//blanco

/*
    Si no se declaran acá no las reconoce
*/
int32_t get_queue();
int32_t send_to_queue(long, char [TAM] );
char* recive_from_queue(long , int32_t );