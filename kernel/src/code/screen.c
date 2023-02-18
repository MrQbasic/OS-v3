#include "screen.h"
#include <stdarg.h>

int cur_X = 0;
int cur_Y = 0;

void screenUp(){
    char *x = (char *) SCREEN_START;
    char *y = (char *) SCREEN_START + (SCREEN_WIDTH * 2);
    for(int i=0; i<SCREEN_HIGHT * (SCREEN_WIDTH * 2); i++){
        *x = *y;
        x++;
        y++;
    }
}

void screenNl(){
    cur_X = 0;
    cur_Y++;
    if(cur_Y > SCREEN_HIGHT){
        cur_Y--;
        screenUp();
    }
}

void screenSpace(){
    screenPrintChar(' ');
}

void screenSetChar(char c, int pos){
    char *x = (char *) SCREEN_START;
    x += pos*2;
    *x = c;
    x++;
    *x = SCREEN_DEFAULT_COLOR;
}

void screenPrintChar(char c){
    screenSetChar(c, cur_Y * SCREEN_WIDTH + cur_X);
    cur_X++;
    if(cur_X > SCREEN_WIDTH){
        cur_X = 0; 
        cur_Y++;
    }
    if(cur_Y > SCREEN_HIGHT){
        cur_Y--;
        screenUp();
    }
}

void screenPrintChars(char *c){
    int i = 0;
    while(1){
        if(c[i] == '/'){
            i++;
            switch(c[i]){
                case 'e':
                    return;
                case 'n':
                    screenNl();
                    i++;
                    continue; 
            }
        }else{
            screenPrintChar(c[i]);
        }
        i++;
    }
}

char hex_dig []   = "0123456789ABCDEF"; 
char hex_prefix[] = "0x/e";

void screenPrintInt(int inp){
    screenPrintChars(hex_prefix);
    for(int i=3; i>=0; i--){
        int x = (inp & (0xFF << (i*8))) >> (i*8);
        screenPrintChar(hex_dig[(x & 0xF0) >> 4]);
        screenPrintChar(hex_dig[x & 0x0F]);
    }
}

void screenPrintX8(uint8_t inp){
    screenPrintChars(hex_prefix);
    screenPrintChar(hex_dig[(inp & 0xF0) >> 4]);
    screenPrintChar(hex_dig[inp & 0x0F]);
}

void screenPrintX16(uint16_t inp){
    screenPrintChars(hex_prefix);
    for(int i=1; i>=0; i--){
        int x = (inp & (0xFF << (i*8))) >> (i*8);
        screenPrintChar(hex_dig[(x & 0xF0) >> 4]);
        screenPrintChar(hex_dig[x & 0x0F]);
    }
}

void screenPrintX32(uint32_t inp){
    screenPrintChars(hex_prefix);
    for(int i=3; i>=0; i--){
        int x = (inp & (0xFF << (i*8))) >> (i*8);
        screenPrintChar(hex_dig[(x & 0xF0) >> 4]);
        screenPrintChar(hex_dig[x & 0x0F]);
    }
}

void screenPrintX64(uint64_t inp){
    screenPrintChars(hex_prefix);
    uint32_t inp1 = (uint32_t) (inp >> 32);
    uint32_t inp2 = (uint32_t) inp; 
    for(int i=3; i>=0; i--){
        int x = (inp1 & (0xFF << (i*8))) >> (i*8);
        screenPrintChar(hex_dig[(x & 0xF0) >> 4]);
        screenPrintChar(hex_dig[x & 0x0F]);
    }
    for(int i=3; i>=0; i--){
        int x = (inp2 & (0xFF << (i*8))) >> (i*8);
        screenPrintChar(hex_dig[(x & 0xF0) >> 4]);
        screenPrintChar(hex_dig[x & 0x0F]);
    }
}

void screenClear(){
    for(int i=0; i<SCREEN_HIGHT; i++){
        for(int j=0; j<SCREEN_WIDTH; j++){
            screenSetChar(' ', i * SCREEN_WIDTH + j);
        }
    }
    cur_X = 0;
    cur_Y = 0;
}

void screenPrint(const char* fmt, ...){
    va_list args;
    va_start(args,fmt);
    while(1){
        if(*fmt == '/'){
            fmt++;
            if(*fmt == 'n'){
                screenNl();
            }else if(*fmt == "0"){
                screenClear();
            }else if(*fmt == 'e'){
                va_end(args);
                return;
            }else if(*fmt == 'x'){

                fmt++;
                switch(*fmt){
                case 'B':
                    screenPrintX8((uint8_t) va_arg(args, uint32_t)); break;
                case 'W':
                    screenPrintX16((uint16_t) va_arg(args, uint32_t)); break;
                case 'D':
                    screenPrintX32(va_arg(args, uint32_t)); break;
                case 'Q': 
                    screenPrintX64(va_arg(args, uint64_t)); break;
                }
            }else if(*fmt == 'c'){
                screenPrintChars(va_arg(args, char*));
            }
        }else{
            screenPrintChar(*fmt);
        }
        fmt++;
    }
}