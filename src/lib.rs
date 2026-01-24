#![no_std]
#![no_main]

use core::panic::PanicInfo;
use core::arch::asm;

fn print(msg: &[u8], mut pos: usize) -> usize {
    let vga = 0xb8000 as *mut u16;
    unsafe {
        for i in 0..msg.len() {
            if msg[i] == b'\n' {
                pos = pos + (80 - pos % 80);
            }
            else {
                *vga.offset(pos as isize) = 0x0F00 | (msg[i] as u16);
                pos += 1
            }
        }
    }
    return pos
}

fn clear() -> usize {
    let vga = 0xb8000 as *mut u16;
    unsafe {
        for i in 0..2000 {
            *vga.offset(i as isize) = 0x0F20;
        }
    }
    return 0
}

fn inb(port: u16) -> u8 {
    let value: u8;
    unsafe { asm!("in al, dx", out("al") value, in("dx") port); }
    return value
}

fn to_ascii(code: u8) -> u8 {
    let parsed = match code {
        0x23 => b'H',
        0x12 => b'E',
        0x18 => b'O',
        0x2E => b'C',
        0x26 => b'L',
        0x1E => b'A',
        0x13 => b'R',
        0x1C => b'!',
        _ => b'?',
    };
    return parsed
}

const PLACEHOLDER: &[u8] = b"PLACEHOLDER\n";
const DEFAULT: &[u8] = b"Hello, World!\n";
const INPUT: &[u8] = b"Input: ";
const NEWLINE: &[u8] = b"\n";
const INVALID: &[u8] = b"Invalid command\n";

#[no_mangle]
pub extern "C" fn rust() -> ! {
    let mut pos: usize = 0;
    pos = clear();
    pos = print(DEFAULT, pos);
    let mut symbols = [0u8; 32];
    let mut i: usize = 0;
    loop {
        if { inb(0x64) } & 1 != 0 {
            let input = unsafe { inb(0x60) };
            let parsed = to_ascii(input);
            if parsed != b'?' && parsed != b'!' {
                if i < symbols.len() {
                    symbols[i] = parsed;
                    i += 1;
                    pos = print(INPUT, pos);
                    for j in 0..i {
                        let msg = symbols[j];
                        pos = print(&[msg], pos);
                    }
                    let msg = [b'\n'];
                    pos = print(&msg, pos);
                }
            }
            if parsed == b'!' {
                let mut is_clear = true;
                let letters = b"CLEAR";
                for j in 0..5 {
                    if symbols[j] != letters[j] {
                        is_clear = false;
                        break;
                    }
                }
                if is_clear {
                    for j in 5..32 {
                        if symbols[j] != 0 {
                            is_clear = false;
                            break;
                        }
                    }
                }
                if is_clear && i == 5 { pos = clear(); }
                else { pos = print(INVALID, pos); }
                for j in 0..32 { symbols[j] = 0; }
                i = 0;
            }
        continue;
        }
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
