CC        := clang
CFLAGS    := -target i686-elf -O0 -static -nostdlib -fno-pic -fno-pie -Wl,-no-pie
INCLUDES  := -Ilib

SRC_DIR   := src
LIB_DIR   := lib
BIN_DIR   := bin
LD_SCRIPT := $(SRC_DIR)/user.ld

LIB_SRCS  := $(wildcard $(LIB_DIR)/*.c)

PROGRAMS  := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(wildcard $(SRC_DIR)/*.c))

.PHONY: all clean

all: $(BIN_DIR) $(PROGRAMS)

$(BIN_DIR):
	mkdir -p $@

$(BIN_DIR)/%: $(SRC_DIR)/%.c $(LIB_SRCS) $(LD_SCRIPT) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -T $(LD_SCRIPT) $< $(LIB_SRCS) -o $@

clean:
	rm -rf $(BIN_DIR)

