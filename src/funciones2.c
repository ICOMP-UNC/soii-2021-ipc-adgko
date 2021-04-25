#include "../include/recursos.h"

/* 
  Devuelve la longitud de una lista 
*/
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
}

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