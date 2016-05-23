BIN := argon
CC := g++
CFLAGS := -std=c++14 -O2 -c

SRC_DIR := src
OBJ_DIR := obj
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

INCLUDE_DIRECTORIES := -Iinclude
LIBRARY_DIRECTORIES := -Llib
LIBRARIES := -lSOIL -lGL -lglfw -lGLEW

.PHONY : all clean
all: $(OBJS)
	$(CC) $(OBJS) $(LIBRARY_DIRECTORIES) $(LIBRARIES) -o $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $< $(INCLUDE_DIRECTORIES) -o $@

debug: CFLAGS += -g -D DEBUG_MODE
debug: clean all

run: all
	./$(BIN)

clean:
	rm -f $(BIN) $(OBJS)
