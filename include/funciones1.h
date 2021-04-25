#include "../include/recursos.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <openssl/md5.h>

/*
	Elementos empleados para crear y usar la cola de mensajes
*/
#define QUEUE_NAME 			"../archivos/queue"
#define UNIQUE_KEY 			65
#define PROJ_ID 			   0777                    //permisos para todos
#define direccion_server 	"src/sock_server.c"

/*
	Estructura del buffer de la cola de mensajes
*/
struct msgbuf {
   long mtype;
   char mtext[TAM];
};

/*
int32_t get_queue();
int32_t send_to_queue(long, char [TAM] );
char* recive_from_queue(long , int32_t );
*/