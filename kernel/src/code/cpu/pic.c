#include "cpu/pic.h"
#include "cpu/port.h"
#include "def.h"

void pic_remap(uint8_t pic1, uint8_t pic2){
    //save marks
    uint8_t old_1 = inb(PIC_1_DATA);
    uint8_t old_2 = inb(PIC_2_DATA);
    //send init cmd
    outb(PIC_1_CMD, PIC_CMD_INIT);
    outb(PIC_2_CMD, PIC_CMD_INIT);
    //set offset
    outb(PIC_1_DATA, pic1);
    outb(PIC_2_DATA, pic2);
    //master slave conf
    outb(PIC_1_DATA, 0b00000100);
    outb(PIC_2_DATA, 0b00000010);
    //ICW4 mode
    outb(PIC_1_DATA, PIC_ICW4_8086);
    outb(PIC_2_DATA, PIC_ICW4_8086);
    //restore marks
    outb(PIC_1_DATA, old_1);
    outb(PIC_2_DATA, old_2);
}

void pic_disable(){
    outb(PIC_1_DATA, PIC_DISABLE);
    outb(PIC_2_DATA, PIC_DISABLE);
}

void pic_eoi(){
    outb(PIC_1_CMD, PIC_EOI);
    outb(PIC_2_CMD, PIC_EOI);
}