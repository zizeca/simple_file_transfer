CC = gcc
CFLAGS = -Wall -g

CLIENT_TARGET := build/client
SERVER_TARGET := build/server

CLIENT_OUTPUT := build/client
SERVER_OUTPUT := build/server

CLIENT_OUTPUT_OBJ := build/obj/client
SERVER_OUTPUT_OBJ := build/obj/server

CLIENT_SOURCES := $(wildcard src/client/*.c)
SERVER_SOURCES := $(wildcard src/server/*.c)

CLIENT_OBJECTS := $(patsubst src/client/%.c, $(CLIENT_OUTPUT_OBJ)/%.o, $(CLIENT_SOURCES))
SERVER_OBJECTS := $(patsubst src/server/%.c, $(SERVER_OUTPUT_OBJ)/%.o, $(SERVER_SOURCES))


all: dirs client server

client: $(CLIENT_OBJECTS)
	$(CC) $(CCFLAGS)  $(CLIENT_OBJECTS) -o $(CLIENT_TARGET)

server: $(SERVER_OBJECTS)
	$(CC) $(CCFLAGS) $(SERVER_OBJECTS) -o $(SERVER_TARGET) 

$(CLIENT_OUTPUT_OBJ)/%.o: src/client/%.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(SERVER_OUTPUT_OBJ)/%.o: src/server/%.c
	$(CC) $(CCFLAGS) -c $< -o $@


dirs:
	mkdir -p $(CLIENT_OUTPUT_OBJ)
	mkdir -p $(SERVER_OUTPUT_OBJ)

clean:
	rm -rf build/*