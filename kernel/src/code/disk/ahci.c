#include "disk/ahci.h"
#include "pci/pci.h"
#include "mem/mem.h"
#include "screen.h"

uint64_t* ahci_abar_phy;
AHCI_HBA** ahci_abar;
AHCI_LIN_ADDRESS_LIST_ENTRY* lin_address_list = 0;

int ahci_init_disk(uint16_t disk_count, uint16_t* disk_addr){
    //setup abar ports
    ahci_abar_phy = (uint64_t*) mem_alloc(sizeof(uint64_t) * disk_count, 0);
    ahci_abar = (AHCI_HBA**) mem_alloc(sizeof(AHCI_HBA*) * disk_count, 0);
    for(int i=0; i<disk_count; i++){
        uint8_t  bus = (uint8_t) (disk_addr[i] >> 0);
        uint8_t slot = (uint8_t) (disk_addr[i] >> 8);
        ahci_abar_phy[i]  = 0x000000000000FFFF & (pciConfigReadWord(bus, slot, 0, 0x24) <<  0);
        ahci_abar_phy[i] |= 0x00000000FFFF0000 & (pciConfigReadWord(bus, slot, 0, 0x26) << 16);
        ahci_abar[i] = (AHCI_HBA*) mem_alloc(0x1000, 0x1000);
        //remap memory so match abar
        int error = 0;
        uint64_t paddr;
        error |= page_trace_PML4((uint64_t) ahci_abar[i], &paddr);
        error |= mem_pfree(paddr,1);
        //unexpected error 
        if(error != 0){
            screenPrint("OUT OF MEMORY!/n/e");
            while(1);
        }
        page_map_PML4((uint64_t) ahci_abar[i], ahci_abar_phy[i], 0b00010001, 0x00);
        //print implemented ports / devices
        uint32_t impl = ahci_abar[i]->ports_implemented;
        int port_index = 0;
        while(impl != 0){
            int type = ahci_port_check_type(&(ahci_abar[i]->ports[port_index]));
            if(type == AHCI_DEV_PM)     screenPrint("PM /e");
            if(type == AHCI_DEV_SATA)   screenPrint("SATA /e");
            if(type == AHCI_DEV_SATAPI) screenPrint("SATAPI /e");
            if(type == AHCI_DEV_SEMB)   screenPrint("SEMB /e");
            if(type != AHCI_DEV_NULL){
                screenPrint("DEV @ PORT: /xB LIN: /xQ PHY: /xQ/n/e",(uint8_t)port_index, ahci_abar[i],ahci_abar_phy[i]);
            }
            port_index++;
            impl = impl >> 1;
        }
        //rebase ports
        ahci_port_rebase(ahci_abar[i]);
        ahci_port_read(ahci_abar[i]->ports,0,1,0);
    }
    //setup 

    return 0;
}

int ahci_port_check_type(AHCI_HBA_PORT* port){
    uint32_t status = port->sata_status;
    uint8_t ipm = (status >> 8) & 0x0F;
    uint8_t det = status & 0x0F;
    //check if a drive is usable
    if(det != AHCI_HBA_PORT_DET_PRESENT) return AHCI_DEV_NULL;
    if(ipm != AHCI_HBA_PORT_IPM_ACTIVE ) return AHCI_DEV_NULL;
    //check type
    uint32_t signature = port->signature;
    if(signature == SATA_SIG_ATAPI) return AHCI_DEV_SATAPI;
    if(signature == SATA_SIG_SEMB)  return AHCI_DEV_SEMB;
    if(signature == SATA_SIG_PM)    return AHCI_DEV_PM;
    return AHCI_DEV_SATA;
}

void ahci_port_stop_cmd(AHCI_HBA_PORT* port){
    port->cmd &= ~AHCI_HBA_PORT_CMD_ST;
    port->cmd &= ~AHCI_HBA_PORT_CMD_FRE;
    while(1){
        if(port->cmd & AHCI_HBA_PORT_CMD_FR) continue;
        if(port->cmd & AHCI_HBA_PORT_CMD_CR) continue;
        break;
    }
}

