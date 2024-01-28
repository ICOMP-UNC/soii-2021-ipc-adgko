#include "../include/cliente.h"
#define puerto_downloader    8021
#define puerto_files    8020

int32_t sockfd,sockfil;
struct sockaddr_in serv_addr,serv_addr_file;

uint32_t client_len;				//tamaño de la dirección del cliente
char buffer[TAM];
ssize_t n;							// hubo que declarar n como ssize_t para que no pierda información al usarse en send() y recv()
long *logsize;							//para guardar size de log
char *logmd5;							//para guardar md5 de log
int32_t main() {

    

    configurar_socket();	// configura y abre el socket para recibir

    conect_to_files(); 

    // En el cliente (downloader)
    printf("Intentando conectar al servidor en el puerto %d\n", puerto_files);

				n = recv( sockfil, buffer, TAM-1, MSG_WAITALL );
				if ( n < 0 ) {
			 	  perror( "lectura de socket" );
				  exit(1);
				}
				
				printf( "%sRecibí: %s%s\n", KBLU,buffer,KNRM );

}

void configurar_socket() {
	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
    
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons( (uint16_t) puerto_downloader);
	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "conexion configurando" );
		exit(1);
	}
}

/*
	Es lo mismo que connect_to_server, pero para file, 
	así le pasa el archivo que quiere descargar por otro socket distinto
	al que usa para enviar comandos
*/
void conect_to_files(){
	sockfil = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfil < 0 ) {
		perror( "ERROR apertura de socket" );
		exit( 1 );
	}

	if ( connect( sockfil, (struct sockaddr *)&serv_addr_file, sizeof(serv_addr_file ) ) < 0 ) {
		perror( "conexion conectando" );
		exit( 1 );
	}
	printf("conectado \n");
}