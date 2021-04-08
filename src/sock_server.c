#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#define TAM 256

#define PROD1_PATH "prod1"
#define PROD2_PATH "prod2"
#define PROD3_PATH "prod3"


int main( int argc, char *argv[] ) {
	int32_t sockfd, newsockfd, puerto, pid;
    uint32_t clilen;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t n;
	int32_t terminar = 0;
    int32_t pid_prod1, pid_prod2, pid_prod3;

	if ( argc < 2 ) {
        	fprintf( stderr, "Uso: %s <puerto>\n", argv[0] );
		exit( 1 );
	}

    pid_prod1 = fork();
  		if ( pid_prod1 == 0 ) {
    		if( execv(PROD1_PATH, argv) == -1 ) {
   			  perror("error en auth ");
   			  exit(1);
   			} 			
   			exit(0);
  		}

  	pid_prod2 = fork();
  		if ( pid_prod2 == 0 ) {
    		if( execv(PROD2_PATH, argv) == -1 ) {
   			  perror("error en file ");
   			  exit(1);
   			}   			
   			exit(0);
		}

    pid_prod3 = fork();
  		if ( pid_prod3 == 0 ) {
    		if( execv(PROD3_PATH, argv) == -1 ) {
   			  perror("error en file ");
   			  exit(1);
   			}   			
   			exit(0);
		}


	sockfd = socket( AF_INET, SOCK_STREAM, 0);

	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
	puerto = atoi( argv[1] );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( (uint16_t)puerto );

	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "ligadura" );
		exit( 1 );
	}

        printf( "Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	listen( sockfd, 5 );
	clilen = sizeof( cli_addr );

	while( 1 ) {
		newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );

		pid = fork(); 

		if ( pid == 0 )
		  {  // Proceso hijo
		    close( sockfd );

		while(1) {
		printf( "Ingrese el mensaje a transmitir: " );
		memset( buffer, '\0', TAM );
		fgets( buffer, TAM-1, stdin );

		n = send( newsockfd, buffer, strlen(buffer),0 );
        if(n < 0){
            perror("fallo en enviar info");
            exit(1);
        }

		// Verificando si se escribió: fin
		buffer[strlen(buffer)-1] = '\0';
		if( !strcmp( "fin", buffer ) ) {
			terminar = 1;
		}

		memset( buffer, '\0', TAM );
		n = recv( newsockfd, buffer, TAM,0 );
        if(n < 0){
            perror("fallo en recibir info");
            exit(1);
        }
		printf( "Respuesta: %s\n", buffer );
		if( terminar ) {
			printf( "Finalizando ejecución\n" );
			exit(0);
		}
		}

		  }

	}
	return 0; 
 
}