#include "recursos.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ioctl.h> //para poner no bloqueante
#include <sys/poll.h> 

/*
    Paths de los binarios productores y CLI
*/
#define PROD1_PATH "prod1"
#define PROD2_PATH "prod2"
#define PROD3_PATH "prod3"
#define CLI_PATH    "cli"
/*
    Máxima cantidad de clientes soportada
*/
#define MAX_CLIENTES 5000

/*
    Constantes para determinar tiempo antes de cerrar la conexión
*/
#define MINUTES     3
#define SECONDS     60
#define MILISECONDS 1000