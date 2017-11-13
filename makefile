#Grupo 07
#Rui Lopes 47900 Rui Teixeira 47889 João Miranda 48666
#

CC = gcc
SRC_DIR = source
INC_DIR = include
OBJ_DIR = object
BIN_DIR = binary
CFLAGS = -g -Wall -I $(INC_DIR)

all: $(OBJ_DIR)/table.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o $(OBJ_DIR)/message.o $(OBJ_DIR)/test_data.o $(OBJ_DIR)/test_entry.o $(OBJ_DIR)/test_table.o $(OBJ_DIR)/test_message.o $(OBJ_DIR)/network_client.o $(OBJ_DIR)/table-client.o $(OBJ_DIR)/table-server.o $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/table_skel.o $(BIN_DIR)/table-client $(BIN_DIR)/table-server $(BIN_DIR)/test_data $(BIN_DIR)/test_entry $(BIN_DIR)/test_table $(BIN_DIR)/test_message

# Compilar Objetos----------------------------------------------

$(OBJ_DIR)/data.o: $(SRC_DIR)/data.c $(INC_DIR)/data.h
				$(CC) $(CFLAGS) -c $(SRC_DIR)/data.c -o $(OBJ_DIR)/data.o

$(OBJ_DIR)/entry.o: $(SRC_DIR)/entry.c $(INC_DIR)/entry.h
				$(CC) $(CFLAGS) -c $(SRC_DIR)/entry.c -o $(OBJ_DIR)/entry.o

$(OBJ_DIR)/table.o: $(SRC_DIR)/table.c $(INC_DIR)/table-private.h
				 $(CC) $(CFLAGS) -c $(SRC_DIR)/table.c -o $(OBJ_DIR)/table.o

$(OBJ_DIR)/message.o: $(SRC_DIR)/message.c $(INC_DIR)/message.h
				$(CC) $(CFLAGS) -c $(SRC_DIR)/message.c -o $(OBJ_DIR)/message.o

$(OBJ_DIR)/test_data.o: $(INC_DIR)/data.h
					$(CC) $(CFLAGS) -c $(SRC_DIR)/test_data.c -o $(OBJ_DIR)/test_data.o

$(OBJ_DIR)/test_entry.o: $(INC_DIR)/entry.h $(INC_DIR)/data.h
					$(CC) $(CFLAGS) -c $(SRC_DIR)/test_entry.c -o $(OBJ_DIR)/test_entry.o

$(OBJ_DIR)/test_table.o: $(INC_DIR)/table.h $(INC_DIR)/table-private.h $(INC_DIR)/entry.h $(INC_DIR)/data.h
					$(CC) $(CFLAGS) -c $(SRC_DIR)/test_table.c -o $(OBJ_DIR)/test_table.o

$(OBJ_DIR)/test_message.o: $(INC_DIR)/message.h $(INC_DIR)/table.h $(INC_DIR)/table-private.h $(INC_DIR)/entry.h $(INC_DIR)/data.h
					$(CC) $(CFLAGS) -c $(SRC_DIR)/test_message.c -o $(OBJ_DIR)/test_message.o

$(OBJ_DIR)/network_client.o:$(INC_DIR)/network_client-private.h $(INC_DIR)/network_client.h $(INC_DIR)/message-private.h $(INC_DIR)/message.h $(INC_DIR)/table-private.h $(INC_DIR)/table.h $(INC_DIR)/data.h
					$(CC) $(CFLAGS) -c $(SRC_DIR)/network_client.c -o $(OBJ_DIR)/network_client.o

$(OBJ_DIR)/client_stub.o:$(INC_DIR)/client_stub-private.h $(INC_DIR)/client_stub.h $(INC_DIR)/data.h
					$(CC) $(CFLAGS) -c $(SRC_DIR)/client_stub.c -o $(OBJ_DIR)/client_stub.o

$(OBJ_DIR)/table_skel.o:
					$(CC) $(CFLAGS) -c $(SRC_DIR)/table_skel.c -o $(OBJ_DIR)/table_skel.o

$(OBJ_DIR)/table-client.o:$(INC_DIR)/network_client-private.h $(INC_DIR)/network_client.h $(INC_DIR)/message-private.h $(INC_DIR)/message.h $(INC_DIR)/table-private.h $(INC_DIR)/table.h $(INC_DIR)/data.h
					$(CC) $(CFLAGS) -c $(SRC_DIR)/table-client.c -o $(OBJ_DIR)/table-client.o

$(OBJ_DIR)/table-server.o:$(INC_DIR)/inet.h $(INC_DIR)/network_client-private.h $(INC_DIR)/network_client.h $(INC_DIR)/message-private.h $(INC_DIR)/message.h $(INC_DIR)/table-private.h $(INC_DIR)/table.h $(INC_DIR)/data.h
					$(CC) $(CFLAGS) -c $(SRC_DIR)/table-server.c -o $(OBJ_DIR)/table-server.o

# Compilar Testes ----------------------------------------------

$(BIN_DIR)/test_data:
	gcc $(OBJ_DIR)/test_data.o $(OBJ_DIR)/data.o -o $(BIN_DIR)/test_data

$(BIN_DIR)/test_entry:
	gcc $(OBJ_DIR)/test_entry.o $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o -o $(BIN_DIR)/test_entry

$(BIN_DIR)/test_table:
	gcc $(OBJ_DIR)/test_table.o $(OBJ_DIR)/table.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o -o $(BIN_DIR)/test_table

$(BIN_DIR)/test_message:
	gcc $(OBJ_DIR)/table.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o $(OBJ_DIR)/test_message.o $(OBJ_DIR)/message.o -o $(BIN_DIR)/test_message

$(BIN_DIR)/table-client:
	gcc $(OBJ_DIR)/table-client.o $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/network_client.o $(OBJ_DIR)/message.o $(OBJ_DIR)/table.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o -o $(BIN_DIR)/table-client

$(BIN_DIR)/table-server:
	gcc $(OBJ_DIR)/table-server.o $(OBJ_DIR)/table_skel.o $(OBJ_DIR)/message.o $(OBJ_DIR)/data.o $(OBJ_DIR)/table.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/network_client.o -o $(BIN_DIR)/table-server
# CLEAR -------------------------------------------------------

clean:
			rm -f ./$(OBJ_DIR)/* | rm -f ./$(BIN_DIR)/*
			clear
