#include "../include/deliverymanager.h"

/*
	Programa Delivery Manager, para recibir lo que envíen los productores y enviarlo a los clientes
	Emplea el socket dado en clase
*/
int32_t main( int argc, char *argv[] ) {
	int32_t sockfd, newsockfd, puerto, pid;
    uint32_t clilen;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t n;
    int32_t pid_prod1, pid_prod2, pid_prod3;
	char* mensaje; 											//lo que recibe de los productores
	char* respuesta;										//lo que envia al cliente

	char* mensaje_prod1 = "Productor 1: ";					// etiqueta para ejecutar el binario del Productor 1								
	char* mensaje_prod2 = "Productor 2: ";					// etiqueta para ejecutar el binario del Productor 2	
	char* mensaje_prod3 = "Productor 3: ";					// etiqueta para ejecutar el binario del Productor 3	

	if ( argc < 2 ) {
        	fprintf( stderr, "Uso: %s <puerto>\n", argv[0] );
		exit( 1 );
	}

	/*
		Crea procesos hijos para los 3 productores
	*/
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

	listen( sockfd, 5000 );								// 5000 es la máxima cantidad de socket que pueden conectarse a este servidor
	clilen = sizeof( cli_addr );

	while( 1 ) {
		newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );

		pid = fork(); 

		if ( pid == 0 )
		{  // Proceso hijo
		    close( sockfd );

			while(1) {
				/*
				printf( "%sIngrese el mensaje a transmitir: %s",KBLU,KNRM );
				memset( buffer, '\0', TAM );
				fgets( buffer, TAM-1, stdin );*/

				/*
				n = send( newsockfd, buffer, strlen(buffer),0 );
				if(n < 0){
					perror("fallo en enviar info");
					exit(1);
				}*/

				/*
				// Verificando si se escribió: fin
				buffer[strlen(buffer)-1] = '\0';
				if( !strcmp( "fin", buffer ) ) {
					terminar = 1;
				}*/

				/*
				memset( buffer, '\0', TAM );
				n = recv( newsockfd, buffer, TAM,0 );
				if(n < 0){
					perror("fallo en recibir info");
					exit(1);
				}
				printf( "%sRespuesta: %s%s\n", KBLU,buffer,KNRM );
				if( terminar ) {
					printf( "Finalizando ejecución\n" );
					exit(0);
				}*/

				/*
					Revisa la cola de mensaje en espera que alguno de los productores haya colocado algo
				*/

				
/*
	Lee la cola de mensajes, y dependiendo quien haya enviado datos, lo envía a una lista de distribución distinta
*/
				mensaje = recive_from_queue((long)ID_PROD1,MSG_NOERROR | IPC_NOWAIT);
				if(errno != ENOMSG){

					char respuesta[strlen(mensaje_prod1)+strlen(mensaje)];
					sprintf(respuesta,"%s%s",mensaje_prod1,mensaje);
					n = send( newsockfd, respuesta, strlen(respuesta),0 );
						if(n < 0){
						perror("fallo en enviar info");
						exit(1);
					}

				}
				memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene

				mensaje = recive_from_queue((long)ID_PROD2,MSG_NOERROR | IPC_NOWAIT);
				if(errno != ENOMSG){

					char respuesta[strlen(mensaje_prod2)+strlen(mensaje)];
					sprintf(respuesta,"%s%s",mensaje_prod2,mensaje);
					n = send( newsockfd, respuesta, strlen(respuesta),0 );
						if(n < 0){
						perror("fallo en enviar info");
						exit(1);
					}
				}
				memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene

				mensaje = recive_from_queue((long)ID_PROD3,MSG_NOERROR | IPC_NOWAIT);
				if(errno != ENOMSG){

					char respuesta[strlen(mensaje_prod3)+strlen(mensaje)];
					sprintf(respuesta,"%s%s",mensaje_prod3,mensaje);
					n = send( newsockfd, respuesta, strlen(respuesta),0 );
						if(n < 0){
						perror("fallo en enviar info");
						exit(1);
					}
				}
				memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene


				if(respuesta != NULL){

				}
				if(buffer != NULL){

				}


			}

		}

	}
	return 0; 
 
}