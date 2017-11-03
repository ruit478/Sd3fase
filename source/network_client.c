// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

#include "network_client-private.h"
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include <errno.h>

void print_message(struct message_t *msg) {

    int i;

    printf("\n----- MESSAGE -----\n");
    printf("Tabela número: %d\n", msg->table_num);
    printf("opcode: %d, c_type: %d\n", msg->opcode, msg->c_type);
    switch(msg->c_type) {
        case CT_ENTRY:{
            printf("key: %s\n", msg->content.entry->key);
            printf("datasize: %d\n", msg->content.entry->value->datasize);
        }break;
        case CT_KEY:{
            printf("key: %s\n", msg->content.key);
        }break;
        case CT_KEYS:{
            for(i = 0; msg->content.keys[i] != NULL; i++) {
                printf("key[%d]: %s\n", i, msg->content.keys[i]);
            }
        }break;
        case CT_VALUE:{
              printf("datasize: %d\n", msg->content.data->datasize);
        }break;
        case CT_RESULT:{
            printf("result: %d\n", msg->content.result);
        }break;
        case OC_RT_ERROR:{
            printf("result: %d\n", msg->content.result);
        };
    }
    printf("-------------------\n");
}

int write_all(int sock, char *buf, int len) {
  int bufsize = len;
   while (len > 0) {
    int res = write(sock, buf, len);
    if (res < 0) {
      if(errno == EINTR) continue;
      perror("write failed:");
      return res;
    }
    buf += res;
    len -= res;
  }
  return bufsize;
}

int read_all(int sock, char *buf, int len) {
  int bufsize = len;
   while (len > 0) {
    int res = read(sock, buf, len);
    if (res < 0) {
      if(errno == EINTR) continue;
      perror("write failed:");
      return res;
    }
    buf += res;
    len -= res;
  }
  return bufsize;
}

struct server_t *network_connect(const char *address_port) {
  struct sockaddr_in server_data;
  char *ptr = strdup(address_port);
  char *host;
  int port;
  int sockfd;
  /* Verificar parâmetro da função e alocação de memória */
  if(address_port == NULL) return NULL;
  /* Estabelecer ligação ao servidor:

          Preencher estrutura struct sockaddr_in com dados do
          endereço do servidor.

          Criar a socket.

          Estabelecer ligação.
  */
  host = strtok(ptr, ":");
  port = atoi(strtok(NULL," "));

  if(port < 1024 || port > 49151){
    printf("Porto nao suportado");
  }

  server_data.sin_family = AF_INET;
  server_data.sin_port = htons(port);

  if(inet_pton(AF_INET, host, &server_data.sin_addr) <1){
    return NULL;
  }
  	//As 3 linhas acima preenchem a struct server_data

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    close(sockfd);
    return NULL;
  }
  	//A linha acima cria o socket
  if(connect(sockfd,(struct sockaddr *) &(server_data) ,sizeof(server_data)) < 0){
    close(sockfd);
    return NULL;
  }

  /* Se a ligação não foi estabelecida, retornar NULL */

  struct server_t *server = malloc(sizeof(struct server_t)) ;
  if(server == NULL){
    close(sockfd);
    return NULL;
  }
	server->hostname = strdup(host);
	server->port = port;
	server->id= sockfd;
	server->server_data = server_data;

  free(ptr);
  return server;
}

struct message_t *network_send_receive(struct server_t *server, struct message_t *msg) {
  char *message_out;
  int message_size, msg_size, result; //message é de send, msg_size
  struct message_t *msg_resposta;

  /* Verificar parâmetros de entrada */
  if(server == NULL || msg == NULL) return NULL;
  /* Serializar a mensagem recebida */
        message_size = message_to_buffer(msg, &message_out);

        /* Verificar se a serialização teve sucesso */
        if(message_size == -1){
          return NULL;
        }
        /* Enviar ao servidor o tamanho da mensagem que será enviada
           logo de seguida
        */
        msg_size = htonl(message_size);
        result = write_all(server->id, (char *) &msg_size, _INT);

        /* Verificar se o envio teve sucesso */
        if(result != _INT){ // Ver caso de erro
          return NULL;
        }
        /* Enviar a mensagem que foi previamente serializada */
                                                    //_INT
        result = write_all(server->id, message_out, message_size);
        print_message(msg);
        /* Verificar se o envio teve sucesso */
                    //_INT
        if(result != message_size){
          return NULL;
        }
        /* De seguida vamos receber a resposta do servidor: */
        int msg_size_reply = 0;
                /* Com a função read_all, receber num inteiro o tamanho da
                mensagem de resposta. */
        int nrBytes = read_all(server->id, (char*) &msg_size_reply, _INT);

        msg_size_reply = ntohl(msg_size_reply);

        if(nrBytes != _INT)
          return NULL;
                /* Alocar memória para receber o número de bytes da
                mensagem de resposta. */
        char * msg_buffer = (char *) malloc(msg_size_reply);
                /* Com a função read_all, receber a mensagem de resposta. */
        int nrBytes_resposta = read_all(server->id, msg_buffer,msg_size_reply);
        if(nrBytes_resposta != msg_size_reply){
          return NULL;
        }

        /* Desserializar a mensagem de resposta */
        msg_resposta = buffer_to_message(msg_buffer, msg_size_reply); // FIQUEI AQUI
        print_message(msg_resposta);
        /* Verificar se a desserialização teve sucesso */
        if(msg_resposta == NULL){
          free(msg_buffer);
          return NULL;
        }
        /* Libertar memória */
        free(msg_buffer);
        return msg_resposta;
}

int network_close(struct server_t *server) {
  /* Verificar parâmetros de entrada */
	if(server == NULL){
		free(server);
		return -1;
	}
  /* Terminar ligação ao servidor */
	close(server->id);
  /* Libertar memória */
	free(server->hostname);
	//Acho que falta libertar mem para a struct sockaddr
	free(server);
	return 0;
}
