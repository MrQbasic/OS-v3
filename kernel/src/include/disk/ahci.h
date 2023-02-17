#include "def.h"

#define	SATA_SIG_ATA	0x00000101
#define	SATA_SIG_ATAPI	0xEB140101
#define	SATA_SIG_SEMB	0xC33C0101
#define	SATA_SIG_PM  	0x96690101
 
#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4
 
#define AHCI_HBA_PORT_IPM_ACTIVE 1
#define AHCI_HBA_PORT_DET_PRESENT 3

#define AHCI_HBA_PORT_CMD_ST  0x0001
#define AHCI_HBA_PORT_CMD_FRE 0x0010
#define AHCI_HBA_PORT_CMD_FR  0x4000
#define AHCI_HBA_PORT_CMD_CR  0x8000
 

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
    uint32_t signature;
    uint32_t sata_status;
    uint32_t sata_control;
    uint32_t sata_error;
    uint32_t sata_active;
    uint32_t ci;
    uint32_t sata_notification;
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
    AHCI_HBA_PORT ports[32];

} AHCI_HBA;


//returns:
//  0 -> OK
//  1 -> ERROR
int ahci_init_disk(uint16_t disk_count, uint16_t* disk_addr);

int ahci_port_check_type(AHCI_HBA_PORT* port);

void ahci_port_stop_cmd(AHCI_HBA_PORT* port);

void ahci_port_start_cmd(AHCI_HBA_PORT* port);

