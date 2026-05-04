// constant shortcuts

#define CHARACTER 'c'
#define SHORT 's'
#define LONG 'l'
#define LONGLONG 'v'

#define CHARACTERBYTES 1
#define SHORTBYTES 2
#define LONGBYTES 4
#define LONGLONGBYTES 8

// functional constants

volatile unsigned short cursor = 0;
volatile unsigned short* vga = (unsigned short*)0xB8000;
volatile char* MALLOC_ADDRESS = (char*)0x100000;

// general/base functions

void halt(void) { while (1) {} }

void clear(void) { for (unsigned short i = 0; i < 2000; i++) { vga[i] = 0x0F20; } cursor = 0; }

char inb(unsigned short port) {
    char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void outb(unsigned short port, unsigned char value) { __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port)); }
void outw(unsigned short port, unsigned short value) { __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port)); }

void* allocateMemory(unsigned long offset, unsigned short value) { char* ptr = (char*)(MALLOC_ADDRESS + offset); *ptr = value; return ptr; }

void copyMemory(void* source, void* destination, unsigned long amountToCopy, unsigned char type) {
    for (unsigned long i = 0; i < amountToCopy; i++) {
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
            default:
                long long* target = (long long*)((char*)destination + i * LONGLONGBYTES);
                *target = 0;
                break;
        }
    }
}

void setMemory(void* value, void* destination, unsigned long amountToSet, char type) {
    for (unsigned long i = 0; i < amountToSet; i++) {
        switch (type) {
            case CHARACTER:
                *(char*)((char*)destination + i * CHARACTERBYTES) = *(char*)(value);
                break;
            case SHORT:
                *(short*)((char*)destination + i * SHORTBYTES) = *(short*)(value);
                break;
            case LONG:
                *(long*)((char*)destination + i * LONGBYTES) = *(long*)(value);
                break;
            case LONGLONG:
                *(long long*)((char*)destination + i * LONGLONGBYTES) = *(long long*)(value);
                break;
            default:
                *(char*)(destination + i) = 0;
                break; 
        }
    }
}

unsigned char getStrLen(char* strArg) {
    unsigned char i = 0;
    for (unsigned long j = 0; j < 4294967295; j++) { if (*(strArg + j) != 0) { i += 1; } else { break; } }
    return i;
}

void reverseArray(char* strArg, unsigned short length) {
    unsigned short currentElement = 0;
    unsigned short lastElement = length - 1;
    while (currentElement < lastElement) {
        char tmp = strArg[currentElement];
        strArg[currentElement] = strArg[lastElement];
        strArg[lastElement] = tmp;
        currentElement += 1;
        lastElement -= 1;
    }
}

char* itoa(unsigned char intArg) {
    static char completedString[255] = {0};
    unsigned char counter = 0;
    for (unsigned char j = 0; j < 255; j++) {
        if (intArg / 10 == 0) { break; }
        completedString[j] = intArg % 10 + 48;
        intArg /= 10;
        counter += 1;
    }
    reverseArray(completedString, counter);
    return completedString;
}

