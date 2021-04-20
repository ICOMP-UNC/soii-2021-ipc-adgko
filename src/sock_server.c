#include "../include/deliverymanager.h"

/*
	Programa Delivery Manager, para recibir lo que envíen los productores y enviarlo a los clientes
	Emplea el socket dado en clase
*/
int32_t main( int argc, char *argv[] ) {
	int32_t sockfd, newsockfd, puerto; //pid;
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

	int32_t rc, on = 1;
	struct pollfd fds[MAX_CLIENTES]; //estructura para el poll()
	int32_t timeout; 		//tiempo luego del cual el programa se cierra
	int32_t end_server = FALSE, compress_array = FALSE;
  	long unsigned int   nfds = 1, current_size = 0, i, j;
	int32_t close_conn;



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
			  perror("error en productor 1 ");
   			  exit(1);
   			} 			
   			exit(0);
  		}

  	pid_prod2 = fork();
  		if ( pid_prod2 == 0 ) {
    		if( execv(PROD2_PATH, argv) == -1 ) {
			  perror("error en productor 2 ");
   			  exit(1);
   			}   			
   			exit(0);
		}

    pid_prod3 = fork();
  		if ( pid_prod3 == 0 ) {
    		if( execv(PROD3_PATH, argv) == -1 ) {
			  perror("error en productor 3 ");
   			  exit(1);
   			}   			
   			exit(0);
		}

	/*
		Creación de Socket
	*/
	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("Error al crear el socket");
		exit(1);
	}

	/*
		Con esta función, habilito al descriptor del socket para ser reutilizable en caso de corte
	*/
	rc = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&on, sizeof(on));
	if(rc < 0){
		perror("falló sersockopt()");
		close(sockfd);
		exit(1);
	}

	/*
		Seteamos el socket en no bloqueante. Todas las conexiones que le lleguen también lo serán
	*/
	rc = ioctl(sockfd, FIONBIO, (char *)&on);
	if(rc < 0){
		perror("falló ioctl()");
		close(sockfd);
		exit(1);
	}

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

	if(listen( sockfd, 5000 ) < 0){								// 5000 es la máxima cantidad de socket que se pueden enconlar para conectarse
		perror("Error al escuchar un cliente");
	}

	clilen = sizeof( cli_addr );

		/*
			Seteamos la estructura par aque escuche sockets
		*/
		memset(fds, 0 , sizeof(fds));
		fds[0].fd = sockfd;
  		fds[0].events = POLLIN;
		timeout = (3 * 60 * 1000); // 3 minutos
		if(timeout < 1){

		}

	do{

	    printf("Waiting on poll()...\n");
    	rc = poll(fds, nfds, timeout);
		if (rc < 0)
    	{
      		perror("  poll() failed");
      		break;
    	}	
		/*
			Se fija si alguien se conectó antes de 3 minutos, sino se va a cerrar el programa
		*/
		if (rc == 0)
		{
		printf("  poll() timed out.  End program.\n");
		break;
		}

		current_size = nfds; 					// cantidad de procesos esperando a ser atendidos
		for (i = 0; i < current_size; i++){

			/*
				recorre el loop buscando que file descriptor atender
			*/
			if(fds[i].revents == 0)
        	continue;

			/*
				Si no recibe un poll, es un evento desconocido y cerramos server antes que explote
			*/
			if(fds[i].revents != POLLIN)
			{
				printf("  Error! revents = %d\n", fds[i].revents);
				end_server = TRUE;
				break;

			}
			
			if (fds[i].fd == sockfd){

				printf(" %sCliente listo%s\n",KGRN,KNRM);

				do{
			
					/*
						acepta conexiones, y si devuelve el error EWOULDBLOCK, es que ya aceptamos todas y salimos
						Si sale otro error, cerramos el server
					*/

					newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen ); 
					if (newsockfd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
						perror("  accept() failed");
						end_server = TRUE;
						}
						break;
					}
					printf("AAAAA\n");
									n = send( newsockfd, "Bienvenido al Delivery Manager\n", TAM,0 );
										if(n < 0){
										perror("fallo en enviar info");
										//exit(1);
									}					
					
					/*
						Avisa las nuevas conexiones y la suma a la estructura fds
					*/
					printf("  New incoming connection - %d\n",newsockfd );
					fds[nfds].fd = newsockfd;
					fds[nfds].events = POLLIN;
					nfds++;

					//for(int32_t k; k < nfds; ){}

					printf("BBBBBB\n");

				}while(newsockfd != -1);

				

			}else{
				printf("  Descriptor %d is readable\n", fds[i].fd);
        		close_conn = FALSE;
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						mensaje = recive_from_queue((long)ID_PROD1,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){
							for(long unsigned int k1 = i; k1 < current_size; k1++){
								printf("%ld\n",k1);
								char respuesta[strlen(mensaje_prod1)+strlen(mensaje)];
								sprintf(respuesta,"%s%s",mensaje_prod1,mensaje);
								n = send( fds[k1].fd, respuesta, strlen(respuesta),0 );
									if(n < 0){
										perror("fallo en enviar info");
										//exit(1);
									}
								}
							}
						memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene
						mensaje = recive_from_queue((long)ID_PROD2,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){
							for(long unsigned int k2 = i; k2 < current_size; k2++){
								char respuesta[strlen(mensaje_prod1)+strlen(mensaje)];
								sprintf(respuesta,"%s%s",mensaje_prod2,mensaje);
								n = send( fds[k2].fd, respuesta, strlen(respuesta),0 );
									if(n < 0){
										perror("fallo en enviar info");
										//exit(1);
									}
								}

							}
						memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene
						mensaje = recive_from_queue((long)ID_PROD3,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){
							for(long unsigned int k3 = i; k3 < current_size; k3++){
								char respuesta[strlen(mensaje_prod1)+strlen(mensaje)];
								sprintf(respuesta,"%s%s",mensaje_prod3,mensaje);
								n = send( fds[k3].fd, respuesta, strlen(respuesta),0 );
								if(n < 0){
									perror("fallo en enviar info");
									//exit(1);
									}
								}
							}
						memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
					


					/*
						Cierrra todas las conexiones 
					*/
				    if (close_conn)
					{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
					}
			}



					//pid = fork(); 

					//if ( pid == 0 )
					//{  // Proceso hijo
					//  close( sockfd );

						//while(1) {
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
						


							if(respuesta != NULL){

							}
							if(buffer != NULL){

							}
				if(mensaje != NULL){}
				if(mensaje_prod1 != NULL){}
				if(mensaje_prod2 != NULL){}
				if(mensaje_prod3 != NULL){}

		} // acá termina el loop del poll

		/*
			Es para acortar el número de file descriptors presentes. 
		*/
		if (compress_array)
		{
		compress_array = FALSE;
		for (i = 0; i < nfds; i++)
		{
			if (fds[i].fd == -1)
			{
			for(j = i; j < nfds-1; j++)
			{
				fds[j].fd = fds[j+1].fd;
			}
			i--;
			nfds--;
			}
		}
		}

	} while (end_server == FALSE); /* End of serving running.    */

		/*
			Cierra todos los sockets antes de cerrar el programa
		*/
	   for (i = 0; i < nfds; i++)
		{
			if(fds[i].fd >= 0)
			close(fds[i].fd);
		}

	return 0; 
 
}