IND_TYPE = _CC
USE_TRANS = _NO

# Lowercase argument
lc = $(shell echo $1 | tr A-Z a-z)

TARG = fm7segm$(call lc,$(IND_TYPE))$(call lc,$(USE_TRANS))

MCU = atmega8
F_CPU = 8000000

# Source files

TUNER_SRC = $(wildcard tuner/*.c)

SRCS = $(wildcard *.c) $(TUNER_SRC)

OPTIMIZE = -Os -mcall-prologues -fshort-enums
DEBUG = -g -Wall -Werror
CFLAGS = $(DEBUG) -lm $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS = $(DEBUG) -mmcu=$(MCU)

CC = avr-gcc
OBJCOPY = avr-objcopy

AVRDUDE = avrdude
AD_MCU = -p $(MCU)
#AD_PROG = -c stk500v2
#AD_PORT = -P avrdoper

AD_CMDLINE = $(AD_MCU) $(AD_PROG) $(AD_PORT) -V

OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
ELF = $(OBJDIR)/$(TARG).elf

all: $(TARG)

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJS) -lm
	mkdir -p flash
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram $(ELF) flash/$@.hex
	./size.sh $(ELF)

obj/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D$(IND_TYPE) -D$(USE_TRANS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)

flash: $(TARG)
	$(AVRDUDE) $(AD_CMDLINE) -U flash:w:flash/$(TARG).hex:i

eeprom:
	$(AVRDUDE) -p $(MCU) -U eeprom:w:fm7segm.bin:r

fuse:
	$(AVRDUDE) -p $(MCU) -U lfuse:w:0x24:m -U hfuse:w:0xc1:m

