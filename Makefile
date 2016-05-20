BIN = space_simulator
OBJ_DIR = obj
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/gfx.o $(OBJ_DIR)/polygon.o $(OBJ_DIR)/body.o $(OBJ_DIR)/vector_fns.o
CC = g++
CFLAGS = -std=c++14 -O2 -c

INCLUDE_DIRECTORIES = -Iinclude
LIBRARY_DIRECTORIES = -Llib
LIBRARIES = -lSOIL -lGL -lglfw -lGLEW

.PHONY : all clean
all: $(OBJS)
	$(CC) $(OBJS) $(LIBRARY_DIRECTORIES) $(LIBRARIES) -o $(BIN)

debug: CFLAGS += -g -D DEBUG_MODE
debug: clean all

$(OBJ_DIR)/main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp $(INCLUDE_DIRECTORIES) -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/gfx.o: src/gfx.cpp
	$(CC) $(CFLAGS) src/gfx.cpp $(INCLUDE_DIRECTORIES) -o $(OBJ_DIR)/gfx.o

$(OBJ_DIR)/polygon.o: src/polygon.cpp
	$(CC) $(CFLAGS) src/polygon.cpp $(INCLUDE_DIRECTORIES) -o $(OBJ_DIR)/polygon.o

$(OBJ_DIR)/body.o: src/body.cpp
	$(CC) $(CFLAGS) src/body.cpp $(INCLUDE_DIRECTORIES) -o $(OBJ_DIR)/body.o

$(OBJ_DIR)/vector_fns.o: src/vector_fns.cpp
	$(CC) $(CFLAGS) src/vector_fns.cpp $(INCLUDE_DIRECTORIES) -o $(OBJ_DIR)/vector_fns.o

run: all
	./$(BIN)

clean:
	rm $(BIN) $(OBJS)
