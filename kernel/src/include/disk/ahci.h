#include "def.h"

typedef volatile struct AHCI_HBA_PORT{
    uint32_t clb;
    uint32_t clbu;
    uint32_t fb;
    uint32_t fbu;
    uint32_t is;
    uint32_t ie;
    uint32_t cmd; 
    uint32_t zero_0;
    uint32_t tfd;
    uint32_t sig;
    uint32_t ssts;
    uint32_t sctl;
    uint32_t serr;
    uint32_t sact;
    uint32_t ci;
    uint32_t sntf;
    uint32_t fbs;
    uint32_t zero_1[11];
    uint32_t vendor[4];
} AHCI_HBA_PORT;

typedef volatile struct AHCI_HBA{
    //Generic config
    uint32_t capability;
    uint32_t ghc;
    uint32_t interrupt_status;
    uint32_t ports_implemented;
    uint32_t version;
    uint32_t ccc_control;
    uint32_t ccc_ports;
    uint32_t em_location;
    uint32_t em_control;
    uint32_t capability_2;
    uint32_t handoff;
    //zero
    uint8_t zero[116];
    //vendor
    uint8_t vendor[96];
    //ports
    AHCI_HBA_PORT ports[0];

} AHCI_HBA;


//returns:
//  0 -> OK
//  1 -> ERROR
int ahci_init_disk(uint16_t disk_count, uint16_t* disk_addr);

int ahci_port_check_type(AHCI_HBA_PORT* port);