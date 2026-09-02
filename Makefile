CFLAGS ?= -mcpu=cortex-m3 -mthumb -Wall -Wextra -Wshadow -Werror -Wundef -Wfloat-equal
LDFLAGS ?= -T link.ld -nostdlib 
LIBS ?= -lc -lm -lgcc

#build target
build: firmware.bin


#convert elf to bin
firmware.bin: firmware.elf
	arm-none-eabi-objcopy -O binary $< $@

#link the object files to create elf file
firmware.elf: main.o
	arm-none-eabi-gcc $(LDFLAGS) $^ $(LIBS) -o $@

#compile C source files to object files
main.o: main.c
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

# flash the firmware to the microcontroller
stmflash: firmware.bin
	st-flash write firmware.bin 0x8000000


#clean the build artifacts
clean:
	rm -f firmware.* *.o