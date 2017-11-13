// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

/*
   Programa que implementa um servidor de uma tabela hash com chainning.
   Uso: table-server <port> <table1_size> [<table2_size> ...]
   Exemplo de uso: ./table_server 5000 10 15 20 25
*/
#include <poll.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "inet.h"
#include "table-private.h"
#include "message-private.h"
#include "network_client-private.h"
#include "table_skel.h"
#define MAX_C 4
#define TIME -1
/* Função para preparar uma socket de receção de pedidos de ligação.
*/
int make_server_socket(short port){
  int socket_fd;
  struct sockaddr_in server;

  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("Erro ao criar socket");
    return -1;
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int so_reuseaddr = 1;
  setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR, &so_reuseaddr, sizeof (so_reuseaddr));

  if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0){
      perror("Erro ao fazer bind");
      close(socket_fd);
      return -1;
  }

  if (listen(socket_fd, 0) < 0){
      perror("Erro ao executar listen");
      close(socket_fd);
      return -1;
  }
  return socket_fd;
}


/* Função que recebe uma tabela e uma mensagem de pedido e:
	- aplica a operação na mensagem de pedido na tabela;
	- devolve uma mensagem de resposta com oresultado.
*/
struct message_t *process_message(struct message_t *msg_pedido, struct table_t *tabela){
	struct message_t *msg_resposta;

	/* Verificar parâmetros de entrada */
	if(msg_pedido == NULL || tabela == NULL){
		return NULL;
	}
	msg_resposta = (struct message_t *) malloc(sizeof(struct message_t));

	/* Verificar opcode e c_type na mensagem de pedido */
	if ((msg_pedido->opcode < 10) || (msg_pedido->opcode > 70)){
		return NULL;
	}
	/* Aplicar operação na tabela */

	switch(msg_pedido->opcode){

		case OC_PUT:
		msg_resposta->table_num = msg_pedido->table_num;
		msg_resposta->opcode = OC_PUT;
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = table_put(tabela, msg_pedido->content.entry->key, msg_pedido->content.entry->value);
		if(msg_resposta->content.result == -1){
			printf("Não consegui inserir na tabela\n");
			msg_resposta->opcode = OC_RT_ERROR;
		}
		else{
			msg_resposta->opcode = msg_resposta->opcode +1;
		}
		break;

		case OC_GET:
		//Todas as keys
		msg_resposta->table_num = msg_pedido->table_num;
		msg_resposta->opcode = OC_GET;

		if(strcmp(msg_pedido->content.key,"*") == 0){
			msg_resposta->c_type = CT_KEYS;
			msg_resposta->content.keys = table_get_keys(tabela);
			msg_resposta->opcode = msg_resposta->opcode + 1;
		}
		//Caso de só querer 1 key
		else{
			msg_resposta->c_type = CT_VALUE;
			struct data_t *dados = table_get(tabela,msg_pedido->content.key);
			if(dados == NULL){
				struct data_t* temp = malloc(sizeof(struct data_t));
        temp->data = NULL;
        temp->datasize = 0;
        msg_resposta->content.data = temp;
				msg_resposta->opcode = msg_resposta->opcode+1;
			}
			else{
				msg_resposta->opcode = msg_resposta->opcode+1;
				msg_resposta->content.data = dados;
			}
		}
		break;

		case OC_UPDATE:
		msg_resposta->table_num = msg_pedido->table_num;
		msg_resposta->opcode = OC_UPDATE;
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = table_update(tabela,msg_pedido->content.entry->key,msg_pedido->content.entry->value);
		if(msg_resposta->content.result == -1){
			printf("Erro ao fazer update");
			msg_resposta->opcode = OC_RT_ERROR;
		}
		else{
			msg_resposta->opcode = msg_resposta->opcode +1;
		}
		break;

		case OC_SIZE:
		msg_resposta->table_num = msg_pedido->table_num;
		msg_resposta->opcode = OC_SIZE; //Tou aqui
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = table_size(tabela);
		if(msg_resposta->content.result == -1){
			printf("Erro ao calcular o size da tabela");
			msg_resposta->opcode = OC_RT_ERROR;
		}
		else{
			msg_resposta->opcode = msg_resposta->opcode +1;
		}
		break;

		case OC_COLLS:
		msg_resposta->table_num = msg_pedido->table_num;
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = tabela->colls;
		msg_resposta->opcode = OC_COLLS +1;
		break;
	}

	/* Preparar mensagem de resposta */

	return msg_resposta;
}


