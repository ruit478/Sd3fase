// Grupo 07
// Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
//

#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "table.h"

struct table_t {
  struct entry_t *entries;
  int nrElems;
  int maxSize;
  int colls;
};

int hash(char *key, int size);

#endif