void ahci_port_start_cmd(AHCI_HBA_PORT* port){
    while(port->cmd & AHCI_HBA_PORT_CMD_CR);
    port->cmd |= AHCI_HBA_PORT_CMD_FRE;
    port->cmd |= AHCI_HBA_PORT_CMD_ST;
}

void ahci_append_to_lin_address_list(AHCI_LIN_ADDRESS_LIST_ENTRY* entry){
    AHCI_LIN_ADDRESS_LIST_ENTRY* current_entry = lin_address_list;
    if(current_entry == 0){
        lin_address_list = entry;
        return;
    }
    while(current_entry->next != 0){
        current_entry = (AHCI_LIN_ADDRESS_LIST_ENTRY*) current_entry->next;
    }
    current_entry->next = (uint64_t) entry;
}

void ahci_port_rebase(AHCI_HBA* ahci_hba){
    //setup going over all ports
    uint64_t impl = (uint64_t) ahci_hba->ports_implemented;
    impl = impl << 1;
    int port_index = -1;
    while(impl != 0){
        //next port
        port_index++;
        impl = impl >> 1;
        int type = ahci_port_check_type(&(ahci_hba->ports[port_index]));
        if(type != AHCI_DEV_SATA) continue;
        //get port ready
        AHCI_HBA_PORT* port = &(ahci_hba->ports[port_index]);
        ahci_port_stop_cmd(port);
        //setup address list entry
        AHCI_LIN_ADDRESS_LIST_ENTRY* addr_list = 
                    (AHCI_LIN_ADDRESS_LIST_ENTRY*) mem_alloc(sizeof(AHCI_LIN_ADDRESS_LIST_ENTRY),0);
        addr_list->next = 0;
        addr_list->id = (uint64_t) port;
        ahci_append_to_lin_address_list(addr_list);
        //remap command list
        uint64_t cmd_list_virt = mem_alloc(0x400,0x400);
        uint64_t cmd_list_phy;
        page_trace_PML4(cmd_list_virt, &cmd_list_phy);
        cmd_list_phy += cmd_list_virt & 0x0FFF;
        //point hardware to new space
        port->clb  = (uint32_t)  cmd_list_phy;
        port->clbu = (uint32_t) (cmd_list_phy >> 32);
        //clear allocated memory
        mem_write((void*) cmd_list_virt, 0,1024);
        //add to phy address list entry
        addr_list->cmd_list = cmd_list_virt; 
        //remap fis
        uint64_t fis_virt = mem_alloc(0x100, 0x100);
        uint64_t fis_phy;
        page_trace_PML4(fis_virt, &fis_phy);
        fis_phy += cmd_list_virt & 0x0FFF;
        //point hardware to new space
        port->fb  = (uint32_t)  cmd_list_phy;
        port->fbu = (uint32_t) (cmd_list_phy >> 32);
        //clear allocated memory
        mem_write((void*) fis_virt, 0, 256);
        //add to phy address list entry
        addr_list->fis = fis_virt;
        //remap command 
        AHCI_HBA_CMD_HEADER *cmdheader = (AHCI_HBA_CMD_HEADER*) cmd_list_virt;
        for(int i=0; i<32; i++){
            cmdheader[i].phy_region_descriptor_table_length = 8;
            //remap cmd tbl
            uint64_t virt_addr = mem_alloc(0x100, 0x100); 
            uint64_t phy_addr;
            page_trace_PML4(virt_addr, &phy_addr);
            //point hardware to new space
            cmdheader[i].cmd_tbl_base_lower = (uint32_t) (phy_addr);
            cmdheader[i].cmd_tbl_base_upper = (uint32_t) (phy_addr >> 32);
            //clear allocated memory
            uint64_t* tmp = (uint64_t*) virt_addr;
            for(int i=0; i<(256/8); i++){
                *tmp = 0;
            }
            //add to phy address list
            addr_list->cmd_tbl[i] = virt_addr;
        }
        //restart port
        ahci_port_start_cmd(port);
    }
}

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

 
// Find a free command list slot
int find_cmdslot(AHCI_HBA_PORT *port){
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sata_active | port->ci);
    screenPrint("/xD/n/e",slots);
	for (int i=0; i<1; i++){
        
		if ((slots&1) == 0) return i;
		slots >>= 1;
	}
	screenPrint("Cannot find free command list entry/n/e");
	return -1;
}

