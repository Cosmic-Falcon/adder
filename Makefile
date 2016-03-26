BIN = space_simulator
OBJS = src/main.cpp src/gfx.cpp src/polygon.cpp

CC = g++
CFLAGS = -std=c++14 -O2

INCLUDE_DIRECTORIES = -Iinclude
LIBRARY_DIRECTORIES = -Llib
LIBRARIES = -lSOIL -lGL -lglfw -lGLEW

.PHONY : all
all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(INCLUDE_DIRECTORIES) $(LIBRARY_DIRECTORIES) $(LIBRARIES) -o $(BIN)

debug: $(OBJS)
	$(CC) $(CFLAGS) -g -D DEBUG_MODE $(OBJS) $(INCLUDE_DIRECTORIES) $(LIBRARY_DIRECTORIES) $(LIBRARIES) -o $(BIN)

run: all
	./$(BIN)
