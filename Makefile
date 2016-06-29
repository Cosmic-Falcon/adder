# General variables
CC := g++
AR := ar
CFLAGS := -std=c++14 -O2 -c
ARFLAGS := rcs

# Library variables
OUT := adder
LIB_OUT := $(shell echo $(OUT) | tr a-z A-Z)# Make library name uppercase
OUT_DIR := out

SRC_DIR := src
OBJ_DIR := obj
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

INCL_DIRS := -Iinclude
LIB_DIRS := -Llib
LIBS := -lGL -lglfw -lGLEW -lBOA

# Test variables
TEST_BIN := $(OUT)_test
TEST_DIR := test

TEST_SRCS := $(wildcard $(TEST_DIR)/$(SRC_DIR)/*.cpp)
TEST_OBJS := $(TEST_SRCS:$(TEST_DIR)/$(SRC_DIR)/%.cpp=$(TEST_DIR)/$(OBJ_DIR)/%.o)

TEST_INCL_DIRS := -I$(OUT_DIR)
TEST_LIB_DIRS := -L$(OUT_DIR)
TEST_LIBS := -lGL -lglfw -lGLEW -lADDER -lBOA

.PHONY : all clean

# Library targets
all: $(OBJS)
	$(AR) $(ARFLAGS) $(OUT_DIR)/lib$(LIB_OUT).a $(OBJS)
	cp $(SRC_DIR)/*.h $(OUT_DIR)/$(OUT)

shared: CFLAGS += -fpic
shared: $(OBJS)
	$(CC) -shared $(OBJS) $(INCL_DIRS) $(LIB_DIRS) $(LIBS) -o $(OUT_DIR)/lib$(LIB_OUT).so # Not sure if linking libraries works when compiling shared libraries
	cp $(SRC_DIR)/*.h $(OUT_DIR)/$(OUT)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $< $(INCL_DIRS) -o $@

# Test targets
$(TEST_DIR)/$(OBJ_DIR)/%.o: $(TEST_DIR)/$(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $< $(INCL_DIRS) $(TEST_INCL_DIRS) -o $@

test: $(TEST_OBJS)
	$(CC) $(TEST_OBJS) $(INCL_DIRS) $(TEST_INCL_DIRS) $(LIB_DIRS) $(TEST_LIB_DIRS) $(TEST_LIBS) -o $(TEST_DIR)/$(TEST_BIN)

run:
	cd $(TEST_DIR); ./$(TEST_BIN)

# General targets
debug: CFLAGS += -g -D DEBUG_MODE
debug: clean all

clean:
	rm -f $(OUT_DIR)/lib$(LIB_OUT).* $(OBJS)
	rm -f $(OUT_DIR)/$(OUT)/*.h
	rm -f $(TEST_DIR)/$(OBJ_DIR)/*.o
