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
#include "client_stub-private.h"
int main(int argc, char **argv) {
  int result = 0;
  char input[81];
  struct message_t *msg_out;
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
  struct rtables_t *rtable = rtables_bind(argv[1]);
  if(rtable == NULL){
    return -1;
  }
  /* Fazer ciclo até que o utilizador resolva fazer "quit" */
  while (1) {
    printf(">>> "); // Mostrar a prompt para inserção de comando

    /* Receber o comando introduzido pelo utilizador
       Sugestão: usar fgets de stdio.h
       Quando pressionamos enter para finalizar a entrada no
       comando fgets, o carater \n é incluido antes do \0.
       Convém retirar o \n substituindo-o por \0.
    */
    fgets(input, 81, stdin); // Ver o size
    input[strlen(input) - 1] = '\0';
    /* Verificar se o comando foi "quit". Em caso afirmativo
       não há mais nada a fazer a não ser terminar decentemente.
     */
    if (strcmp(input, "quit") == 0) {
      break;
    }

    option = strtok(input, " "); // Tratar dos argumentos de entrada
    split2 = strtok(NULL, " ");  // Nr tabela

    msg_out = (struct message_t *)malloc(sizeof(struct message_t));
    if (msg_out == NULL) {
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
      rtable->activeTable = atoi(split2);
      split3 = strtok(NULL, " ");  // Key
      split4 = strtok(NULL, "\0"); // data
      if (split2 == NULL || split3 == NULL || split4 == NULL) {
        printf("Nr inválido de argumentos");
      }
      struct data_t *dados = data_create2(strlen(split4), split4);
      result = rtables_put(rtable,split3, dados);
      data_destroy(dados);
    
      if (result == -2)
        printf("\nO servidor não se encontra disponivel. Saia da aplicacao usando o comando \"quit\"\n");

      else if (result == -1)
        printf("\nOcorreu um erro no lado do servidor, tente novamente!\n");
  
      else
        printf("\nInseriu a chave \"%s\" com a data \"%s\"\n", split3, split4);
    }

    else if (strcmp(option, "get") == 0) {
      rtable->activeTable = atoi(split2);
      split3 = strtok(NULL, "\0");

      if (split2 == NULL || split3 == NULL) {
        printf("Nr Inválido de argumentos");
      }
      int i = 0;
      char **allKeys;
      if(strcmp(split3, "*") == 0){
        allKeys = rtables_get_keys(rtable);
        if(allKeys == NULL){
          printf("\nNão há chaves\n");
        }
        else{
          printf("Lista de chaves: ");
          while(allKeys[i] != NULL){
            printf("%s ", allKeys[i]);
          i++;
          }
          printf("\n");
          rtables_free_keys(allKeys);
        }
      }
      else{
        struct data_t *aux = rtables_get(rtable,split3);
        if(aux == NULL){
          printf("Nao ha chave");
        }
        else{
          char* data = (char *) malloc(aux->datasize);
          memcpy(data,aux->data,aux->datasize);
          data[aux->datasize] = '\0';
          printf("\nA data dessa key eh %s\n", data);
          free(data);
        }
      }
    }

    else if (strcmp(option, "update") == 0) {
      rtable->activeTable = atoi(split2);
      split3 = strtok(NULL, " ");  // Key
      split4 = strtok(NULL, "\0"); // Data

      if (split2 == NULL || split3 == NULL || split4 == NULL) {
        printf("Nr inválido de argumentos");
      }
      struct data_t *dados = data_create2(strlen(split4), split4);

      result = rtables_update(rtable,split3, dados);

      if (result == -2)
        printf("\nO servidor não se encontra disponivel. Saia da aplicacao usando o comando \"quit\"\n");

      else if (result == -1)
        printf("\nOcorreu um erro no lado do servidor, tente novamente!\n");
  
      else {
        printf("\nAtualizacao da chave \"%s\" com a data \"%s\"\n", split3, split4);
        printf("Pode conferir com o comando get!\n");

      }      
      data_destroy(dados); 
    }

    else if (strcmp(option, "size") == 0) {
      rtable->activeTable = atoi(split2);
      result = rtables_size(rtable);

      if (result == -2)
        printf("\nO servidor não se encontra disponivel. Saia da aplicacao usando o comando \"quit\"\n");

      else if (result == -1)
        printf("\nOcorreu um erro no lado do servidor, tente novamente!\n");
  
      else
        printf("\nA tabela tem %d elemento(s)!\n", result);
    }

    else if (strcmp(option, "collisions") == 0) {
      rtable->activeTable = atoi(split2);
      result = rtables_collisions(rtable);

      if (result == -2)
        printf("\nO servidor não se encontra disponivel. Saia da aplicacao usando o comando \"quit\"\n");

      else if (result == -1)
        printf("\nOcorreu um erro no lado do servidor, tente novamente!\n");
  
      else
        printf("\nA tabela tem %d colisoes!\n", result);
    } 
    else if (strcmp(option, "ntables") == 0) {
      result = rtables_get_ntables(rtable);

      if (result == -2)
        printf("\nO servidor não se encontra disponivel. Saia da aplicacao usando o comando \"quit\"\n");

      else if (result == -1)
        printf("\nOcorreu um erro no lado do servidor, tente novamente!\n");
  
      else
        printf("\nA tabela tem %d tabela(s)!\n", result);      
    }

    else {
      printf("Essa opção não existe");
      return -1;
    }

  } // Ciclo while Acaba Aqui

  printf("Cliente Terminado \n");
  return rtables_unbind(rtable);
}
