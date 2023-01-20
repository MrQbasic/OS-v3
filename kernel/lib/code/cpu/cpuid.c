#include "../../include/cpu/cpuid.h"
#include "../../include/screen.h"

char cpuid_vendorString[] = "                /e";

char* cpuid_getVendor(){
    uint64_t regB, regC, regD;
    __asm__ __volatile__("movq $0x0, %%rax;"
                         "cpuid":
                         "=b"(regB),
                         "=c"(regC),
                         "=d"(regD)
                         :);
    cpuid_vendorString[0]  = (char) regB;
    cpuid_vendorString[1]  = (char) (regB >> 8);
    cpuid_vendorString[2]  = (char) (regB >> 16);
    cpuid_vendorString[3]  = (char) (regB >> 24);
    cpuid_vendorString[4]  = (char) regD;
    cpuid_vendorString[5]  = (char) (regD >> 8);
    cpuid_vendorString[6]  = (char) (regD >> 16);
    cpuid_vendorString[7]  = (char) (regD >> 24);
    cpuid_vendorString[8]  = (char) regC;
    cpuid_vendorString[9]  = (char) (regC >> 8);
    cpuid_vendorString[10] = (char) (regC >> 16);
    cpuid_vendorString[11] = (char) (regC >> 24);
    return cpuid_vendorString;
}

uint32_t cpuid_features[2];

uint32_t* cpuid_getFeatures(){
    __asm__ __volatile__("movq $0x01, %%rax;"
                         "cpuid":
                         "=c"(cpuid_features[0]),
                         "=d"(cpuid_features[1])
                         :);
    return cpuid_features;
}