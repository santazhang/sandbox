# Makefile for SantaOS

OBJDIR=obj

all: $(OBJDIR)/sos.img

# disk size = dd_count * 512 (bytes)
$(OBJDIR)/sos.img: $(OBJDIR)/boot
	mkdir -p $(OBJDIR)
	dd if=/dev/zero of=$(OBJDIR)/sos.img~ count=20480 2>/dev/null
	dd if=$(OBJDIR)/boot of=$(OBJDIR)/sos.img~ conv=notrunc 2>/dev/null
	mv $(OBJDIR)/sos.img~ $(OBJDIR)/sos.img

$(OBJDIR)/boot: boot.asm
	mkdir -p $(OBJDIR)
	nasm boot.asm -o $(OBJDIR)/boot
	python sign.py $(OBJDIR)/boot

clean:
	rm -rf $(OBJDIR)

