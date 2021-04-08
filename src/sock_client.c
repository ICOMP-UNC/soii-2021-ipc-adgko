#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define TAM 256

int main( int argc, char *argv[] ) {
	int32_t sockfd, puerto;
	struct sockaddr_in serv_addr;
	struct hostent *server;
    ssize_t n;

	char buffer[TAM];
	if ( argc < 3 ) {
		fprintf( stderr, "Uso %s host puerto\n", argv[0]);
		exit( 0 );
	}

	puerto = atoi( argv[2] );
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );

	server = gethostbyname( argv[1] );

	memset( (char *) &serv_addr, '0', sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t )server->h_length );
	serv_addr.sin_port = htons( (uint16_t)puerto );
	if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "conexion" );
		exit( 1 );
	}

		while(1) {
			memset( buffer, 0, TAM );
			
			n = read( sockfd, buffer, TAM-1 );
			if ( n < 0 ) {
			  perror( "lectura de socket" );
			  exit(1);
			}
			
			printf( "PROCESO %d. ", getpid() );
			printf( "Recibí: %s", buffer );
			
			n = write( sockfd, "Obtuve su mensaje", 18 );
			if ( n < 0 ) {
			  perror( "escritura en socket" );
			  exit( 1 );
			}
			// Verificación de si hay que terminar
			buffer[strlen(buffer)-1] = '\0';
			if( !strcmp( "fin", buffer ) ) {
			  printf( "PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid() );
			  exit(0);
			}

		}

	return 0;
} 
