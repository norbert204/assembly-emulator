CC := gcc
CFLAGS :=

TARGET_EXEC := asemu

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

MKDIR_P := mkdir -p
RM_RF := rm -rf

default: debug

# This is a bit ugly
debug: CFLAGS += -DDEBUG -Wall
debug: OBJ_DIR := $(OBJ_DIR)/debug # This doesn't work
debug: BIN_DIR := $(BIN_DIR)/debug
debug: TARGET_EXEC := $(BIN_DIR)/$(TARGET_EXEC)
debug: all

release: CFLAGS += -O2
release: OBJ_DIR := $(OBJ_DIR)/release # This doesn't work either
release: BIN_DIR := $(BIN_DIR)/release
release: TARGET_EXEC := $(BIN_DIR)/$(TARGET_EXEC)
release: all

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(addsuffix .o, $(basename $(SRCS))))

.PHONY: all
all: $(OBJS)
	@echo "[\e[32mLinking\e[0m] $(TARGET_EXEC)"

	@$(MKDIR_P) $(BIN_DIR)
	@$(CC) $(OBJS) -o $(TARGET_EXEC)
	@chmod +x $(TARGET_EXEC)

	@echo "\n\e[32mBuild successful!\e[0m"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "[\e[32mBuilding\e[0m] $< -> $@"

	@$(MKDIR_P) $(dir $@)
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@$(RM_RF) $(OBJ_DIR)/
	@$(RM_RF) $(BIN_DIR)/
