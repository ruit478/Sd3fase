// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table-private.h"
#include "table.h"

/* Função para criar/inicializar uma nova tabela hash, com n
 * linhas(n = módulo da função hash)
 */
struct table_t *table_create(int n) {

  if (n <= 0)
    return NULL;

  struct table_t *table = (struct table_t *)malloc(sizeof(struct table_t));

  if (table == NULL) {
    free(table);
    return NULL;
  }

  table->nrElems = 0;
  table->maxSize = n;
  table->entries = (struct entry_t *)malloc(n * sizeof(struct entry_t));

  if (table->entries == NULL) {
    free(table);
    return NULL;
  }

  for (int i = 0; i < table->maxSize; i++) {
    entry_initialize(&(table->entries[i]));
  }

  return table;
}

/* Libertar toda a memória ocupada por uma tabela.
 */
void table_destroy(struct table_t *table) {

  if (table == NULL)
    return;

  if (table->maxSize <= 0) {
    free(table);
    return;
  }

  for (int i = 0; i < (table->maxSize); i++) {
    free(table->entries[i].key);
    data_destroy(table->entries[i].value);
    table->nrElems--;
  }

  free(table->entries);
  free(table);
}

/* Função para adicionar um par chave-valor na tabela.
 * Os dados de entrada desta função deverão ser copiados.
 * Devolve 0 (ok) ou -1 (out of memory, outros erros)
 */
int table_put(struct table_t *table, char *key, struct data_t *value) {

  if (table == NULL || key == NULL || value == NULL)
    return -1;

  int index = hash(key, table->maxSize);
  struct entry_t *curr = &(table->entries[index]);

  if (table->nrElems == table->maxSize)
    return -1;

  if (curr->key == NULL) {
    curr->key = strdup(key);
    curr->value = data_dup(value);
    table->nrElems++;
    return 0;
  }

  while (curr->next != NULL) {
    if (strcmp(curr->key, key) == 0)
      return -1;
    curr = curr->next;
  }
  if (strcmp(curr->key, key) == 0)
    return -1;

  int i = table->maxSize - 1;
  while (table->entries[i].key != NULL) {
    i--;
  }

  table->entries[i].key = strdup(key);
  table->entries[i].value = data_dup(value);
  curr->next = &(table->entries[i]);
  table->nrElems++;

  return 0;
}

/* Função para substituir na tabela, o valor associado à chave key.
 * Os dados de entrada desta função deverão ser copiados.
 * Devolve 0 (OK) ou -1 (out of memory, outros erros)
 */
int table_update(struct table_t *table, char *key, struct data_t *value) {

  if (table == NULL || key == NULL || value == NULL)
    return -1;

  int index = hash(key, table->maxSize);
  struct entry_t *curr = table->entries + index;

  while (curr != NULL) {
    if (strcmp(curr->key, key) == 0) {
      data_destroy(curr->value);
      curr->value = data_dup(value);
      return 0;
    }
    curr = curr->next;
  }

  return -1;
}

/* Função para obter da tabela o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser libertados
 * no contexto da função que chamou table_get.
 * Devolve NULL em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key) {

  if (key == NULL || table == NULL)
    return NULL;

  int index = hash(key, table->maxSize);
  struct entry_t *curr = table->entries + index;

  while (curr->key != NULL) {
    if (strcmp(curr->key, key) == 0)
      return data_dup(curr->value);
    curr = curr->next;
  }

  return NULL;
}

/* Devolve o número de elementos na tabela.
 */
int table_size(struct table_t *table) {

  if (table == NULL)
    return 0;

  return table->nrElems;
}

/* Devolve um array de char * com a cópia de todas as keys da tabela,
 * e um último elemento a NULL.
 */
char **table_get_keys(struct table_t *table) {

  if (table == NULL)
    return NULL;

  char **allKeys = (char **)malloc(sizeof(char *) * (table->nrElems + 1));

  if (allKeys == NULL) {
    return NULL;
  }

  int temp = 0;
  for (int i = 0; i < table->maxSize; i++) {
    if (table->entries[i].key != NULL) {
      allKeys[temp] = strdup(table->entries[i].key);
      if (temp + 1 != table->maxSize) {
        temp++;
      }
    }
  }
  allKeys[temp + 1] = NULL;

  return allKeys;
}

/* Liberta a memória alocada por table_get_keys().
 */
void table_free_keys(char **keys) {

  if (keys != NULL) {
    int i = 0;
    while (keys[i] != '\0') {
      free(keys[i]);
      i++;
    }
    free(keys);
  }
}

int hash(char *key, int size) {

  int soma = 0;
  int len = strlen(key);
  if (len <= 4) {
    for (int i = 0; i < len; i++) {
      soma += (int)key[i];
    }
    return (soma % (size));
  } else {
    soma = (int)key[0] + (int)key[1] + (int)key[len - 2] + (int)key[len - 1];
    return (soma % (size));
  }
}
