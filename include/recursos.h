
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define TAM 1024

#define X 3
#define Y 2
#define Z 4

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