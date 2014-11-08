TARG = fm7segm
MCU = atmega8
F_CPU = 8000000

# Source files
SRCS = $(wildcard *.c)

# Compiler options
OPTIMIZE = -Os -mcall-prologues
CFLAGS   = -g -Wall -Werror -lm $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS  = -g -Wall -Werror -mmcu=$(MCU)

# AVR toolchain and flasher
CC       = avr-gcc
OBJCOPY  = avr-objcopy
AVRDUDE  = avrdude

OBJS = $(SRCS:.c=.o)

all: $(TARG)

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJS) -lm
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex
	sh ./size.sh $@.elf

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARG).elf $(TARG).hex $(OBJS)

flash: $(TARG)
	$(AVRDUDE) -p $(MCU) -U flash:w:$(TARG).hex:i

eeprom:
	$(AVRDUDE) -p $(MCU) -U eeprom:w:$(TARG).eep:r

fuse:
	$(AVRDUDE) -p $(MCU) -U lfuse:w:0x24:m -U hfuse:w:0xc1:m

