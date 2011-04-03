# Makefile for SantaOS

all: sos.img

# disk size = dd_count * 512 (bytes)

sos.img: boot
	dd if=/dev/zero of=sos.img~ count=20480 2>/dev/null
	dd if=boot of=sos.img~ conv=notrunc 2>/dev/null
	mv sos.img~ sos.img

boot: boot.asm
	nasm boot.asm -o boot
	python sign.py boot

clean:
	rm -f sos.img sos.img~
	rm -f boot *.o
