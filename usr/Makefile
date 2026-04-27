CC        := clang
ASM       := nasm
ASFLAGS   := -felf32

CFLAGS    := -target i686-elf -O0 -static -nostdlib -fno-pic -fno-pie -Wl,-no-pie
INCLUDES  := -Ilib
SRC_DIR   := src
LIB_DIR   := lib
BIN_DIR   := bin
LD_SCRIPT := $(SRC_DIR)/user.ld

CRT0_OBJ  := $(LIB_DIR)/crt0.o
LIB_SRCS  := $(wildcard $(LIB_DIR)/*.c)
PROGRAMS  := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(wildcard $(SRC_DIR)/*.c))

.PHONY: all clean

all: $(BIN_DIR) $(PROGRAMS)

$(BIN_DIR):
	mkdir -p $@

$(CRT0_OBJ): $(LIB_DIR)/crt0.s
	$(ASM) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%: $(SRC_DIR)/%.c $(LIB_SRCS) $(LD_SCRIPT) $(CRT0_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -T $(LD_SCRIPT) $(CRT0_OBJ) $< $(LIB_SRCS) -o $@

clean:
	rm -rf $(BIN_DIR)
	rm -f $(CRT0_OBJ)
