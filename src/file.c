#include "../include/deliverymanager.h"
#define puerto_files    8020
#define IMAGES_PATH     "/home/diego/Escritorio/Sistemas-Operativos-2/TP1/soii-2021-ipc-adgko/bin/log.zip"


int32_t sockfd, sock_cli;
struct sockaddr_in serv_addr;
struct sockaddr_in client_addr;
uint32_t client_len;				//tamaño de la dirección del cliente
char buffer[TAM];
ssize_t n;							// hubo que declarar n como ssize_t para que no pierda información al usarse en send() y recv()

int32_t main(){

    

	configurar_socket();	// configura y abre el socket para transmitir 

	escuchando();			// espera que se conecte un socket

	conectar_cliente();

    //read_log();

   // n = recv( sock_cli, buffer, TAM-1, MSG_WAITALL );
	//			if ( n < 0 ) {
	//		 	 perror( "lectura de socket" );
	//			  exit(1);
	//			}
	
		printf( "%sRecibí: %s%s\n", KBLU,buffer,KNRM );
     n = send( sock_cli, "12341234 respuesta", strlen("12341234 respuesta"),0 );
									if(n < 0){
										perror("fallo en enviar info");
									}

	exit(0);
}

/*
	Configura el socket con el que se enviará la imagen
*/
void configurar_socket() {
	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( (uint16_t) puerto_files);
	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "conexion" );
		exit(1);
	}
}

/*
	Abre conexión para que se conecte un clietne
*/
void escuchando(){
	listen(sockfd, 1);
	client_len = sizeof(client_addr);
}

/*
	Busca el archivo solicitado, avisa si lo encontró o no, y en caso afirmativo
	envía dicho archivo
*/
void read_log(){

	char img[TAM];
    long size = 0;

	sprintf(img,"%s",IMAGES_PATH);		//Path de la imagen
				
  	FILE *imgn;
  	imgn = fopen(img, "r");
  	if(imgn == NULL)
  	{
    	perror("file");
    	return;
  	}

  	// Calcula el tamaño de la imagen
  	fseek(imgn, 0, SEEK_END); 							
  	size = ftell(imgn);
  	fclose(imgn);

  	// guarda tamaño para enviar
  	char size_s[TAM] = "";
  	sprintf(size_s, "%ld", size);						

  	char *md5s = md5(img, 0);

    if(md5s != NULL){}

  	// guarda en buffer el tamaño y hash para enviar
  	//sprintf(buffer, "Download %s %s", size_s, md5s);	

    

    //Envía la imagen por socket
    //enviar_imagen(img, &size);
   

}

/*
	Conecta con el cliente que usó su dirección
*/
void conectar_cliente(){

	sock_cli = accept( sockfd, NULL,NULL); 
	if ( sock_cli < 0 ) {
		
		perror( "accept" );
		exit( 1 );
	}
	printf("conectado");
}