void printChar(char* character) {
    if (cursor == 2000) {
        for (unsigned short i = 1920; i < 2000; i++) { vga[i] = 0x0F00; }
        cursor = 1920;
    }
    if (*character == '\n') { cursor = cursor + (80 - cursor % 80); }
    else { vga[cursor] = 0x0F00 | *character; cursor++; }
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
        case 0x2B: parsed = '\\'; break;
        case 0x0B: parsed = '0'; break;
        case 0x02: parsed = '1'; break;
        case 0x03: parsed = '2'; break;
        case 0x04: parsed = '3'; break;
        case 0x05: parsed = '4'; break;
        case 0x06: parsed = '5'; break;
        case 0x07: parsed = '6'; break;
        case 0x08: parsed = '7'; break;
        case 0x09: parsed = '8'; break;
        case 0x0A: parsed = '9'; break;
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

char getFileDescriptorFromInput(char originChar) {
    char fileDescriptor;
    switch (originChar) {
        case '0': fileDescriptor = 0; break;
        case '1': fileDescriptor = 1; break;
        case '2': fileDescriptor = 2; break;
        case '3': fileDescriptor = 3; break;
        case '4': fileDescriptor = 4; break;
        case '5': fileDescriptor = 5; break;
        case '6': fileDescriptor = 6; break;
        case '7': fileDescriptor = 7; break;
        case '8': fileDescriptor = 8; break;
        case '9': fileDescriptor = 9; break;
        default: fileDescriptor = -1; break;
    }
    return fileDescriptor;
}

// system messages

volatile const char* PLACEHOLDER = "PLACEHOLDER (Something unexpected happened)\n";
volatile const char* DEFAULT = "Hello, World!\n";
volatile const char* INPUT = "Input: ";
volatile const char neverFuckingUsingCForThisFuckingKindOfFuckingShitEverFuckingAgainThisCharacterToPointerShitIsTooFuckingCrazyWhyICantJustDoItNormally = '\n';
volatile const char* NEWLINE = &neverFuckingUsingCForThisFuckingKindOfFuckingShitEverFuckingAgainThisCharacterToPointerShitIsTooFuckingCrazyWhyICantJustDoItNormally;
volatile const char* INVALID = "Invalid command\n";

// misc messages

volatile const char* FUNNYEASTEREGG = "CPU Triple Fault GNU x86 GNU/LINUX DEBIAN UPDATE BIOS ERASE Crash (Your Disk And RAM Are Going To Explode in THIRTY SECONDS)\n";
volatile const char* KERNELPANIC = "KERNEL PANIC ";

// commands without arguments

volatile const char* CLEARCMD = "CLEAR";
volatile const char* FUNNYCMD = "FUNNY MOMENT";
volatile const char* SUICIDECMD = "SUICIDE";
volatile const char* HALTCMD = "HALT";

// commands with arguments

volatile const char* ECHOCMD = "ECHO ";
volatile const char* WRITECMD = "WRITE ";
volatile const char* READCMD = "READ ";
volatile const char* ERASECMD = "ERASE ";

// less-important functions / descendants from base functions

void suicide(void) {
    while (1) {
        char* ptr = 0;
        cursor = 0;
        for (unsigned char i = 0; i < 255; i++) { printString(KERNELPANIC); }
        while (1) { for (unsigned long i = 0; i < 0x100000; i++) { *(ptr + i) = 0; } }
        while (1) { halt(); }
    }
}

unsigned char isEqualEcho(char* originMsg) {
    char firstFiveSymbols[6] = {0};
    copyMemory(originMsg, &firstFiveSymbols, 5, CHARACTER);
    unsigned char validness = isEqual(firstFiveSymbols, ECHOCMD);
    return validness;
}

unsigned char isEqualWrite(char* originMsg) {
    char firstSixSymbols[7] = {0};
    copyMemory(originMsg, &firstSixSymbols, 6, CHARACTER);
    unsigned char validness = isEqual(firstSixSymbols, WRITECMD);
    return validness;
}

unsigned char isEqualRead(char* originMsg) {
    char firstFiveSymbols[6] = {0};
    copyMemory(originMsg, &firstFiveSymbols, 5, CHARACTER);
    unsigned char validness = isEqual(firstFiveSymbols, READCMD);
    return validness;
}

unsigned char isEqualErase(char* originMsg) {
    char firstSixSymbols[7] = {0};
    copyMemory(originMsg, &firstSixSymbols, 6, CHARACTER);
    unsigned char validness = isEqual(firstSixSymbols, ERASECMD);
    return validness;
}

// main executable code

void mainC(void) {
    clear();
    printString(DEFAULT);
    char symbols[65] = {0};
    char files[10][1024];
    char* tmpPtr = allocateMemory(0, 0);
    setMemory(tmpPtr, files, 1024*10, CHARACTER);
    char actualCharacters = 0;
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
                else if (isEqualWrite(symbols) && actualCharacters >= 7) {
                    char fileDescriptor = getFileDescriptorFromInput(symbols[6]);
                    if (fileDescriptor != -1 && symbols[7] == ' ') { 
                        for (unsigned char j = 0; j < (actualCharacters - 8); j++) { files[fileDescriptor][j] = symbols[j+8]; }
                    }
                    else { printString(INVALID); }
                    for (unsigned char j = 0; j < 65; j++) { symbols[j] = 0; }
                    actualCharacters = 0;
                }
                else if (isEqualRead(symbols) && actualCharacters >= 6) {
                    char fileDescriptor = getFileDescriptorFromInput(symbols[5]);
                    if (fileDescriptor != -1) { printString(files[fileDescriptor]); }
                    else { printString(INVALID); }
                    for (unsigned char j = 0; j < 65; j++) { symbols[j] = 0; }
                    actualCharacters = 0;
                    printChar(NEWLINE);
                }
                else if (isEqualErase(symbols) && actualCharacters >= 7) {
                    char fileDescriptor = getFileDescriptorFromInput(symbols[6]);
                    if (fileDescriptor != -1) { for (unsigned short j = 0; j < 1024; j++) { files[fileDescriptor][j] = 0; } }
                    else { printString(INVALID); }
                    for (unsigned char j = 0; j < 65; j++) { symbols[j] = 0; }
                    actualCharacters = 0;
                }
                else if (isEqual(symbols, SUICIDECMD)) { suicide(); }
                else if (isEqual(symbols, HALTCMD)) { halt(); }
                else {
                    printString(INVALID);
                    for (unsigned char j = 0; j < 65; j++) { symbols[j] = 0; }
                    actualCharacters = 0;
                }
            }
            else if (parsed == '?') {}
            else { printString(PLACEHOLDER); }
        }
    }
}
