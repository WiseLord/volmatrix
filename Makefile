TARG = volmatrix

MCU = atmega8
F_CPU = 8000000

# Source files
SRCS = $(wildcard *.c)

# Compiler options
OPTIMIZE = -Os -mcall-prologues -fshort-enums -ffunction-sections -fdata-sections
DEBUG = -g -Wall -Werror
CFLAGS = $(DEBUG) -lm $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS = $(DEBUG) -mmcu=$(MCU) -Wl,-gc-sections

# AVR toolchain and flasher
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

$(TARG): dirs $(OBJS)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJS) -lm
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex
	sh ./size.sh $@.elf

dirs:
	mkdir -p $(OBJDIR)

obj/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)

flash: $(TARG)
	$(AVRDUDE) -p $(MCU) -U flash:w:$(TARG).hex:i

eeprom:
	$(AVRDUDE) -p $(MCU) -U eeprom:w:$(TARG).bin:r

fuse:
	$(AVRDUDE) -p $(MCU) -U lfuse:w:0x24:m -U hfuse:w:0xD1:m

