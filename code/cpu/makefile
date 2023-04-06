# makefile
# adapted from: https://wiki.archlinux.org/title/AVR
# other source: https://www.avrfreaks.net/forum/compile-script-avr-gcc-linux
# program by:
# avrdude -p x128a1 -c avrispmkII -B 1MHz -U flash:w:$(TARGET).hex

# --
# variables for make

# paths
BINDIR = ./bin
OBJDIR = ./obj
TARGET = $(BINDIR)/control_code
EXTRAINCDIRS = /usr/avr/include
SUBMISSION = ./archiv/control_code_S1.3.1.zip

# Create object files directory, 2>/dev/null -> error supression
$(shell mkdir $(OBJDIR) 2>/dev/null)
$(shell mkdir $(BINDIR) 2>/dev/null)

# tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f

# MCU name
MCU = atxmega128a1

# Processor frequency.
F_CPU = 16000000

# fuses?
LFUSE = 0x9f
HFUSE = 0xd1

# format
FORMAT = ihex

# optimization level, s: optimized for size
OPTLEVEL = s

# compiling flags
CFLAGS = -DF_CPU=$(F_CPU)UL
CFLAGS += -O$(OPTLEVEL)
CFLAGS += -mmcu=$(MCU)
CFLAGS += -std=gnu99
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:%.c=$(OBJDIR)/%.lst)
CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))
CFLAGS += --param=min-pagesize=0

# load flags
LDFLAGS = -Wl,--gc-sections
LDFLAGS += -Wl,--print-gc-sections

# avrdude
AVRDUDE_MCU = x128a1
AVRDUDE_PROGRAMMER = avrispmkII
AVRDUDE_SPEED = 1MHz

# avrdude flags
AVRDUDE_FLAGS = -p $(AVRDUDE_MCU)
AVRDUDE_FLAGS += -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += -B $(AVRDUDE_SPEED)

# messages
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_FLASH = Preparing HEX file:

# source files
SRC = ${wildcard *.c}

# object files
OBJ = $(SRC:%.c=$(OBJDIR)/%.o) 

# define listing files.
LST = $(SRC:%.c=$(OBJDIR)/%.lst) 


# --
# special stuff

# precious -> not deleted after interruption of make
.PRECIOUS: $(OBJ)

# secondary -> intermediate files, but are not deleted
.SECONDARY: $(TARGET).elf


# --
# make rules

# all
all: gccversion build size

# build
build: elf hex

# builds
elf: $(TARGET).elf
hex: $(TARGET).hex

# create hex
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -j .text -j .data $< $@

# create elf
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) -mmcu=$(MCU) $(LDFLAGS) $^ -o $@

# create objects
$(OBJDIR)/%.o: %.c
	@echo
	@echo $(MSG_COMPILING) $< to $@
	$(CC) $(CFLAGS) -c $< -o $@

# version
gccversion: 
	@$(CC) --version

# size
size: $(TARGET).elf
	@echo
	$(SIZE) -C --mcu=$(AVRDUDE_MCU) $(TARGET).elf

# analyze
analyze: $(TARGET).elf
	$(NM) -S --size-sort -t decimal $(TARGET).elf

# program
isp: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET).hex

# fuse
fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

# fuse and program
release: fuses isp

# submission
submission: 
	zip $(SUBMISSION) *.hex

# clean
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).elf $(OBJ) $(LST) *~