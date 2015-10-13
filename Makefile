IND_TYPE = _CC
USE_TRANS = _NO
PINOUT = _PIN1

# Lowercase argument
lc = $(shell echo $1 | tr A-Z a-z)

# Output file name
ifeq ($(IND_TYPE), _NIXIE)
TARG = fm7segm$(call lc,$(PINOUT))$(call lc,$(IND_TYPE))
else
TARG = fm7segm$(call lc,$(PINOUT))$(call lc,$(IND_TYPE))$(call lc,$(USE_TRANS))
endif

# MCU name and frequency
MCU      = atmega8
F_CPU    = 8000000

# Source files
TUNER_SRC = $(wildcard tuner/*.c)
SRCS     = $(wildcard *.c) $(TUNER_SRC)

# Build directory
BUILDDIR = build

# Compiler options
OPTIMIZE = -Os -mcall-prologues -fshort-enums -ffunction-sections -fdata-sections
DEBUG    = -g -Wall -Werror
DEPS     = -MMD -MP -MT $(BUILDDIR)/$(*F).o -MF $(BUILDDIR)/$(*D)/$(*F).d
CFLAGS   = $(DEBUG) -lm $(OPTIMIZE) $(DEPS) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS  = $(DEBUG) -mmcu=$(MCU) -Wl,-gc-sections -mrelax

# AVR toolchain and flasher
CC       = avr-gcc
OBJCOPY  = avr-objcopy
OBJDUMP  = avr-objdump

# AVRDude parameters
AVRDUDE  = avrdude
AD_MCU   = -p $(MCU)
#AD_PROG = -c stk500v2
#AD_PORT = -P avrdoper

AD_CMD   = $(AD_MCU) $(AD_PROG) $(AD_PORT) -V

# Build objects
OBJS     = $(addprefix $(BUILDDIR)/, $(SRCS:.c=.o))
ELF      = $(BUILDDIR)/$(TARG).elf

# Dependencies
-include $(OBJS:.o=.d)

all: $(ELF) size

$(ELF): $(OBJS)
	@mkdir -p $(BUILDDIR) flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJS) -lm
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram $(ELF) flash/$(TARG).hex
	$(OBJDUMP) -h -S $(ELF) > $(BUILDDIR)/$(TARG).lss

size:
	@sh ./size.sh $(ELF)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D$(IND_TYPE) -D$(USE_TRANS) -D$(PINOUT) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: flash
flash: $(ELF)
	$(AVRDUDE) $(AD_CMD) -U eeprom:w:eeprom/fm7segm.bin:r

.PHONY: eeprom
eeprom: $(ELF)
	$(AVRDUDE) $(AD_CMD) -U flash:w:flash/$(TARG).hex:i

.PHONY: fuse
fuse:
	$(AVRDUDE) $(AD_CMD) -U lfuse:w:0x24:m -U hfuse:w:0xC1:m

