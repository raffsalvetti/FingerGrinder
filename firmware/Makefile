# -------------- start of configurtion --------------

PROJ_NAME = firmware
DEVICE = attiny85
CLOCK = 16500000L
PROGRAMMER = usbasp

# https://www.engbedded.com/fusecalc/
FUSE_LOW = 0x62
FUSE_HIGH = 0xdd
FUSE_EXTENDED = 0xff


INCLUDES = -I/usr/lib/avr/include -I./src -I./src/usbdrv
LFLAGS =
LIBS =
CFLAGS = -std=c11 -Wl,-Map,$(PROJ_NAME).map -mmcu=$(DEVICE) -DF_CPU=$(CLOCK) $(INCLUDES)
CPPFLAGS = 

# -------------- end of configuration --------------

AVRDUDE = avrdude
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
CC = avr-gcc

H_SOURCE = $(wildcard ./src/*.h)
H_SOURCE += $(wildcard ./src/usbdrv/*.h)
C_SOURCE = $(wildcard ./src/*.c)
C_SOURCE += $(wildcard ./src/usbdrv/*.c)
S_SOURCE = $(wildcard ./src/usbdrv/*.S)
CPP_SOURCE = $(wildcard ./src/*.cpp)

OBJ = $(C_SOURCE:.c=.o) $(S_SOURCE:.cpp=.o) $(CPP_SOURCE:.cpp=.o) 

RM = rm -rf

all: objFolder hex eep size

$(PROJ_NAME).elf: $(OBJ)
	@ echo 'Linking: $@'
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LFLAGS) $(LIBS) $^ -o $@
	@ echo 'Finished linking: $@'
	@ echo ' '

./obj/%.o: ./src/%.c ./src/%.cpp ./src/%.h
	@ echo 'Building objects: $<'
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
	@ echo ' '

./obj/main.o: ./src/main.c $(H_SOURCE)
	@ echo 'Building main: $<'
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
	@ echo ' '

hex: $(PROJ_NAME).elf
	$(OBJCOPY) -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex

eep: $(PROJ_NAME).elf
	$(OBJCOPY) -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex $(PROJ_NAME).elf $(PROJ_NAME).eep

size: $(PROJ_NAME).elf
	$(AVRSIZE) --format=avr --mcu=$(DEVICE) $(PROJ_NAME).elf

disasm: $(PROJ_NAME).elf
	$(OBJDUMP) -d $(PROJ_NAME).elf

objFolder:
	@ mkdir -p obj

clean:
	@ $(RM) ./obj/*.o ./src/*.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).eep $(PROJ_NAME).map 

test:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(DEVICE) -v

flash: all
	$(AVRDUDE) -c $(PROGRAMMER) -p $(DEVICE) -U flash:w:$(PROJ_NAME).hex:i

dump:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(DEVICE) -U flash:r:dump_$(shell date +'%y%m%d%H%M%S').hex:i

fuse:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(DEVICE) -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m -U efuse:w:$(FUSE_EXTENDED):m	

.PHONY: all clean

# DO NOT DELETE THIS LINE -- make depend needs it