#include "../include/cliente.h"
#define puerto_downloader 8021
#define puerto_files 8020

int32_t sockfd, sockfil;
struct sockaddr_in serv_addr;

uint32_t client_len; // tamaño de la dirección del cliente
char buffer[TAM];
ssize_t n;     // hubo que declarar n como ssize_t para que no pierda información al usarse en send() y recv()
long *logsize; // para guardar size de log
char *logmd5;  // para guardar md5 de log
int32_t main()
{

    configurar_socket(); // configura y abre el socket para recibir

    conect_to_files();

    // En el cliente (downloader)
    printf("Intentando conectar al servidor en el puerto %d\n", puerto_files);

    memset(buffer, 0, TAM);

    n = recv(sockfil, buffer, TAM - 1, 0);
    if (n < 0)
    {
        perror("lectura de socket");
        exit(1);
    }

    printf("%sRecibí: %s%s\n", KBLU, buffer, KNRM);

    memset(buffer, 0, TAM);

    // recibe tamaño
    n = recv(sockfil, buffer, TAM - 1, 0);
    if (n < 0)
    {
        perror("lectura de socket");
        exit(1);
    }

    printf("%sRecibí: %s%s\n", KBLU, buffer, KNRM);

    // envía confirmaciòn
    n = send(sockfil, "mandame el hash", strlen("mandame el hash"), 0);
    if (n < 0)
    {
        perror("fallo en enviar info");
    }

    memset(buffer, 0, TAM);

    //recibe hash md5
    n = recv(sockfil, buffer, TAM - 1, 0);
    if (n < 0)
    {
        perror("lectura de socket");
        exit(1);
    }

    printf("%sRecibí: %s%s\n", KBLU, buffer, KNRM);
}

void configurar_socket()
{
    memset((char *)&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((uint16_t)puerto_files);
    struct hostent *server;
    server = gethostbyname("localhost");
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t)server->h_length);
}

/*
    Es lo mismo que connect_to_server, pero para file,
    así le pasa el archivo que quiere descargar por otro socket distinto
    al que usa para enviar comandos
*/
void conect_to_files()
{
    sockfil = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfil < 0)
    {
        perror("ERROR apertura de socket");
        exit(1);
    }

    if (connect(sockfil, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("conexion conectando");
        exit(1);
    }
    printf("conectado \n");
}
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define puerto_files    8020
#define TAM 1024

int32_t sockfd;
struct sockaddr_in serv_addr;
char buffer[TAM];

int main() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al abrir el socket");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(puerto_files);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Dirección no válida");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error al conectar");
        exit(EXIT_FAILURE);
    }

    printf("Conectado al servidor. Esperando mensaje...\n");

    ssize_t n = recv(sockfd, buffer, TAM - 1, MSG_WAITALL);
    if (n < 0) {
        perror("Error al leer del socket");
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';
    printf("Recibido: %s\n", buffer);

    close(sockfd);
    return 0;
}*/