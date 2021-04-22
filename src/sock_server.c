#include "../include/deliverymanager.h"

/*
	Programa Delivery Manager, para recibir lo que envíen los productores y enviarlo a los clientes
	Emplea el socket dado en clase
*/
int32_t main( int argc, char *argv[] ) {
	int32_t sockfd, newsockfd, puerto; 
    uint32_t clilen;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t n;
    int32_t pid_prod1, pid_prod2, pid_prod3, pid_cli;
	char* mensaje; 											//lo que recibe de los productores
	char* respuesta;										//lo que envia al cliente

	char* mensaje_prod1 = "Productor 1: ";					// etiqueta para ejecutar el binario del Productor 1								
	char* mensaje_prod2 = "Productor 2: ";					// etiqueta para ejecutar el binario del Productor 2	
	char* mensaje_prod3 = "Productor 3: ";					// etiqueta para ejecutar el binario del Productor 3	

	int32_t rc, on = 1;
	struct pollfd fds[MAX_CLIENTES]; 						//estructura para el poll()
	int32_t timeout; 										//tiempo luego del cual el programa se cierra
	int32_t end_server = FALSE, compress_array = FALSE;
  	long unsigned int   nfds = 1, current_size = 0, i, j;
	int32_t close_conn;
	struct lista *clientes_conectados;
	clientes_conectados = NULL;
	struct lista *suscriptos1;
	suscriptos1 = NULL;
	struct lista *suscriptos2;
	suscriptos2 = NULL;
	struct lista *suscriptos3;
	suscriptos3 = NULL;

	if ( argc < 2 ) {
        	fprintf( stderr, "Uso: %s <puerto>\n", argv[0] );
		exit( 1 );
	}

	/*
		Crea procesos hijos para los 3 productores y el CLI
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
	
	pid_cli = fork();
  		if ( pid_cli == 0 ) {
    		if( execv(CLI_PATH, argv) == -1 ) {
			  perror("error en CLI ");
   			  exit(1);
   			}   			
   			exit(0);
		}
	/*
		Creación de Socket
		AF_INET internet
		SOCK_STREAM stream de bytes
	*/
	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("Error al crear el socket");
		exit(1);
	}
	/*
		Con esta función seteo el socket. Habilito al descriptor del socket para ser reutilizable en caso de corte
		SOL_SOCKET para ingresar a nivel socket, no aplicación
		SO_REUDEADDR para reutilizar la dirección
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
	/*
		Configurando el socket en internet, con el puerto que le pase
	*/
	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
	puerto = atoi( argv[1] );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( (uint16_t)puerto );
	/*
		Asigno la dirección al socket, sino el so le asigna una por defecto
	*/
	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "error de ligadura" );
		exit( 1 );
	}

        printf( "Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	if(listen( sockfd, MAX_CLIENTES ) < 0){								// 5000 es la máxima cantidad de socket que se pueden enconlar para conectarse
		perror("Error al escuchar un cliente");
		exit(1);
	}

	clilen = sizeof( cli_addr );

	/*
		Seteamos la estructura par aque escuche sockets y el tiempo en el cual el programa se apaga si no recibe más conexiones
	*/
	memset(fds, 0 , sizeof(fds));
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;						// datos para leer
	timeout = (MINUTES * SECONDS * MILISECONDS); // 3 minutos
	/*
		Comienza a recibir conexiones y entregar mensajes
	*/
	do{

    	rc = poll(fds, nfds, timeout);
		if (rc < 0)
    	{
      		perror(" Falló poll() ");
      		break;
    	}	
		/*
			Se fija si alguien se conectó antes de 3 minutos, sino se va a cerrar el programa
		*/
		if (rc == 0)
		{
		printf(" TIEMPO! No hubo más conexiones y el Servidor se cierra. Que tenga un buen día\n");
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
			
			/*
				Si el file descriptor es del delivery, atiende conexiones, 
				si es otro, recorre los descriptores viendo si debe enviarles algo
			*/
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
					n = send( newsockfd, "Bienvenido al Delivery Manager\n", TAM,0 );
					if(n < 0){
						perror("fallo en enviar info");
						//exit(1);
					}					
					
					/*
						Avisa las nuevas conexiones y la suma a la estructura fds
					*/
					printf("  New incoming connection - %d - socket %s:%d\n",newsockfd, inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port );
					fds[nfds].fd = newsockfd;
					fds[nfds].events = POLLIN;
					nfds++;

					/*
						Guarda los clientes conectados a la lista
					*/
					clientes_conectados = insertafinal(clientes_conectados,newsockfd,inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port);



				}while(newsockfd != -1);

				

			}else{
        		close_conn = FALSE;

				//ImprimirElementosLista(clientes_conectados);
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						mensaje = recive_from_queue((long)ID_PROD1,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){
							for(long unsigned int k1 = i; k1 < current_size; k1++){
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
					/*	mensaje = recive_from_queue((long)ID_CLI,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){						
							printf("Prueba 2\n");
							mensaje[strlen(mensaje)-1]='\0'; //coloca un valor final al final del comando

							//variables que usa para guardar comandos, opciones y argumentos
							char* mensaje_comando;
							char comando[TAM];
							char socket[TAM];
							char productor[TAM];

						printf("Prueba 3\n");
							//separa el comando en tokens para valuar
							mensaje_comando = strtok(mensaje, " ");
						printf("Prueba 4\n");
							for(int32_t i=0; mensaje_comando != NULL; i++){
								if(i == 0){
									sprintf(comando, "%s", mensaje_comando);
								}
								else if(i == 1){
									sprintf(socket,"%s", mensaje_comando);
								}
								else {
									sprintf(productor,"%s",mensaje_comando);
								}
									
								mensaje_comando = strtok(NULL," ");
							}
						printf("Prueba 5\n");

							fflush(stdout);			//limpio el teclado o se va a pisar

							printf(" %s %s %s\n", comando, socket, productor );

							
								//toma lo que puse en <socket> y lo separa en ip y puerto
							
							char* login;
							login = strtok(socket,":");
							char ip[strlen(login)];
							sprintf(ip,"%s",login);
							login = strtok(NULL,":");
							char puerto[strlen(login)];
							sprintf(puerto,"%s",login);

							//printf("la ip es %s\n",ip);
							//printf("el puerto es %s\n",puerto);							

								if( strcmp("add", comando) == 0 ){
									printf("estamos agregando\n");
									if(strcmp("1",productor)){
										struct lista *aux = clientes_conectados;
										while(aux != NULL){
											  if(((strcmp(aux->ip, ip) == 0) && aux->port == atoi(puerto))){
												  suscriptos1 = insertafinal(suscriptos1,aux->fd,aux->ip, aux->port);
												  printf("agregado %d %s %d",suscriptos1->fd,suscriptos1->ip,suscriptos1->port);
												  break;
											  }
											  aux = aux->sig;
										}
									}
								}
								else if( strcmp("delete", comando) == 0 ){
									printf("estamos borrando\n");
								}
								else if( strcmp("log", comando) == 0 ){
									printf("estamos logueando\n");
								}
								else{
									printf("estamos nose\n");
								}
								//memset(mensaje_comando,'\0',strlen(mensaje_comando));
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
			} // acá termina el for de recorrer file descriptors
			//////////////////////////////////
				if(respuesta != NULL){}
				if(buffer != NULL){}
				if(suscriptos1 != NULL){}
				if(suscriptos2 != NULL){}
				if(suscriptos3 != NULL){}
				if(mensaje_prod1 != NULL){}
				if(mensaje_prod2 != NULL){}
				if(mensaje_prod3 != NULL){}		
				if(mensaje != NULL){}																														
			//////////////////////////////////

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