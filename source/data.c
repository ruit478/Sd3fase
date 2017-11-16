// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size
 */
struct data_t *data_create(int size) {

  if (size <= 0)
    return NULL;

  struct data_t *temp = (struct data_t *)malloc(sizeof(struct data_t));

  if (temp == NULL) {
    return NULL;
  }

  temp->datasize = size;
  temp->data = malloc(size);

  if (temp->data == NULL) {
    free(temp->data);
    free(temp);
    return NULL;
  }
  return temp;
}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void *data) {

  if (data == NULL || size <= 0) {
    return NULL;
  }

  struct data_t *temp = data_create(size);

  if (temp == NULL) {
    free(temp);
    return NULL;
  }

  memcpy(temp->data, data, size);

  return temp;
}

/* Função que destrói um bloco de dados e liberta toda a memória.
 */
void data_destroy(struct data_t *data) {

  if (data == NULL) {
    return;
  }
  free(data->data);
  free(data);
}

/* Função que duplica uma estrutura data_t.
 */
struct data_t *data_dup(struct data_t *data) {

  if (data == NULL)
    return NULL;
  return data_create2(data->datasize, data->data);
}
