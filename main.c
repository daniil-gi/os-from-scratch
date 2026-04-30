#define CHARACTER 'c'
#define SHORT 's'
#define LONG 'l'
#define LONGLONG 'v'

#define CHARACTERBYTES 1
#define SHORTBYTES 2
#define LONGBYTES 4
#define LONGLONGBYTES 8

volatile unsigned short cursor = 0;
volatile unsigned short* vga = (unsigned short*)0xB8000;
volatile char* RAM_ADDRESS = (char*)0x100000;

void clear(void) { for (unsigned short i = 0; i < 2000; i++) { vga[i] = 0x0F20; } cursor = 0; }

char inb(unsigned short port) {
    char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void outb(unsigned short port, unsigned char value) { __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port)); }
void outw(unsigned short port, unsigned short value) { __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port)); }

void copyMemory(void* source, void* destination, unsigned char amountToCopy, unsigned char type) {
    for (unsigned char i = 0; i < amountToCopy; i++) {
        switch (type) {
            case CHARACTER: {
                char* currentElement = (char*)((char*)source + i * CHARACTERBYTES);
                char* target = (char*)((char*)destination + i * CHARACTERBYTES);
                *target = *currentElement;
                break;
            }
            case SHORT: {
                short* currentElement = (short*)((char*)source + i * SHORTBYTES);
                short* target = (short*)((char*)destination + i * SHORTBYTES);
                *target = *currentElement;
                break;
            }
            case LONG: {
                long* currentElement = (long*)((char*)source + i * LONGBYTES);
                long* target = (long*)((char*)destination + i * LONGBYTES);
                *target = *currentElement;
                break;
            }
            case LONGLONG: {
                long long* currentElement = (long long*)((char*)source + i * LONGLONGBYTES);
                long long* target = (long long*)((char*)destination + i * LONGLONGBYTES);
                *target = *currentElement;
                break;
            }
        }
    }
}

unsigned char getStrLen(char* strArg) {
    unsigned char i = 0;
    for (unsigned char j = 0; j < 255; j++) { if (*(strArg + j) != 0) { i += 1; } else { break; } }
    return i;
}

void printChar(char* character) {
    if (*character == '\n') { cursor = cursor + (80 - cursor % 80); }
    else { vga[cursor] = 0x0F00 | *(char*)character; cursor++; }
}

void printString(char* string) {
    unsigned char stringLen = getStrLen(string);
    for (unsigned char i = 0; i < stringLen; i++) { printChar(string + i); }
}

char toAscii(unsigned char code) {
    char parsed;
    switch (code) {
        case 0x1F: parsed = 'S'; break;
        case 0x16: parsed = 'U'; break;
        case 0x31: parsed = 'N'; break;
        case 0x14: parsed = 'T'; break;
        case 0x30: parsed = 'B'; break;
        case 0x11: parsed = 'W'; break;
        case 0x20: parsed = 'D'; break;
        case 0x23: parsed = 'H'; break;
        case 0x12: parsed = 'E'; break;
        case 0x18: parsed = 'O'; break;
        case 0x2E: parsed = 'C'; break;
        case 0x26: parsed = 'L'; break;
        case 0x1E: parsed = 'A'; break;
        case 0x13: parsed = 'R'; break;
        case 0x17: parsed = 'I'; break;
        case 0x19: parsed = 'P'; break;
        case 0x21: parsed = 'F'; break;
        case 0x15: parsed = 'Y'; break;
        case 0x32: parsed = 'M'; break;
        case 0x39: parsed = ' '; break;
        case 0x0E: parsed = '~'; break;
        case 0x1C: parsed = '!'; break;
        default: parsed = '?'; break;
    };
    return parsed;
}

unsigned char isEqual(char* originMsg, char* comparisonMsg) {
    unsigned char validness = 1;
    unsigned char originMsgLen = getStrLen(originMsg);
    unsigned char comparisonMsgLen = getStrLen(comparisonMsg);
    if (originMsgLen != comparisonMsgLen) { validness = 0; }
    if (validness == 1) { for (unsigned char i = 0; i < originMsgLen; i++) { if (originMsg[i] != comparisonMsg[i]) { validness = 0; } } }
    return validness;
}

volatile const char* PLACEHOLDER = "PLACEHOLDER (Something unexpected happened)\n";
volatile const char* DEFAULT = "Hello, World!\n";
volatile const char* INPUT = "Input: ";
volatile const char neverFuckingUsingCForThisFuckingKindOfFuckingShitEverFuckingAgainThisCharacterToPointerShitIsTooFuckingCrazyWhyICantJustDoItNormally = '\n';
volatile const char* NEWLINE = &neverFuckingUsingCForThisFuckingKindOfFuckingShitEverFuckingAgainThisCharacterToPointerShitIsTooFuckingCrazyWhyICantJustDoItNormally;
volatile const char* INVALID = "Invalid command\n";

volatile const char* FUNNYEASTEREGG = "CPU Triple Fault GNU x86 GNU/LINUX DEBIAN UPDATE BIOS ERASE Crash (Your Disk And RAM Are Going To Explode in THIRTY SECONDS)\n";

volatile const char* ECHOCMD = "ECHO ";
volatile const char* CLEARCMD = "CLEAR";
volatile const char* FUNNYCMD = "FUNNYMOMENT";

unsigned char isEqualEcho(char* originMsg) {
    char firstFiveSymbols[6] = {0};
    copyMemory(originMsg, &firstFiveSymbols, 5, CHARACTER);
    unsigned char validness = isEqual(firstFiveSymbols, ECHOCMD);
    return validness;
}

void halt(void) { while (1) {} }

void mainC(void) {
    clear();
    printString(DEFAULT);
    char symbols[65] = {0};
    char actualCharacters = 0;
    unsigned short ramFile = 0;
    while (1) {
        if ((inb(0x64) & 1) != 0) {
            char input = inb(0x60);
            char parsed = toAscii(input);
            if (parsed != '~' && parsed != '!' && parsed != '?' && actualCharacters != 64) {
                symbols[actualCharacters] = parsed;
                actualCharacters += 1;
                printString(INPUT);
                for (unsigned short j = 0; j < actualCharacters; j++) { printChar(&symbols[j]); }
                printChar(NEWLINE);
            }
            else if (parsed == '~') {
                if (actualCharacters != 0) {
                    symbols[actualCharacters] = 0;
                    actualCharacters -= 1;
                    printString(INPUT);
                    for (unsigned short j = 0; j < actualCharacters; j++) { printChar(&symbols[j]); }
                    printChar(NEWLINE);
                }
            }
            else if (parsed == '!') {
                if (isEqualEcho(symbols)) {
                    for (unsigned short j = 0; j < (actualCharacters - 5); j++) { printChar(&symbols[j+5]); }
                    printChar(NEWLINE);
                    for (unsigned char j = 0; j < 65; j++) { symbols[j] = 0; }
                    actualCharacters = 0;
                }
                else if (isEqual(symbols, CLEARCMD)) {
                    clear();
                    for (unsigned char j = 0; j < 65; j++) { symbols[j] = 0; }
                    actualCharacters = 0;
                }
                else if (isEqual(symbols, FUNNYCMD)) { clear(); while (1) { printString(FUNNYEASTEREGG); } }
                else {
                    printString(INVALID);
                    for (unsigned char j = 0; j < 65; j++) { symbols[j] = 0; }
                    actualCharacters = 0;
                }
            }
            else { printString(PLACEHOLDER); }
        }
    }
}
