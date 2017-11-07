#include "table_skel.h"
#include <stdlib.h>
/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_tables define o número e dimensão das
 * tabelas a serem mantidas no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
//Declarar var global 
struct table_t * tables;

int table_skel_init(char **n_tables){
	
	if(n_tables == NULL){
		return -1;
	}
	tables = (struct table_t*) malloc(sizeof(struct table_t));
	if(tables == NULL){
		return -1;
	}

	int i = 0;
	while(n_tables[i] != NULL){
		tables[i] = *table_create(atoi(n_tables[i]));
		i++;
	}
	tables->nrTables = i;

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


struct message_t *invoke(struct message_t *msg_in){
	struct message_t *msg_resposta;

	msg_resposta = (struct message_t *) malloc(sizeof(struct message_t));

	/* Verificar opcode e c_type na mensagem de pedido */
	if ((msg_in->opcode < 10) || (msg_in->opcode > 70)){
		return NULL;
	}

	switch(msg_in->opcode){
		case OC_PUT:
		msg_resposta->table_num = msg_in->table_num;
		msg_resposta->opcode = OC_PUT;
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = table_put(tables[msg_in->table_num], msg_in->content.entry->key, msg_in->content.entry->value);
		if(msg_resposta->content.result == -1){
			printf("Não consegui inserir na tabela\n");
			msg_resposta->opcode = OC_RT_ERROR;
		}
		else{
			msg_resposta->opcode = msg_resposta->opcode +1;
		}
		break;

		case OC_GET:
		msg_resposta->table_num = msg_in->table_num;
		msg_resposta->opcode = OC_GET;
		if(strcmp(msg_in->content.key,"*") == 0){
			msg_resposta->c_type = CT_KEYS;
			msg_resposta->content.keys = table_get_keys(tables[msg_in->table_num]);
			msg_resposta->opcode = msg_resposta->opcode + 1;
		}
		//Caso de só querer 1 key
		else{
			msg_resposta->c_type = CT_VALUE;
			struct data_t *dados = table_get(tables[msg_in->table_num],msg_in->content.key);
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
		msg_resposta->table_num = msg_in->table_num;
		msg_resposta->opcode = OC_UPDATE;
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = table_update(tables[msg_in->table_num],msg_pedido->content.entry->key,msg_pedido->content.entry->value);
		if(msg_resposta->content.result == -1){
			printf("Erro ao fazer update");
			msg_resposta->opcode = OC_RT_ERROR;
		}
		else{
			msg_resposta->opcode = msg_resposta->opcode +1;
		}
		break;

		case OC_SIZE:
		msg_resposta->table_num = msg_in->table_num;
		msg_resposta->opcode = OC_SIZE; //Tou aqui
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = table_size(tables[msg_in->table_num]);
		if(msg_resposta->content.result == -1){
			printf("Erro ao calcular o size da tabela");
			msg_resposta->opcode = OC_RT_ERROR;
		}
		else{
			msg_resposta->opcode = msg_resposta->opcode +1;
		}
		break;

		case OC_COLLS:
		msg_resposta->table_num = msg_in->table_num;
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = tables[msg_in->table_num]->colls;
		msg_resposta->opcode = OC_COLLS +1;
		break;

		case OC_NTABLES:
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = tables->nrTabelas;
		msg_resposta->opcode = OC_NTABLES;
	}
	return msg_resposta;
}