// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

/*
    Programa cliente para manipular tabela de hash remota.
    Os comandos introduzido no programa não deverão exceder
    80 carateres.

    Uso: table-client <ip servidor>:<porta servidor>
    Exemplo de uso: ./table_client 10.101.148.144:54321
*/

#include "network_client-private.h"
int main(int argc, char **argv) {
  struct server_t *server;
  char input[81];
  struct message_t *msg_out, *msg_resposta;
  char *option;
  char *split2;
  char *split3;
  char *split4;
  /* Testar os argumentos de entrada */
  if (argc != 2) {
    printf("Argumentos Insuficientes");
    printf("Uso: ./client 127.0.0.1:5000 ");
    return -1;
  }
  /* Usar network_connect para estabelcer ligação ao servidor */
  server = network_connect(argv[1]);

  /* Fazer ciclo até que o utilizador resolva fazer "quit" */
  while (1) {
    printf(">>> ");  // Mostrar a prompt para inserção de comando

    /* Receber o comando introduzido pelo utilizador
       Sugestão: usar fgets de stdio.h
       Quando pressionamos enter para finalizar a entrada no
       comando fgets, o carater \n é incluido antes do \0.
       Convém retirar o \n substituindo-o por \0.
    */
    fgets(input, 81, stdin);  // Ver o size
    input[strlen(input) - 1] = '\0';
    /* Verificar se o comando foi "quit". Em caso afirmativo
       não há mais nada a fazer a não ser terminar decentemente.
     */
    if (strcmp(input, "quit") == 0) {
      break;
    }

    option = strtok(input, " ");  // Tratar dos argumentos de entrada
    split2 = strtok(NULL, " ");   // Nr tabela

    msg_out = (struct message_t *)malloc(sizeof(struct message_t));
    if(msg_out == NULL){
      free_message(msg_out);
      return -1;
    }
    /* Caso contrário:

        Verificar qual o comando;

        Preparar msg_out;

        Usar network_send_receive para enviar msg_out para
        o server e receber msg_resposta.
    */
    /////////////////////Put///////////////////
    if (strcmp(option, "put") == 0) {
      split3 = strtok(NULL, " ");      // Key
      split4 = strtok(NULL, "\0");     // data
      if (split2 == NULL || split3 == NULL || split4 == NULL) {
        printf("Nr inválido de argumentos");
      }
      struct data_t *dados = data_create2(strlen(split4), split4);

      msg_out->table_num = (short) atoi(split2);
      msg_out->opcode = OC_PUT;
      msg_out->c_type = CT_ENTRY;
      msg_out->content.entry = (struct entry_t*) malloc(sizeof(struct entry_t));
      if(msg_out->content.entry == NULL){
        free_message(msg_out);
        data_destroy(dados);
        return -1;
      }
      msg_out->content.entry->key = strdup(split3);
      msg_out->content.entry->value = data_dup(dados);

      data_destroy(dados);
    }

    else if (strcmp(option, "get") == 0) {
      split3 = strtok(NULL, "\0");

      if (split2 == NULL || split3 == NULL) {
        printf("Nr Inválido de argumentos");
      }

      msg_out->table_num = (short)atoi(split2);
      msg_out->opcode = OC_GET;
      msg_out->c_type = CT_KEY;
      msg_out->content.key = strdup(split3);
    }

    else if (strcmp(option, "update") == 0) {
      split3 = strtok(NULL, " ");   // Key
      split4 = strtok(NULL, "\0");  // Data

      if (split2 == NULL || split3 == NULL || split4 == NULL) {
        printf("Nr inválido de argumentos");
      }
      struct data_t *dados = data_create2(strlen(split4), split4);

      msg_out->table_num = (short)atoi(split2);
      msg_out->opcode = OC_UPDATE;
      msg_out->c_type = CT_ENTRY;
      msg_out->content.entry = (struct entry_t*) malloc(sizeof(struct entry_t));
      if(msg_out->content.entry == NULL){
        free_message(msg_out);
        data_destroy(dados);
        return -1;
      }
      msg_out->content.entry->key = strdup(split3);
      msg_out->content.entry->value = data_dup(dados);

      data_destroy(dados);
    }

    else if (strcmp(option, "size") == 0) {
      if (split2 == NULL) {
        printf("Nr inválido de argumentos");
      }
      msg_out->table_num = (short)atoi(split2);
      msg_out->opcode = OC_SIZE;
      msg_out->c_type = CT_RESULT;
    }

    else if (strcmp(option, "collisions") == 0) {
      if (split2 == NULL) {
        printf("Nr inválido de argumentos");
      }
      msg_out->table_num = (short)atoi(split2);
      msg_out->opcode = OC_COLLS;
      msg_out->c_type = CT_RESULT;
  }
    else if(strcmp(option, "ntables") == 0){
      msg_out->opcode = OC_NTABLES;
      msg_out->c_type = CT_RESULT;
    }

    else {
      printf("Essa opção não existe");
      return -1;
    }
    //////////////////////RECEBER/////////////////////
    msg_resposta = network_send_receive(server, msg_out);
    //Resposta ao put
    if(msg_resposta->opcode == OC_PUT + 1){
      if(msg_resposta->content.result == 0){
        printf("\nOperacao put com sucesso!");
        int datasize = msg_out->content.entry->value->datasize;
        char * data = malloc(datasize);
        memcpy(data,msg_out->content.entry->value->data,datasize);
        data[datasize+1] = '\0';
        printf("Inseriu a chave \"%s\" com a data \"%s\"\n", msg_out->content.entry->key, data);
        free(data);
      }
    }

    //Resposta ao get
    if(msg_resposta->opcode == OC_GET + 1){
      //Get individual em baixo
      if(msg_resposta->c_type == CT_VALUE){
        printf("\nOperacao get feita com sucesso!");

        if(msg_resposta->content.data->datasize == 0)
          printf("\n a data da key eh NULL\n");

        else{
          int datasize = msg_resposta->content.entry->value->datasize;
          char * data = malloc(datasize);
          memcpy(data,msg_resposta->content.data->data,datasize);
          data[datasize+1] = '\0';
          printf("\nA data dessa key eh : %s\n\n", data);
          free(data);
        }
      }
      if(msg_resposta->c_type == CT_KEYS){
        printf("\nAs chaves sao: ");
        int j = 0;
        while( msg_resposta->content.keys[j] != NULL){
          printf("%s ", msg_resposta->content.keys[j]);
          j++;
        }
        printf("\n\n");
      }
    }
      //PARA FUNCIONAR METER SPLIT2 NO ARG DO PRINTF
      if(msg_resposta->opcode == OC_UPDATE +1){
        printf("\nOperacao update feita com sucesso! Chave \"%s\"", split2);
        printf("\nFaça get para ver alterações!\n");
      }

      if(msg_resposta->opcode == OC_SIZE + 1){
        printf("\nOperacao size feita com sucesso!\n");
        printf("O numero de elementos na tabela eh: %d\n\n" , msg_resposta->content.result);
      }

      if(msg_resposta->opcode == OC_COLLS + 1){
        printf("\nOperacao collisions feita com sucesso!\n");
        printf("O numero de colisoes na tabela eh: %d\n\n" , msg_resposta->content.result);
      }

      if(msg_resposta->opcode == OC_NTABLES +1){
        printf("\nOperacao ntables feita com sucesso!\n");
        printf("O numero de tabelas eh: %d\n\n" , msg_resposta->content.result);
      }

    split2 = NULL;
    split3 = NULL;

    free_message(msg_out);
    free_message(msg_resposta);
  }// Ciclo while Acaba Aqui

  printf("Cliente Terminado \n");
  return network_close(server);
}