#define HBA_PxIS_TFES (1 << 30) 
#define ATA_CMD_READ_DMA_EX 0x25

//1 is bad
int ahci_port_read(AHCI_HBA_PORT *port, uint64_t start, uint32_t count, uint64_t buf){
    screenPrint("/0---AHCI-READ---/n/e");
    //search address list
    if(lin_address_list == 0) return 1;
    AHCI_LIN_ADDRESS_LIST_ENTRY* addr_list = lin_address_list;
    while(addr_list->id != (uint64_t) port){
        if(addr_list->next == 0) return 1;
        addr_list = (AHCI_LIN_ADDRESS_LIST_ENTRY*) addr_list->next;
    }
    screenPrint("PORT ADDR    : /xQ/n/e",(uint64_t) port);
    screenPrint("PORT ADDR_TBL: /xQ/n/e",(uint64_t) addr_list);
    //find cmd slot
    port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1) return 1;
    
	AHCI_HBA_CMD_HEADER *cmdheader = (AHCI_HBA_CMD_HEADER*) addr_list->cmd_list;

	cmdheader += slot;
	cmdheader->cmd_FIS_length = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->phy_region_descriptor_table_length = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
 
	AHCI_HBA_CMD_TABLE *cmdtbl;
    cmdtbl = (AHCI_HBA_CMD_TABLE*)((uint64_t) cmdheader->cmd_tbl_base_lower);
    cmdtbl = (AHCI_HBA_CMD_TABLE*)((uint64_t) cmdheader->cmd_tbl_base_upper << 32);
    
    uint64_t size = sizeof(AHCI_HBA_CMD_TABLE) + (cmdheader->phy_region_descriptor_table_length-1)*sizeof(HBA_PRDT_ENTRY);
    uint8_t *tmp = (uint8_t*) cmdtbl;
    for(int i=0; i<size; i++){
       *tmp = 0;
    }

	// 8K bytes (16 sectors) per PRDT
    int i=0;
	for (i=0; i<cmdheader->phy_region_descriptor_table_length-1; i++)
	{
		cmdtbl->prdt_entry[i].data_base_lower = (uint32_t) (buf);
        cmdtbl->prdt_entry[i].data_base_upper = (uint32_t) (buf >> 32);
		cmdtbl->prdt_entry[i].data_byte_count = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].interrupt = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].data_base_lower = (uint32_t) (buf);
    cmdtbl->prdt_entry[i].data_base_upper = (uint32_t) (buf >> 32);
	cmdtbl->prdt_entry[i].data_byte_count = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].interrupt = 1;
 
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cmd_FIS);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)start;
	cmdfis->lba1 = (uint8_t)(start >> 8);
	cmdfis->lba2 = (uint8_t)(start >> 16);
	cmdfis->lba3 = (uint8_t)(start >> 24);
	cmdfis->lba4 = (uint8_t)(start >> 32);
	cmdfis->lba5 = (uint8_t)(start >> 40);
    
    cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;

	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
		spin++;
	}
	if (spin == 1000000){
		screenPrint("Port is hung/n/e");
		return 1;
	}
    
	port->ci = 1<<slot;
	while (1){
		if ((port->ci & (1<<slot)) == 0) break;
		if (port->is & HBA_PxIS_TFES){
			screenPrint("Read disk error/n/e");
			return 1;
		}
	}
	if (port->is & HBA_PxIS_TFES){
		screenPrint("Read disk error/n/e");
		return 1;
	}
    screenPrint("/nREAD GOOD!/e");
	return 0;
}
