all: boot2 boot1

install: 
	bximage -mode=create -fd=1.44M  -q a.img
	/sbin/mkdosfs a.img
	dd if=boot1 of=a.img bs=1 count=512 conv=notrunc
	mcopy -o boot2 a:BOOT2

boot1: boot1.asm boot2.exe
	nasm -l boot1.list -DENTRY=`./getaddr.sh main` boot1.asm

boot2: boot2.exe
	objcopy -j .text* -j .data* -j .rodata* -S -O binary boot2.exe boot2

boot2.exe: boot2_S.o boot2_c.o
	ld -g -melf_i386 -Ttext 0x10000 -e main -o boot2.exe boot2_S.o boot2_c.o

boot2_S.o: boot2.S
	gcc -g -m32 -c -masm=intel -o boot2_S.o boot2.S

boot2_c.o: boot2.c
	gcc -g -m32 -c -o boot2_c.o boot2.c

run:
	qemu-system-i386 -boot a -fda a.img

debug:
	qemu-system-i386 -S -s -boot a -fda a.img

clean:
	rm -f *.o
	rm -f a.img
	rm -f *.exe
	rm -f *.list
	rm boot1
	rm boot2
	echo Clean done