/* Função "inversa" da função network_send_receive usada no table-client.
   Neste caso a função implementa um ciclo receive/send:

	Recebe um pedido;
	Aplica o pedido na tabela;
	Envia a resposta.
*/
int network_receive_send(int sockfd){
  char *message_resposta, *message_pedido;
  int message_size, msg_size, result; //message -> recebe do client, msg->dá ao client
  struct message_t *msg_pedido, *msg_resposta;

	/* Verificar parâmetros de entrada */
  if(sockfd < 0){
  	return -1;
  }
	/* Com a função read_all, receber num inteiro o tamanho da
	   mensagem de pedido que será recebida de seguida.*/
	result = read_all(sockfd, (char *) &msg_size, _INT);
	int msg_size_conv = 0;
	msg_size_conv = ntohl(msg_size);
	/* Verificar se a receção teve sucesso */
	if(result != _INT)
		return -1;
	/* Alocar memória para receber o número de bytes da
	   mensagem de pedido. */
	message_pedido = (char *) malloc(msg_size_conv);
	/* Com a função read_all, receber a mensagem de pedido. */
	result = read_all(sockfd, message_pedido ,msg_size_conv);
	/* Verificar se a receção teve sucesso */
	if(result != msg_size_conv){
		return -1;
	}
	/* Desserializar a mensagem do pedido */
	msg_pedido = buffer_to_message(message_pedido, msg_size_conv);
	print_message(msg_pedido);
	/* Verificar se a desserialização teve sucesso */
	if(msg_pedido == NULL)
		return -1;
	/* Processar a mensagem */

	msg_resposta = invoke(msg_pedido);
	print_message(msg_resposta);
	/* Serializar a mensagem recebida */
	message_size = message_to_buffer(msg_resposta, &message_resposta); //Problema aqui, server manda a resposta

	/* Verificar se a serialização teve sucesso */
	if(message_size == -1){
		return -1;
	}

	/* Enviar ao cliente o tamanho da mensagem que será enviada
	   logo de seguida
	*/
	msg_size = htonl(message_size);
 	result = write_all(sockfd, (char *) &msg_size, _INT);

	/* Verificar se o envio teve sucesso */
 	if(result != _INT){
 		return -1;
 	}
	/* Enviar a mensagem que foi previamente serializada */

	result = write_all(sockfd, message_resposta, message_size);

	/* Verificar se o envio teve sucesso */
	if(result != message_size){
		return -1;
	}
	/* Libertar memória */
	free(message_pedido);
	free(message_resposta);
	free_message(msg_pedido);
	return 0;
}

int main(int argc, char **argv){
	int listening_socket, connsock;
	struct sockaddr_in client;
	socklen_t size_client;
	int server_error = 0;

	if (argc < 3){
	printf("Uso: ./server <porta TCP> <table1_size> [<table2_size> ...]\n");
	printf("Exemplo de uso: ./table-server 54321 10 15 20 25\n");
	return -1;
	}
	struct pollfd *poll_list = NULL;
	size_client = sizeof(struct sockaddr_in);
	if ((listening_socket = make_server_socket(atoi(argv[1]))) < 0) return -1;

	/*********************************************************/
	/* Criar as tabelas de acordo com linha de comandos dada */
	/*********************************************************/
	//Copiar do argc o nr de tabelas
	char **n_tables = (char **) malloc(sizeof(char*) * argc-2);
	int ts;
	int index = 0;
	for(ts = 2; ts < argc;ts++){
		n_tables[index] = strdup(argv[ts]);
		index++;
	}
	n_tables[index+1] = NULL;

	if(table_skel_init(n_tables) == -1){
		perror("Erro ao criar tabela");
		close(listening_socket);
		return -1;
	}
	int totalConnections = MAX_C + 2;

	poll_list = (struct pollfd*) malloc(sizeof(struct pollfd) * totalConnections);
	
	for(int i = 0; i < totalConnections; i++){
		poll_list[i].fd = -1;
		poll_list[i].events = POLLIN;
	}
	poll_list[0].fd = listening_socket; // Socket de escuta toma o 1 lugar da lista

	printf("Servidor operacional!\n");

	while(server_error == 0){
		int resultado = poll(poll_list, totalConnections, TIME);
		if(resultado < 0){
			if(errno != EINTR)
				server_error = 1;
			continue;
		}

		if(poll_list[0].revents && POLLIN){ //Tem pedidos para ler(Socket de escuta)

			//Nova ligacao
			if((connsock = accept(poll_list[0].fd, (struct sockaddr *) &client, &size_client)) != -1){
				int j = 1;
				//Encontrar a posicao onde adicionar
				while(j < MAX_C && poll_list[j].fd != -1)
					j++;

				if(j < MAX_C){
					printf("\nCliente %d ligou-se!\n", j);
					poll_list[j].fd = connsock;
					poll_list[j].events = POLLIN;

				}
			}
		}
		//para cada socket cliente
		for(int k = 1; k < totalConnections; k++){
			if(poll_list[k].revents & POLLIN){
				int conn = network_receive_send(poll_list[k].fd);
				if(conn == -2 || conn == -1){
					printf("Cliente %d saiu!\n" , k);
					close(poll_list[k].fd);
					poll_list[k].fd = -1;
				}
			}

		if(poll_list[k].fd != -1 && poll_list[k].revents & POLLHUP){
			printf("Cliente %d saiu!\n" , k);
			close(poll_list[k].fd);
			poll_list[k].fd = -1;
			}
		}
	}
	table_skel_destroy();

	for(int t = 0; t< totalConnections; t++){
		if(poll_list[t].fd != -1)
			close(poll_list[t].fd);
	}
	printf("Tudo limpo, servidor a terminar\n");
	return 0;
}
