
#TODO: 
#Preciso criar um 'Makefile' para compilação em outros lugares, mas no vscode funciona
# com as extensões para C/c++. Preciso averiguar pq nenhum Makefile fuciona.

CC = gcc
CFLAGS = -Wall -g3 -fsanitize=address -pthread
LDFLAGS = -L /lib64
LIBS = -lusb-1.0 -l pthread

SERVER_SRCS = server.c
CLIENT_SRCS = client.c

SERVER_OBJ = $(SERVER_SRCS:.c=.o)
CLIENT_OBJ = $(CLIENT_SRCS:.c=.o)

SERVER_TARGET = server
CLIENT_TARGET = client

.PHONY: all clean

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJ) $(LIBS)

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJ) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET) $(SERVER_OBJ) $(CLIENT_OBJ)

