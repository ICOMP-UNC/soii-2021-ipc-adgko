
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
#define ID_PROD1    1
#define ID_PROD2    2
#define ID_PROD3    3
#define ID_CLI      4
#define ID_FAIL_ADD 5
#define ID_FAIL_DEL 5

/*
    Etiquetas para los colores de los mensajes
*/
#define GREAT   1
#define OK      2
#define WARNING 3
#define ERROR   4

/*
    Tags para cerrar las conexiones
*/
#define TRUE    1
#define FALSE   0

/*
	Variables empleadas para imprimir en colores
*/
#define KNRM  "\x1B[0m"	    //normal
#define KRED  "\x1B[31m"	//rojo
#define KGRN  "\x1B[32m"	//verde
#define KYEL  "\x1B[33m"	//amarillo
#define KBLU  "\x1B[34m"	//azul
#define KMAG  "\x1B[35m"	//magenta
#define KCYN  "\x1B[36m"	//cyan
#define KWHT  "\x1B[37m"	//blanco

/*
    Si no se declaran acá no las reconoce
*/
int32_t get_queue();
int32_t send_to_queue(long, char [TAM] );
char* recive_from_queue(long , int32_t );


/*
    Estructura de los nodos de la lista
*/
struct lista { /* lista simple enlazada */
  int32_t fd;
  char* ip;
  int32_t port;
  struct lista *sig;
};
/*
    funciones de lista
*/
int longitudl(struct lista *l);
struct lista *creanodo(void);
struct lista *insertafinal(struct lista *l, int32_t a,char* b,int32_t c);
struct lista *elimina(struct lista *p, char* a,int32_t b);
void ImprimirElementosLista (struct lista *a);