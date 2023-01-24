#include "def.h"

#define PIC_1_CMD  0x20
#define PIC_1_DATA 0x21
#define PIC_2_CMD  0xA0
#define PIC_2_DATA 0xA1

#define PIC_ICW1_ICW4       0x01
#define PIC_ICW1_SINGLE     0x02
#define PIC_ICW1_INTERVAL4  0x04
#define PIC_ICW1_LEVEL      0x08
#define PIC_ICW1_INIT       0x10
#define PIC_ICW4_8086       0x01
#define PIC_ICW4_AUTO       0x02
#define PIC_ICW4_BUF_SLAVE  0x08
#define PIC_ICW4_BUF_MASTER 0x0C
#define PIC_ICW4_SFNM       0x10
#define PIC_EOI             0x20
#define PIC_DISABLE         0xFF

#define PIC_CMD_INIT        (PIC_ICW1_INIT | PIC_ICW1_ICW4)

void pic_remap(uint8_t pic1, uint8_t pic2);

void pic_disable();

void pic_eoi();