#include "def.h"

#define PCI_CONFIG_ADDRESS 0x0CF8
#define PCI_CONFIG_DATA    0x0CFC
#define PCI_CONFIG_FORWARD 0x0CFA

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

void pciPrintInfo();

// ret -> table ptr
// table & 0x00FF = bus
// table & 0xFF00 = slot
uint16_t* pciGetDeviceTable(uint16_t* amount);

void pciCheckBus();
