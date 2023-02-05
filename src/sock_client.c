#include "../include/cliente.h"

int32_t sockfd, puerto;

void signal_handler(void){
	printf("cerrando conexión\n");
	close(sockfd);
	exit(1);
}

/*
	Función que se conecta al Delivery Managment vía socket y recibe mensajes
*/
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
int32_t main( int argc, char *argv[] ) {
	
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

	if (signal(SIGINT, signal_handler) == SIG_ERR) {			// crea el signal, el cual llamara al handler en caso de ingresar Ctrl+C
        fputs("Error al levantar el handler.\n", stderr);
        return EXIT_FAILURE;
    }

		while(1) {
			memset( buffer, 0, TAM );
			
			n = read( sockfd, buffer, TAM-1 );
			if ( n < 0 ) {
			  perror( "lectura de socket" );
			  exit(1);
			}
			
			//char* token;
			//token = strtok(buffer, " ");
			//char hash[strlen(token)];
			//sprintf(hash, "%s", token);
			//printf("%s\n", hash);

			//token = strtok(NULL, "\n");
			//char mensaje[strlen(token)];
			//sprintf(mensaje, "%s", token);
			//printf("%s\n", mensaje);
			

			printf( "%sRecibí: %s%s\n", KBLU,buffer,KNRM );
			
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