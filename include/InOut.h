#ifndef INOUT_H
#define INOUT_H

#include "Arduino.h"

#undef min
#undef max

#define WDOG      (1<<2)                  // watch dog - Port A
#define LED       (1<<3)                  // led - Port B

#define LOAD_H    (1<<28)                 // digital outputs load high byte - Port A
#define LOAD_L    (1<<22)                 // digital outputs load low byte - Port B
#define SYNC      (1<<2)                  // digital outputs sync - Port B
#define DATA_MASK 0x00FF0000              // digital outputs data - Port A

#define FAULT_H   (1<<27)                 // digital outputs fault high byte - Port A 
#define FAULT_L   (1<<23)                 // digital outputs fault low byte - Port B 

#define DAC_CS    (1<<13)                 // DAC chip select - Port A
#define DAC_CK    (1<<14)                 // DAC clock - Port A
#define DAC_DATA  (1<<15)                 // DAC data - Port A

#define IN_BANK_0 (1<<18)                 // digital inputs select bank 0 - Port A
#define IN_BANK_1 (1<<17)                 // digital inputs select bank 1 - Port A
#define IN_BANK_2 (1<<19)                 // digital inputs select bank 2 - Port A
#define IN_BANK_3 (1<<16)                 // digital inputs select bank 3 - Port A
#define IN_BANK_MASK 0x000F0000           // digital inputs all banks - Port A 
#define DIGI_IN_NIBBLE_MASK 0x00000F00    // digital inputs data - Port A

uint8_t MirrorByte(uint8_t ByteIn);
void DigitalOut(uint16_t Data);
uint16_t DigitalIn(void);
void WriteToDac(int8_t Data);
int ReadAdc(uint8_t Channel);
void WriteDacValues (int16_t Data_A , int16_t Data_B );
void ConfDac(void);

#endif