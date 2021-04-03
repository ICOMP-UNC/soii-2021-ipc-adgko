// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 8080
#define TAM 1024
   
int main()
{
    int sock = 0; 
	ssize_t valread;
    struct sockaddr_in serv_addr;
    //char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
	while(1){
	memset(buffer, 0, TAM);	
	printf("ingrese mensaje a enviar\n");
	fgets(buffer,TAM,stdin);
    send(sock , buffer , strlen(buffer) , 0 );
    printf("Hello message sent\n");
    valread = recv( sock , buffer, 1024,0);
	if(valread < 0){
		perror("no se recibió la info");
		exit(1);
	}
    printf("%s\n",buffer );
	}
    return 0;
}