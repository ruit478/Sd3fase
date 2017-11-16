// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

#ifndef _NETWORK_CLIENT_PRIVATE_H
#define _NETWORK_CLIENT_PRIVATE_H

#include "client_stub-private.h"
#include "inet.h"
#include "network_client.h"
#define rTime 3 // Tempo de retry

struct server_t {
  /* Atributos importantes para interagir com o servidor, */
  /* tanto antes da ligação estabelecida, como depois.    */
  char *hostname;
  int port;
  int id; // id do socket
  struct sockaddr_in server_data;
};

/* Função que garante o envio de len bytes armazenados em buf,
   através da socket sock.
*/
int write_all(int sock, char *buf, int len);

/* Função que garante a receção de len bytes através da socket sock,
   armazenando-os em buf.
*/
int read_all(int sock, char *buf, int len);
/*
Tenta restablecer a ligacao
*/
int reconnect(struct rtables_t *tables);
#endif
