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

fn outb(port: u16, value: u8) {
    unsafe {
        asm!("out dx, al", in("dx") port, in("al") value);
    }
}

fn outw(port: u16, value: u16) {
    unsafe {
        asm!("out dx, ax", in("dx") port, in("ax") value);
    }
}

fn to_ascii(code: u8) -> u8 {
    let parsed = match code {
        0x1F => b'S',
        0x16 => b'U',
        0x31 => b'N',
        0x14 => b'T',
        0x30 => b'B',
        0x11 => b'W',
        0x20 => b'D',
        0x23 => b'H',
        0x12 => b'E',
        0x18 => b'O',
        0x2E => b'C',
        0x26 => b'L',
        0x1E => b'A',
        0x13 => b'R',
        0x17 => b'I',
        0x19 => b'P',
        0x39 => b' ',
        0x0E => b'~',
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

const RAM_ADDRESS: *mut u8 = 0x1000000 as *mut u8;

#[unsafe(no_mangle)]
pub extern "C" fn rust() -> ! {
    let mut pos: usize = 0;
    pos = clear();
    pos = print(DEFAULT, pos);
    let mut symbols = [0u8; 32];
    let symbols = &mut symbols[..32];
    let mut i: usize = 0;
    let mut ram_file: usize = 0;
    loop {
        if { inb(0x64) } & 1 != 0 {
            let input = inb(0x60);
            let parsed = to_ascii(input);
            if parsed != b'?' && parsed != b'!' && parsed != b'~' {
                if i < symbols.len() {
                    unsafe { *symbols.get_unchecked_mut(i) = parsed; }
                    i += 1;
                    pos = print(INPUT, pos);
                    for j in 0..i {
                        let msg = symbols[j];
                        pos = print(&[msg], pos);
                    }
                    pos = print(NEWLINE, pos);
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
                let mut is_echo_msg = true;
                let letters = b"ECHO ";
                for j in 0..5 {
                    if symbols[j] != letters[j] {
                        is_echo_msg = false;
                        break;
                    }
                }
                let mut is_echo_empty = true;
                let letters = b"ECHO";
                for j in 0..4 {
                    if symbols[j] != letters[j] {
                        is_echo_empty = false;
                        break;
                    }
                }
                let mut is_reboot = true;
                let letters = b"REBOOT";
                for j in 0..6 {
                    if symbols[j] != letters[j] {
                        is_reboot = false;
                        break;
                    }
                }
                let mut is_shutdown = true;
                let letters = b"SHUTDOWN";
                for j in 0..8 {
                    if symbols[j] != letters[j] {
                        is_shutdown = false;
                        break;
                    }
                }
                let mut is_write = true;
                let letters = b"WRITE ";
                for j in 0..6 {
                    if symbols[j] != letters[j] {
                        is_write = false;
                        break;
                    }
                }
                let mut is_read = true;
                let letters = b"READ";
                for j in 0..4 {
                    if symbols[j] != letters[j] {
                        is_read = false;
                        break;
                    }
                }
                let mut is_append = true;
                let letters = b"APPEND ";
                for j in 0..7 {
                    if symbols[j] != letters[j] {
                        is_append = false;
                        break;
                    }
                }
                if is_clear {
                    if i != 5 { is_clear = false; }
                }
                else if is_echo_msg {
                    if i < 5 { is_echo_msg = false; }
                }
                else if is_echo_empty {
                    if i != 4 { is_echo_empty = false; }
                }
                else if is_reboot {
                    if i != 6 { is_reboot = false; }
                }
                else if is_shutdown {
                    if i != 8 { is_shutdown = false; }
                }
                else if is_write {
                    if i <= 6 { is_write = false; }
                }
                else if is_read {
                    if i != 4 { is_read = false; }
                }
                else if is_append {
                    if i <= 8 { is_append = false; }
                }
                if is_clear { pos = clear(); }
                else if is_echo_msg {
                    pos = print(&symbols[5..i], pos);
                    pos = print(NEWLINE, pos);
                }
                else if is_echo_empty {
                    pos = print(DEFAULT, pos);
                }
                else if is_reboot {
                    outb(0x64, 0xFE);
                    loop {}
                }
                else if is_shutdown {
                    outw(0x604, 0x2000);
                    loop {}
                }
                else if is_write {
                    let content = &symbols[6..i];
                    ram_file = content.len();
                    unsafe {
                        for j in 0..ram_file {
                            *RAM_ADDRESS.offset(j as isize) = content[j];
                        }
                    }
                    pos = print(b"WROTE TO RAM\n", pos);
                }
                else if is_read {
                    if ram_file == 0 {
                        pos = print(b"EMPTY\n", pos);
                    }
                    else {
                        unsafe {
                            let data = core::slice::from_raw_parts(RAM_ADDRESS, ram_file);
                            pos = print(data, pos);
                            pos = print(NEWLINE, pos);
                        }
                    }
                }
                else if is_append {
                    if ram_file == 0 {
                        pos = print(b"EMPTY\n", pos);
                    }
                    else {
                        let content = &symbols[8..i];
                        let old_len = ram_file;
                        ram_file += content.len();
                        unsafe {
                            for j in 0..content.len() {
                                *RAM_ADDRESS.offset((old_len + j) as isize) = content[j];
                            }
                        }
                        pos = print(b"APPENDED TO RAM\n", pos);
                    }
                }
                else { pos = print(INVALID, pos); }
                for j in 0..32 { symbols[j] = 0; }
                i = 0;
            }
            if parsed == b'~' && i > 0 && i < symbols.len() {
                i -= 1;
                unsafe { *symbols.get_unchecked_mut(i) = 0; }
                pos = print(INPUT, pos);
                for j in 0..i {
                    let msg = symbols[j];
                    pos = print(&[msg], pos);
                }
                pos = print(NEWLINE, pos);
            }
            continue;
        }
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[unsafe(no_mangle)]
pub extern "C" fn PanicHandler() -> ! {
    loop {}
}
