#include "../include/funciones2.h"
#include <time.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                  LISTAS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 
  Devuelve la longitud de una lista 
*/
/*
int longitudl(struct lista *l) {
  struct lista *p;
  int n;
  n = 0;
  p = l;
  while (p != NULL) {
    ++n;
    p = p->sig;
  }
  return n;
}*/

/*
    Crea el nodo principal que apunta al primer elemento de la lista
*/
struct lista *creanodo(void) {
  return (struct lista *) malloc(sizeof(struct lista));
}

      
/* 
  Inserta dato al final de la lista (para colas) 
  a es el file descriptor
  b la ip
  c el puerto
*/
struct lista *insertafinal(struct lista *l, int32_t a,char* b,int32_t c) {
  struct lista *p,*q;
  q = creanodo(); /* crea un nuevo nodo */
  q->fd = a;      /* copiar los datos */
  q->ip = b;      /* copiar los datos */
  q->port = c;    /* copiar los datos */
  q->subs_1 = 0;
  q->subs_2 = 0;
  q->subs_3 = 0;
  q->desconectado = 0;      
  q->sig = NULL;
  if (l == NULL){
    return q;
  }
  /* la lista argumento no es vacía. Situarse en el último */
  p = l;
  while (p->sig != NULL){   
    p = p->sig;
  }
  p->sig = q;
  return l;
}

/*
  Elimina un nodo entero de la lista y enlaza los nodos que están antes y después de ese,
  en caso de que haya alguno
*/
struct lista *elimina(struct lista *p, char* a,int32_t b) {
  if (p == NULL) /* no hay nada que borrar */
    return p;
  /* compara el dato */
  if((strcmp(p->ip, a) == 0 && p->port == b)){
    struct lista *q;
    q = p->sig;
    free(p); /* libera la memoria y hemos perdido el enlace, por eso se guarda en q */
    p = q; /* restaurar p al nuevo valor */
    return p;
  }else{ /* no encontrado */
    p->sig = elimina(p->sig,a,b); /* recurre */
    return p;
  }
}

void ImprimirElementosLista (struct lista *a)
{
struct lista  *aux = a;
int i = 0;
while (aux != NULL)
{
printf("\n %d: %d %s %d\n", i++,aux->fd,aux->ip,aux->port);
aux = aux->sig;
}
if (aux == NULL)
printf("\n\nSe ha llegado al final de la lista\n\n");
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                  LOG
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
  La función recibe el productor y el mensaje
  Dependientdo el productor, guarda en logs distintos
*/
void imprimir_log(int productor, char* mensaje,char* ip,int32_t puerto)
  {
  	char hora[TAM_HORA];
    get_time(hora);
  /*
     guardo de quien es el log
     lo voy a necesitar para crear el archivo
  */
    char propietario[TAM_SERV];
    if(productor == LOG_PROD_1)
      sprintf(propietario, "productor_1");
    else if(productor == LOG_PROD_2)
      sprintf(propietario, "productor_2");
    else if(productor == LOG_PROD_3)
      sprintf(propietario, "productor_3");
    else if(productor == LOG_CLI)
      sprintf(propietario, "cli");
    else
      sprintf(propietario, "err");

    char log[TAM];
    sprintf(log, "%s | %s | %s | %s:%d \n", hora, propietario, mensaje,ip,puerto);
    //printf("%s", log);

    /*
      con el propietario, puedo hacer el log
      a+ me habilita a escribir el archivo al final
      y si no existe, crearlo
    */
    char log_file[TAM];
    sprintf(log_file, "../archivos/logs/%s.log", propietario);
    FILE * file = fopen(log_file, "a+");
    if (file == NULL)
      {
        char msg[TAM*2];
        sprintf(msg, "ERROR %s", log_file);
        perror(msg);
        return;
      }
    fprintf(file, "%s", log);
    fclose(file);
  }

/*
  Esta función guarda el tiempo a modo de timestamp
  con lo que puedo definir a que hora se ocasionó el evento
  le paso por parámetro el buffer donde guarda la hora 
*/
void get_time(char * tiempo)
  {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(tiempo, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900,
  	 tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  }

/*
  función tomada de la documentación suministrada por la cátedra de libzip
  esta función toma un directorio, recursivamente lo va comprimiendo y lo guarda en un archivo

  void zip_walk(struct zip_t *zip, const char *path) {
    DIR *dir;
    struct dirent *entry;
    char fullpath[TAM];
    struct stat s;

    memset(fullpath, 0, TAM);
    dir = opendir(path);
    assert(dir);

    while ((entry = readdir(dir))) {
      // skip "." and ".."
      if (!strcmp(entry->d_name, ".\0") || !strcmp(entry->d_name, "..\0"))
        continue;

      snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
      stat(fullpath, &s);
      if (S_ISDIR(s.st_mode))
        zip_walk(zip, fullpath);
      else {
        zip_entry_open(zip, fullpath);
        zip_entry_fwrite(zip, fullpath);
        zip_entry_close(zip);
      }
    }

    closedir(dir);
}*/
