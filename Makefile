FREQUENCIES=433 470 868 915
XRAM_SIZE=4096
MEMORY_MODEL:=--model-large
LDFLAGS+=$(MEMORY_MODEL) --iram-size 256 --xram-size $(XRAM_SIZE) --code-loc 0x400 --code-size 0x00f400 --stack-size 64
BOOTLDFLAGS=--iram-size 256 --xram-size $(XRAM_SIZE) --stack-size 64 --nostdlib -Wl -bHIGHCSEG=0xf800 

OBJ_DIR=./obj
BIN_DIR=./bin
SRC_DIR=./src
INCLUDE_DIR=./include

SRCS:=$(wildcard $(SRC_DIR)/*.c)

$(info SRCS is $(SRCS))

OBJS:=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.rel, $(SRCS))
AMS:=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.rel, $(SRCS))

$(info OBJS is $(OBJS))

CC=sdcc -mmcs51
AS=sdas8051 -jlocsp
LD=sdcc
INCLUDES:=-I$(INCLUDE_DIR)
CFLAGS+=--opt-code-speed --std-sdcc99 --fomit-frame-pointer $(MEMORY_MODEL) $(INCLUDES)
CFLAGS+=-D BUILD_DATE=\""$(shell date)"\" -D BUILD_COMMIT=\""$(shell git rev-parse --short HEAD)"\"
DEPFLAGS:=-MM $(CFLAGS) 
HEX=$(BIN_DIR)/firmware.ihx

all: $(HEX)

$(HEX): $(OBJS) | $(BIN_DIR)
	$(LD) $(LDFLAGS) -o $@ $^
	
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@
	
$(OBJ_DIR)/%.rel: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)