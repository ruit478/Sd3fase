#include "table_skel.h"
#include <stdlib.h>
/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_tables define o número e dimensão das
 * tabelas a serem mantidas no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */

struct table_t * tables;

int table_skel_init(char **n_tables){
	
	if(n_tables == NULL){
		return -1;
	}
	int nrTables = atoi(n_tables[0]);
	int index = 0;
	tables = (struct table_t*) malloc(sizeof(struct table_t) * (nrTables));
	if(tables == NULL){
		return -1;
	}
	for(int i = 1; i < nrTables; i++){
		tables[index] = *table_create(atoi(n_tables[i]));
		index++;
	}

	return 0;
}

int table_skel_destroy(){
	int index = 0;
	int i = 0;

	while(&tables[i]!= NULL){
		table_destroy(&tables[index]);
		index++;
	}

	table_destroy(tables);
	return 0;
}