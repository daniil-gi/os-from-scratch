#!/bin/bash
set -e

as --32 bootloader.gas -o bootloader.o
ld -m elf_i386 -Ttext 0x7C00 --oformat binary bootloader.o -o bootloader.bin
truncate -s 512 bootloader.bin

rustc \
    --target i686-unknown-linux-gnu \
    --emit obj \
    -A warnings \
    -C panic=abort \
    -C opt-level=z \
    -C relocation-model=static \
    -C target-cpu=i386 \
    -C target-feature=-mmx,-sse,-sse2 \
    src/lib.rs -o rust.o

PANIC_SYMS=$(nm -u rust.o | grep -E "panic|fail" | awk '{print $2}')

DEFSYM_FLAGS=""
for SYM in $PANIC_SYMS; do
    DEFSYM_FLAGS="$DEFSYM_FLAGS --defsym=$SYM=PanicHandler"
done

as --32 boot.gas -o boot.o
ld -m elf_i386 -T kernel.ld boot.o rust.o -o kernel.bin $DEFSYM_FLAGS

cat bootloader.bin kernel.bin > os.img
truncate -s 1440k os.img
