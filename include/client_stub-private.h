#include "client_stub.h"

struct rtables_t{
	struct server_t *server;
	char * address_port;
	int nrTables;
	int activeTable; //Guardar a tabela ativa no momento
}