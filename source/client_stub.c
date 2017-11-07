#include "client_stub.h"
#include "client_stub-private.h"
#include "network_client.h"

struct rtables_t *rtables_bind(const char *address_port) {
  struct server_t *server = network_connect(address_port);

  if (server == NULL) {
    return NULL
  }
  struct rtable_t *rtables = (struct rtable_t *)malloc(sizeof(struct rtable_t));
  if (rtables == NULL) {
    return NULL;
  }

  rtables->server = server;
  rtables->address_port = strdup(address_port);
  return rtables;
}

int rtables_unbind(struct rtables_t *rtables) {
  int result = network_close(rtable->server);
  if (result == -1) return result;

  free(rtrables->address_port);
  for (int i = 0; i < rtables->nrTables; i++) {
    table_destroy(&rtables->tables[i]);
  }

  free(rtables);
  return result;
}

int rtables_put(struct rtables_t *rtables, char *key, struct data_t *value) {
  if (rtables == NULL || key == NULL || value == NULL) {
    return -1;
  }
  int result = 0;
  int rc = 0;

  struct message_t *message = (struct message_t *)malloc(sizeof(struct message_t));
  if (message == NULL) {
    return -1;
  }

  message->opcode = OC_PUT;
  message->c_type = CT_ENTRY;
  message->content.entry->key = strdup(key);
  message->content.entry->value = data_dup(value);

  // Receber msg de resposta

  struct message_t *msg_resposta =
      network_send_receive(rtables->server, message);
  if (msg_resposta == NULL) {
    rc = reconnect(rtables);

    if (rc == -1) {
      free_message(message);
      free_message(msg_resposta);
      return -2;
    } else
      msg_resposta = network_send_receive(rtables->server, message);
  }

  if (msg_resposta->opcode == OC_RT_ERROR) {
    free_message(message);
    free_message(msg_resposta);
    return -1;
  }

  if (msg_resposta->opcode == OC_PUT + 1) {
    result = msg_resposta->content.result;
  }

  free_message(message);
  free_message(msg_resposta);
  return result;
}

int rables_update(struct rtables_t *rtables, char *key, struct data_t *value) {
  if (rtables == NULL || key == NULL || value == NULL) {
    return -1;
  }
  int result = -1;
  int rc = 0;

  struct message_t *message = (struct message_t *)malloc(sizeof(struct message_t));
  if (message == NULL) {
    return -1;
  }
  message->opcode = OC_UPDATE;
  message->c_type = CT_ENTRY;
  message->content.entry->key = strdup(key);
  message->content.entry->value = data_dup(value);

  struct message_t *msg_resposta = network_send_receive(rtables->server, message);
  if (msg_resposta == NULL) {
    rc = reconnect(rtables);

    if (rc == -1) {
      free_message(message);
      free_message(msg_resposta);
      return -2;
    } 
    else
      msg_resposta = network_send_receive(rtables->server, message)
  
  }

  if(msg_resposta->opcode == OC_RT_ERROR){
  	free_message(message);
    free_message(msg_resposta);
  	return -1;
  }

  if(msg_resposta->opcode == OC_UPDATE + 1){
  	result = msg_resposta->content.result;
  }
  
  free_message(message);
  free_message(msg_resposta);
  return result;
  }

  struct data_t *rtables_get(struct rtables_t * tables, char *key) {
  	if (rtables == NULL || key == NULL) {
    return -1;
  }
  	int rc = 0;
  	struct message_t *message = (struct message_t *) malloc(sizeof(struct message_t));
  	if(message == NULL)
  		return NULL;
  
  	message->opcode = OC_GET;
  	message->opcode = CT_KEY;
  	message->content.key = strdup(key);

  	struct message_t *msg_resposta = network_send_receive(rtables->server, message);
  	if (msg_resposta == NULL) {
    rc = reconnect(rtables);

    if (rc == -1) {
      free_message(message);
      free_message(msg_resposta);
      return -2;
    } 
    else
      msg_resposta = network_send_receive(rtables->server, message)
  
  }

  if(msg_resposta->opcode == OC_RT_ERROR){
  	free_message(message);
  	free_message(msg_resposta);
  	return NUL;
  }

  //Falta tratar Recepção do get
}
  int rtables_size(struct rtables_t * rtables) {}

  char **rtables_get_keys(struct rtables_t * rtables) {}

  void rtables_free_keys(char **keys) {
  	if(keys != NULL){
  		int j = 0;
  		while(keys[j] != NULL){
  			free(keys[j]);
  			j++;
  		}
  	free(keys);
  	}
  }