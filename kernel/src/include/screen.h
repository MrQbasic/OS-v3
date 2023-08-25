#include "def.h"

#define SCREEN_START 0x000B8000
#define SCREEN_WIDTH 80
#define SCREEN_HIGHT 19
#define SCREEN_DEFAULT_COLOR 0x0F


void screenUp();

void screenNl();

void screenSpace();

void screenSetChar(char c, int pos);

void screenPrintChar(char c);

void screenPrintChars(char *c);

void screenPrintX(uint64_t inp, uint8_t bits);

void screenPrintB(uint64_t inp, uint8_t bits);

void screenClear();

void screenCursorMove(int x, int y);

void screenCursorSet(int x, int y);

void screenCursorGet(int* x, int*y);

//commands:
//  /e  ->  end of string
//  /n  ->  new line
//  /0  ->  clear screen
//  /xB ->  print hex byte  (8-bit)
//  /xW ->  print hex word  (16-bit)
//  /xD ->  print hex dword (32-bit)
//  /xQ ->  print hex qword (64-bit)
//  /bB ->  print bin byte  (8-bit)
//  /bW ->  print bin word  (16-bit)
//  /bD ->  print bin dword (32-bit)
//  /bQ ->  print bin qword (64-bit)
//  /c  ->  print char[] (ends with/e)
void screenPrint(const char* fmt,...);

#define SCREEN_MEMORY_DUMP_BYTES_PER_ROW 8
void screenMemoryDump(uint64_t start, uint64_t size);

// 0 -> Normal
// 1 -> printDisables (only function screenPrint)
void screenSetPrintStatus(uint8_t status);