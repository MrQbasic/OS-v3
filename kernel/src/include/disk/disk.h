#include "ahci.h"

#define DISK_MAX_AHCI 5

// 0 ------> nothing found
// BIT 0 --> AHCI found
int disk_searchDisks();

void disk_init();