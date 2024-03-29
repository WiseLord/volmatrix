# Output file name
TARG     = volmatrix

# MCU name and frequency
MCU      = atmega8
F_CPU    = 8000000

# Source files
SRCS     = $(wildcard *.c) $(wildcard audio/*.c)

# Build directory
BUILDDIR = build

# Compiler options
OPTIMIZE = -Os -mcall-prologues -fshort-enums -ffunction-sections -fdata-sections
DEBUG    = -g -Wall -Werror
DEPS     = -MMD -MP -MT $(BUILDDIR)/$(*F).o -MF $(BUILDDIR)/$(*D)/$(*F).d
CFLAGS   = $(DEBUG) -lm $(OPTIMIZE) $(DEPS) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS  = $(DEBUG) -mmcu=$(MCU) -Wl,-gc-sections -mrelax

# Main definitions
DEFINES  += -D_NO_MUTE_PORT
# Supported audioprocessors
DEFINES += -D_TDA7439 -D_TDA731X -D_TDA7448 -D_PT232X -D_TEA63X0

# AVR toolchain and flasher
PREFIX = avr-
CC = $(PREFIX)gcc
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
SZ = $(PREFIX)size

# AVRDude parameters
AVRDUDE  = avrdude
AD_MCU   = -p $(MCU)
#AD_PROG = -c stk500v2
#AD_PORT = -P avrdoper

AD_CMD   = $(AD_MCU) $(AD_PROG) $(AD_PORT) -V

# Build objects
OBJS     = $(addprefix $(BUILDDIR)/, $(SRCS:.c=.o))
ELF      = $(BUILDDIR)/$(TARG).elf
HEX = flash/$(TARG).hex

all: $(HEX) size

$(HEX): $(ELF)
	$(CP) -O ihex -R .eeprom -R .nwram $(ELF) $(HEX)

$(ELF): $(OBJS)
	@mkdir -p $(addprefix $(BUILDDIR)/, $(SUBDIRS)) flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJS)
	$(OD) -h -S $(ELF) > $(BUILDDIR)/$(TARG).lss

size: $(ELF)
	$(SZ) $(ELF)

$(BUILDDIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR)

.PHONY: flash
flash: $(ELF)
	$(AVRDUDE) $(AD_CMD) -U flash:w:flash/$(TARG).hex:i

.PHONY: eeprom
eeprom:
	$(AVRDUDE) $(AD_CMD) -U eeprom:w:../eeprom/$(TARG).bin:r

fuse:
	$(AVRDUDE) $(AD_CMD) -U lfuse:w:0x24:m -U hfuse:w:0xD1:m

# Dependencies
-include $(OBJS:.o=.d)
