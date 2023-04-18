CC = gcc
CCFLAGS = -std=c11 -Wall -g -Wformat -Wpedantic

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

client: $(CLIENT_SOURCES)
	$(CC) $(CCFLAGS)  $(CLIENT_SOURCES) -o $(CLIENT_TARGET)

server: $(SERVER_SOURCES)
	$(CC) $(CCFLAGS) $(SERVER_SOURCES) -o $(SERVER_TARGET) 

dirs:
	mkdir -p $(CLIENT_OUTPUT_OBJ)
	mkdir -p $(SERVER_OUTPUT_OBJ)

clean:
	rm -rf build/*