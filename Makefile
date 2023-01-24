OBJS=src/entry.o src/sys/x86_64/mem.o src/sys/x86_64/irq.o src/sys/x86_64/gdt.o src/sys/x86_64/idt.o 
TESTOBJS=src/sys/x86_64/mem.o src/sys/x86_64/irq.o src/sys/x86_64/gdt.o src/sys/x86_64/idt.o 
TESTS=
CC=cc
LD=ld
PREFIX=/usr/local
LDFLAGS=-Tsrc/linker.ld  -z max-page-size=0x1000 -static
LDLIBS=
CFLAGS=-Wall -Wextra -Wno-unused -Wno-unused-parameter -ffreestanding -fno-stack-protector -fno-pic -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel

all: $(OBJS) $(TESTS) orion-x86_64.x

clean:
	rm -rf $(OBJS)
	rm -rf $(TESTS)
	rm -rf vgcore.*
	rm -rf core*
	rm -rf orion-x86_64.x

install:
	mkdir -p $(PREFIX)
	install -m 755 orion-x86_64.x $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/orion-x86_64.x

src/entry.o: src/entry.c src/common/ultra_protocol.h
	$(CC) -c $(CFLAGS) src/entry.c -o src/entry.o

src/sys/x86_64/utils.o: src/sys/x86_64/utils.c
	$(CC) -c $(CFLAGS) src/sys/x86_64/utils.c -o src/sys/x86_64/utils.o

src/sys/x86_64/mem.o: src/sys/x86_64/mem.c
	$(CC) -c $(CFLAGS) src/sys/x86_64/mem.c -o src/sys/x86_64/mem.o

src/sys/x86_64/irq.o: src/sys/x86_64/irq.c
	$(CC) -c $(CFLAGS) src/sys/x86_64/irq.c -o src/sys/x86_64/irq.o

src/sys/x86_64/gdt.o: src/sys/x86_64/gdt.c
	$(CC) -c $(CFLAGS) src/sys/x86_64/gdt.c -o src/sys/x86_64/gdt.o

src/sys/x86_64/idt.o: src/sys/x86_64/idt.c
	$(CC) -c $(CFLAGS) src/sys/x86_64/idt.c -o src/sys/x86_64/idt.o

orion-x86_64.x: $(OBJS)
	$(LD) $(OBJS) -o orion-x86_64.x $(LDFLAGS) $(LDLIBS)
