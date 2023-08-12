#include "screen.h"
#include <stdarg.h>

int cur_X = 0;
int cur_Y = 0;

void screenCursorMove(int x, int y){
    cur_X += x;
    cur_Y += y;
    if(cur_X < 0) cur_X = 0;
    if(cur_Y < 0) cur_Y = 0;

}

void screenUp(){
    char *x = (char *) SCREEN_START;
    char *y = (char *) SCREEN_START + (SCREEN_WIDTH * 2);
    __asm__("cli");
    for(int i=0; i < (SCREEN_HIGHT) * (SCREEN_WIDTH * 2); i++){
        *x = *y;
        x++;
        y++;
    }
    for(int i=0; i < (SCREEN_WIDTH * 2); i++){
        *x = 0;
        x++;
    }
    __asm__("sti");
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

char hex_digits[] = "0123456789ABCDEF"; 
char hex_prefix[] = "0x/e";

void screenPrintX(uint64_t inp, uint8_t bits){
    screenPrintChars(hex_prefix);
    for(int i=(bits-4); i>=0; i=i-4){
        uint64_t offset = (inp >> i) & 0xF;
        screenPrintChar(hex_digits[offset]);
    }
}

char bin_digits[] = "01";
char bin_prefix[] = "0b/e";

void screenPrintB(uint64_t inp, uint8_t bits){
    screenPrintChars(bin_prefix);
    for(int i=(bits-1); i>=0; i--){
        uint64_t offset = (inp >> i) & 1;
        screenPrintChar(bin_digits[offset]);
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

uint8_t printStatus = 0;

void screenPrint(const char* fmt, ...){
    if(printStatus == 1) return;
    va_list args;
    va_start(args,fmt);
    while(1){
        if(*fmt == '/'){
            fmt++;
            if(*fmt == 'n'){
                screenNl();
            }else if(*fmt == '0'){
                screenClear();
            }else if(*fmt == 'e'){
                va_end(args);
                return;
            }else if(*fmt == 'x'){

                fmt++;
                switch(*fmt){
                case 'B':
                    screenPrintX((uint8_t)  va_arg(args, uint32_t), 8);  break;
                case 'W':
                    screenPrintX((uint16_t) va_arg(args, uint32_t), 16); break;
                case 'D':
                    screenPrintX((uint32_t) va_arg(args, uint32_t), 32); break;
                case 'Q': 
                    screenPrintX((uint64_t) va_arg(args, uint64_t), 64); break;
                }
            }else if(*fmt == 'b'){
                fmt++;

                switch(*fmt){
                case 'B':
                    screenPrintB((uint8_t)  va_arg(args, uint32_t), 8);  break;
                case 'W':
                    screenPrintB((uint16_t) va_arg(args, uint32_t), 16); break;
                case 'D':
                    screenPrintB((uint32_t) va_arg(args, uint32_t), 32); break;
                case 'Q':
                    screenPrintB((uint64_t) va_arg(args, uint64_t), 64); break;
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

void screenSetPrintStatus(uint8_t status){
    printStatus = status;
}

void screenMemoryDump(uint64_t start, uint64_t size){
    size = size & 0xFFFF;
    for(int i=0; i < size; i += SCREEN_MEMORY_DUMP_BYTES_PER_ROW){
        screenPrint("/xW: /e",i);
        for(int j=0; j<SCREEN_MEMORY_DUMP_BYTES_PER_ROW; j++){
            uint8_t* b = (uint8_t*) start + i + j;
            screenPrint("/xB /e ", *b);
        }
        screenNl();
    }
}

