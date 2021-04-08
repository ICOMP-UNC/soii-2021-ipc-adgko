#include "recursos.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#define TAM 1024

#define PROD1_PATH "prod1"
#define PROD2_PATH "prod2"
#define PROD3_PATH "prod3"