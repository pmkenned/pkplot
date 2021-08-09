CC = gcc
CPPFLAGS += -MMD
CFLAGS += -Wall -Wextra -std=c99
LDFLAGS +=
LDLIBS +=

SRC_DIR = ./src
BUILD_DIR = ./build
TEST_DIR = $(BUILD_DIR)/test

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)

TEST_OBJ = $(SRC:$(SRC_DIR)/%.c=$(TEST_DIR)/%.o)
TEST_DEP = $(TEST_OBJ:%.o=%.d)

TARGET = pkplot
TEST_TARGET = test_$(TARGET)

.PHONY: all run test clean

#CFLAGS += -Wno-unused-function  -Wno-unused-parameter
CFLAGS += -Wno-unused-const-variable
#CFLAGS += -O3
CFLAGS += -g

all: $(BUILD_DIR)/$(TARGET)

OUT  ?= ./out.png
DATA ?= ./data/example1.dat

run: all $(DATA)
	$(BUILD_DIR)/$(TARGET) -o $(OUT) $(DATA)

test: CPPFLAGS += -DTEST
test: $(TEST_DIR)/$(TEST_TARGET)
	$(TEST_DIR)/$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR)

-include $(DEP)
-include $(TEST_DEP)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(TARGET): $(OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(TEST_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(TEST_DIR)/$(TEST_TARGET): $(TEST_OBJ)
	mkdir -p $(TEST_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
