#include "recursos.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>

#define puerto_files 			 8020

#define DOWNLOAD                 "downloader"

void conect_to_files();
void configurar_socket();
void conect_to_files();

int isdownload = 0;