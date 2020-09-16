PINOUT = PIN1

# Lowercase argument
lc = $(shell echo $1 | tr A-Z a-z)

# Output file name
TARG = fm7segm_$(call lc,$(PINOUT))

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

# Main definitions
DEFINES  += -D_$(PINOUT)
# Supported tuners
DEFINES += -D_TEA5767 -D_RDA580X -D_TUX032

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
HEX      = flash/$(TARG).hex

all: $(HEX) size

$(ELF): $(OBJS)
	@mkdir -p $(BUILDDIR) flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJS) -lm
	$(OBJDUMP) -h -S $(ELF) > $(BUILDDIR)/$(TARG).lss

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram $(ELF) $(HEX)

size: $(ELF)
	@sh ./size.sh $(ELF)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: flash
flash: $(HEX)
	$(AVRDUDE) $(AD_CMD) -U flash:w:$(HEX):i

.PHONY: eeprom
eeprom:
	$(AVRDUDE) $(AD_CMD) -U eeprom:w:eeprom/fm7segm.bin:r

.PHONY: fuse
fuse:
	$(AVRDUDE) $(AD_CMD) -U lfuse:w:0x24:m -U hfuse:w:0xC1:m

# Dependencies
-include $(OBJS:.o=.d)
