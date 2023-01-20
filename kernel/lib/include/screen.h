#include "def.h"

#define SCREEN_START 0x000B8000
#define SCREEN_WIDTH 80
#define SCREEN_HIGHT 20
#define SCREEN_DEFAULT_COLOR 0x0F


void screenUp();

void screenNl();

void screenSpace();

void screenSetChar(char c, int pos);

void screenPrintChar(char c);

void screenPrintChars(char *c);

void screenPrintInt(int inp);

void screenPrintX8(uint8_t inp);

void screenPrintX16(uint16_t inp);

void screenPrintX32(uint32_t inp);

void screenPrintX64(uint64_t inp);

void screenClear();