#include "../include/deliverymanager.h"

/*
	Programa Delivery Manager, para recibir lo que envíen los productores y enviarlo a los clientes
	Emplea el socket dado en clase
*/

//Cuando se produce la interrupción, cierra la cola de mensaje y sale del programa
void signal_handler(void){
	printf("Cerrando cola de mensajes\n");
	msgctl(get_queue(),IPC_RMID,(struct msqid_ds *) NULL);
	exit(1);
}
 #pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
int32_t main( int argc, char *argv[] ) {
	int32_t sockfd, newsockfd, puerto; 
    uint32_t clilen;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t n;
    int32_t pid_prod1, pid_prod2, pid_prod3, pid_cli;
	char* mensaje; 											//lo que recibe de los productores
	char* respuesta = malloc(sizeof(*respuesta));			//lo que envia al cliente

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
	int32_t desconectado = 0;

	atexit(signal_handler);										// Cuando el programa se termina, invoca a la función handler
    if (signal(SIGINT, signal_handler) == SIG_ERR) {			// crea el signal, el cual llamara al handler en caso de ingresar Ctrl+C
        fputs("Error al levantar el handler.\n", stderr);
		free(respuesta);
        return EXIT_FAILURE;
    }

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
					*/

					newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen ); 
					if (newsockfd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
						perror("  accept() failed");
						}
						break;
					}
					n = send( newsockfd, "Bienvenido al Delivery Manager\n", TAM,0 );
					if(n < 0){
						perror("fallo en enviar info");
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
					
				if(desconectado == 0){}


				}while(newsockfd != -1);

				

			}else{
        		close_conn = FALSE;


				//ImprimirElementosLista(clientes_conectados);
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						mensaje = recive_from_queue((long)ID_PROD1,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){
							char respuesta[strlen(mensaje_prod1)+strlen(mensaje)];
							sprintf(respuesta,"%s%s",mensaje_prod1,mensaje);
							struct lista *aux = clientes_conectados;
							while(aux != NULL){
								n = send( aux->fd, respuesta, strlen(respuesta),0 );
								if(n < 0){
									perror("fallo en enviar info");
								}		
								imprimir_log(LOG_PROD_1,respuesta,aux->ip,aux->port);	
								aux = aux->sig;					 									
							}			
						memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene				
						}

						mensaje = recive_from_queue((long)ID_PROD2,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){
							char respuesta[strlen(mensaje_prod2)+strlen(mensaje)];
							sprintf(respuesta,"%s%s",mensaje_prod2,mensaje);					
						//	struct lista *aux = clientes_conectados;
						//	while(aux != NULL){
						//		n = send( aux->fd, respuesta, strlen(respuesta),0 );
						//		if(n < 0){
						//			perror("fallo en enviar info");
						//		}	
						//		imprimir_log(LOG_PROD_2,respuesta,aux->ip,aux->port);	
						//		aux = aux->sig;							
						//	}
						memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene
						}

						mensaje = recive_from_queue((long)ID_PROD3,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){			
							char respuesta[strlen(mensaje_prod3)+strlen(mensaje)];
							sprintf(respuesta,"%s%s",mensaje_prod3,mensaje);						
						//	struct lista *aux = clientes_conectados;
						//	while(aux != NULL){
						//		n = send( aux->fd, respuesta, strlen(respuesta),0 );
						//		if(n < 0){
						//			perror("fallo en enviar info");
						//		}	
						//		imprimir_log(LOG_PROD_3,respuesta,aux->ip,aux->port);										
						//		aux = aux->sig;				
						//	}
						memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene
						}
					
						mensaje = recive_from_queue((long)ID_CLI,MSG_NOERROR | IPC_NOWAIT);
						if(errno != ENOMSG){			
							printf("Prueba 1\n");		
							mensaje[strlen(mensaje)-1]='\0'; //coloca un valor final al final del comando

							//variables que usa para guardar comandos, opciones y argumentos
							char* mensaje_comando;
							char comando[TAM];
							char socket[TAM];
							char productor[TAM];

							//separa el comando en tokens para valuar

							printf("Prueba 2\n");
							mensaje_comando = strtok(mensaje, " ");
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
							fflush(stdout);			//limpio el teclado o se va a pisar

							printf(" %s %s %s\n", comando, socket, productor );

							
							//toma lo que puse en <socket> y lo separa en ip y puerto	
							char* login;			
							login = strtok(socket,":");
							printf("Prueba 3\n");
							char ip[strlen(login)];
							if(ip == NULL){
								perror("Fallo en alocar memoria en ip\n");
							}
							sprintf(ip,"%s",login);
							printf("Prueba 4\n");
							login = strtok(NULL,":");
							char puerto[strlen(login)];								
							if(puerto == NULL){
								perror("Fallo en alocar memoria en puerto\n");
							}												
							sprintf(puerto,"%s",login);
							printf("Prueba 5\n");

							printf("La ip es %s",ip);
							printf("Elpuerto es %s",puerto);																					

							//
							//	Valida el comando
							//
							
							//
							//	Si es add, se fija en el productor
							//	recorre la lista comparando ip y puerto, cuando lo encuentra, agrega a la lista 
							//	de suscriptos colocando en 1 el campo subs_# 
							//
							if( strcmp("add", comando) == 0 ){
									struct lista *aux = clientes_conectados;
									int agregado = 0;								
									if(strcmp("1",productor)){
										while(aux != NULL){
											printf("viendo\n");
											if(((strcmp(aux->ip, ip) == 0) && aux->port == atoi(puerto))){
												aux->subs_1 = 1;
												agregado = 1;
												break;
											}
											aux = aux->sig;
										}
									}
									if(strcmp("2",productor)){
										while(aux != NULL){
											if(((strcmp(aux->ip, ip) == 0) && aux->port == atoi(puerto))){
												aux->subs_2 = 1;
												agregado = 1;
												break;
											}
											aux = aux->sig;
										}
									}
									if(strcmp("3",productor)){
										while(aux != NULL){
											if(((strcmp(aux->ip, ip) == 0) && aux->port == atoi(puerto))){
												aux->subs_3 = 1;
												agregado = 1;
												break;
											}
											aux = aux->sig;
										}
									}

									//
									//	Envía al CLI el resultado, si se agregó o no
									//
									if(agregado == 0){
										sprintf(respuesta,"No se detecta cliente para agregar\n");    
									}else{
										sprintf(respuesta,"Cliente agregado con éxito\n");
									}	

								imprimir_log(LOG_CLI,respuesta,ip,atoi(puerto));
								//
								//	Si es delete, recorre la lista, busca el socket por ip y puerto
								//	si lo encuentra, lo borra de la lista, cambiando el campo
								//	subs_# por 0
							}
							else if( strcmp("delete", comando) == 0 ){
									struct lista *aux = clientes_conectados;
									int borrado = 0;
									if(strcmp("1",productor)){
										while(aux != NULL){
											if(((strcmp(aux->ip, ip) == 0) && aux->port == atoi(puerto))){
												aux->subs_1 = 0;
												borrado = 1;
												break;
											}
											aux = aux->sig;
										}
									}
									if(strcmp("2",productor)){									
										while(aux != NULL){
											if(((strcmp(aux->ip, ip) == 0) && aux->port == atoi(puerto))){
												aux->subs_2 = 0;
												borrado = 1;
												break;
											}
											aux = aux->sig;
										}
									}
									if(strcmp("3",productor)){									
										while(aux != NULL){
											if(((strcmp(aux->ip, ip) == 0) && aux->port == atoi(puerto))){
												aux->subs_3 = 0;
												borrado = 1;
												break;
											}
											aux = aux->sig;
										}
									}

									if(borrado == 0){
										sprintf(respuesta,"No se detecta cliente para borrar\n");
									}else{
										sprintf(respuesta,"Cliente borrado con éxito\n");
									}
							imprimir_log(LOG_CLI,respuesta,ip,atoi(puerto));
							}
							else if( strcmp("log", comando) == 0 ){
									printf("estamos logueando\n");
									zip_t* z;
									//zip_source_t* zs;
									z = zip_open("log.zip", ZIP_CREATE, NULL);
    								if(z == NULL){
        								perror("Error al clear el archivo zip\n");
        								exit(EXIT_FAILURE);
    								}
									//zip_walk(z, LOG_PATH);

    								zip_close(z);

							}else{
									printf("estamos nose\n");
							}
								//
								//	limpio las variables o se llenan de datos anteriores
								//
								memset(comando,'\0',strlen(comando));
								memset(socket,'\0',strlen(socket));
								memset(productor,'\0',strlen(productor));	
								memset(ip,'\0',strlen(ip));	
								memset(puerto,'\0',strlen(puerto));		
								memset(mensaje,'\0',strlen(mensaje));			// limpia el buffer "mensaje" para que no se llene															
						}
						
											
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