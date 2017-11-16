// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entry.h"

/* Função que inicializa os membros de uma entrada na tabela com
   o valor NULL.
 */
void entry_initialize(struct entry_t *entry) {

  if (entry == NULL)
    return;

  entry->key = NULL;
  entry->value = NULL;
  entry->next = NULL;
}

/* Função que duplica um par {chave, valor}. */
struct entry_t *entry_dup(struct entry_t *entry) {

  if (entry == NULL || entry->key == NULL || entry->value == NULL)
    return NULL;

  struct entry_t *temp = (struct entry_t *)malloc(sizeof(struct entry_t));

  if (temp == NULL) {
    free(temp);
    return NULL;
  }

  temp->key = strdup(entry->key);
  temp->value = data_dup(entry->value);
  memcpy(&(temp->next), &(entry->next), sizeof(temp->next));

  return temp;
}
