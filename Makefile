CC = gcc
CPPFLAGS += -MMD
CFLAGS += -Wall -Wextra -std=c99
LDFLAGS +=
LDLIBS +=

SRC_DIR = ./src
BUILD_DIR = ./build

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)

TARGET = pkplot

.PHONY: all run clean

.PHONY: test

CFLAGS += -Wno-unused-function -Wno-unused-const-variable -Wno-unused-parameter
#CFLAGS += -O3
CFLAGS += -g
all: $(BUILD_DIR)/$(TARGET)

run: all
	$(BUILD_DIR)/$(TARGET)

#test:
#	$(BUILD_DIR)/test

clean:
	rm -rf $(BUILD_DIR)

-include $(DEP)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(TARGET): $(OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

#$(BUILD_DIR)/test: 
