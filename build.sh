#!/bin/bash
set -e

as --32 bootloader.S -o bootloader.o
ld -m elf_i386 -Ttext 0x7C00 --oformat binary bootloader.o -o bootloader.bin
truncate -s 512 bootloader.bin

gcc -m32 -ffreestanding -nostdlib -fno-pic -fno-stack-protector -mno-red-zone -mno-sse -Wno-discarded-qualifiers -c main.c -o main.o

as --32 boot.S -o boot.o

ld -m elf_i386 -T linker.ld boot.o main.o -o kernel.bin

cat bootloader.bin kernel.bin > os.img
truncate -s 1440k os.img

echo "Run with: qemu-system-i386 -fda os.img"
