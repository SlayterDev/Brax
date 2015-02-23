SOURCES := kernel/boot.o kernel/kmain.o kernel/led.o kernel/interrupt.o

INC=-include

CC=arm-none-eabi-gcc

CFLAGS=-march=armv6zk -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -Iinclude -O2 -Wall -Wextra
ASFLAGS=-march=armv6zk -mcpu=arm1176jzf-s -fpic -ffreestanding
LDFLAGS=-ffreestanding -O2 -nostdlib

all: $(SOURCES) link
	arm-none-eabi-objcopy kernel.elf -O binary kernel.img

clean:
	-rm */*.o kernel.img kernel.elf

link:
	$(CC) -T linker.ld -o kernel.elf $(LDFLAGS) $(SOURCES)

.s.o:
	$(CC) $(ASFLAGS) $<