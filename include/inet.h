// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// tamanho máximo da mensagem enviada pelo cliente
#define MAX_MSG 2048
