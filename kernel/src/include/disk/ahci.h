#include "def.h"
#include "ahci_fis.h"

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

typedef struct HBA_PRDT_ENTRY
{
	uint32_t data_base_lower;
	uint32_t data_base_upper;
	uint32_t zero_0;
	uint32_t data_byte_count:22;
	uint32_t zero_1:9;
	uint32_t interrupt:1;
} HBA_PRDT_ENTRY;

typedef struct AHCI_HBA_CMD_TABLE{
	uint8_t  cmd_FIS[64];
	uint8_t  atapi_cmd[16];
	uint8_t  reserved[48];
	HBA_PRDT_ENTRY	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
} AHCI_HBA_CMD_TABLE;

typedef struct AHCI_HBA_CMD_HEADER{
	uint8_t  cmd_FIS_length:5;		// Command FIS length in DWORDS, 2 ~ 16
	uint8_t  a:1;		// ATAPI
	uint8_t  w:1;		// Write, 1: H2D, 0: D2H
	uint8_t  p:1;		// Prefetchable
	uint8_t  r:1;		// Reset
	uint8_t  b:1;		// BIST
	uint8_t  c:1;		// Clear busy upon R_OK
	uint8_t  rsv0:1;	// Reserved
	uint8_t  pmp:4;		// Port multiplier port
	uint16_t phy_region_descriptor_table_length;		//in entries
    volatile uint32_t phy_region_descriptor_byte_cnt;
	uint32_t cmd_tbl_base_lower;
	uint32_t cmd_tbl_base_upper;
    uint32_t reserved[4];
} AHCI_HBA_CMD_HEADER;

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


typedef struct AHCI_LIN_ADDRESS_LIST_ENTRY{
    //make it a list
    uint64_t next;
    //id (port adress)
    uint64_t id;
    //phy base addresses
    uint64_t cmd_list;
    uint64_t fis;
    uint64_t cmd_tbl[32];
} AHCI_LIN_ADDRESS_LIST_ENTRY;



//returns:
//  0 -> OK
//  1 -> ERROR
int ahci_init_disk(uint16_t disk_count, uint16_t* disk_addr);

//returns:
//  0 -> ERROR
//  1 -> SATA
//  2 -> SEMB
//  3 -> PM
//  4 -> SATAPI
int ahci_port_check_type(AHCI_HBA_PORT* port);

//returns:
//  0 -> OK
//  1 -> ERROR
int ahci_port_read(AHCI_HBA_PORT *port, uint64_t start, uint32_t count, uint64_t buf);

void ahci_port_stop_cmd(AHCI_HBA_PORT* port);

void ahci_port_start_cmd(AHCI_HBA_PORT* port);

void ahci_port_rebase(AHCI_HBA* ahci_hba);