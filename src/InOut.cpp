#include "InOut.h"
 
void DigitalOut(uint16_t Data)
{
    uint8_t Byte;                          //--- low byte ----------------------
                                           //
    PORT->Group[0].OUTCLR.reg = DATA_MASK; // data from 0 to 7

    Byte = MirrorByte(Data & 0x00FF);
    PORT->Group[0].OUTSET.reg = (uint32_t)Byte << 16; //

    delayMicroseconds(2); //

    PORT->Group[1].OUTCLR.reg = LOAD_L; // low byte strobe
    delayMicroseconds(2);               //
    PORT->Group[1].OUTSET.reg = LOAD_L; //
    delayMicroseconds(2);               //

    //--- high byte ----------------------                                                   //
    PORT->Group[0].OUTCLR.reg = DATA_MASK; // data from 8 to 15
    Byte = MirrorByte((Data >> 8) & 0x00FF);
    PORT->Group[0].OUTSET.reg = (uint32_t)Byte << 16;

    delayMicroseconds(2); //

    PORT->Group[0].OUTCLR.reg = LOAD_H; // high byte strobe
    delayMicroseconds(2);               //
    PORT->Group[0].OUTSET.reg = LOAD_H; //
    delayMicroseconds(2);               //

    PORT->Group[1].OUTCLR.reg = SYNC; // sync
    delayMicroseconds(40);            //
    PORT->Group[1].OUTSET.reg = SYNC;
    //
}

uint8_t MirrorByte(uint8_t ByteIn)
{
    uint8_t i;
    uint8_t MaskIn = 0x01;
    uint8_t MaskOut = 0x80;
    uint8_t ByteOut = 0x00;

    for (i = 0; i < 8; i++)
    {
        if (ByteIn & MaskIn)
            ByteOut |= MaskOut;
        MaskIn <<= 1;
        MaskOut >>= 1;
    }

    return ByteOut;
}

uint16_t DigitalIn(void) // the digital inputs are read
{                        // in 4 banks of 4 inputs each
    uint16_t DataIn = 0x0000;

    PORT->Group[0].OUTSET.reg = IN_BANK_MASK; // clear the bank selection

    PORT->Group[0].OUTCLR.reg = IN_BANK_0; // select bank 0
    delayMicroseconds(30);                 // wait a bit for stabilization
                                           // Increased in V_2 from 5uS to 30uS

    DataIn |= ((PORT->Group[0].IN.reg >> 8) & 0x000F); // read 4 input
    PORT->Group[0].OUTSET.reg = IN_BANK_0;             // deselect bank 0

    PORT->Group[0].OUTCLR.reg = IN_BANK_1;
    delayMicroseconds(30);
    DataIn |= ((PORT->Group[0].IN.reg >> 4) & 0x00F0);
    PORT->Group[0].OUTSET.reg = IN_BANK_1;

    PORT->Group[0].OUTCLR.reg = IN_BANK_2;
    delayMicroseconds(30);
    DataIn |= ((PORT->Group[0].IN.reg) & 0x0F00);
    PORT->Group[0].OUTSET.reg = IN_BANK_2;

    PORT->Group[0].OUTCLR.reg = IN_BANK_3;
    delayMicroseconds(30);
    DataIn |= ((PORT->Group[0].IN.reg << 4) & 0xF000);
    PORT->Group[0].OUTSET.reg = IN_BANK_3;

    DataIn ^= 0xFFFF;
    return DataIn;
}

void ConfDac(void)
{
    //-------- set both channels to +-10V ----
    //
    PORT->Group[0].OUTCLR.reg = DAC_CS; // enable chip select
    WriteToDac(0b00001100);             //
    WriteToDac(0b00000000);             //
    WriteToDac(0b00000100);             //
    PORT->Group[0].OUTSET.reg = DAC_CS; // disable chip select

    delayMicroseconds(2);
    //-- power on both channels and enable vref out
    //
    PORT->Group[0].OUTCLR.reg = DAC_CS; // enable chip select
    WriteToDac(0b00010000);             //
    WriteToDac(0b00000000);             //
    WriteToDac(0b00010101);             //
    PORT->Group[0].OUTSET.reg = DAC_CS; // disable chip select
}

//- DAC output -----------------------------------------------------------------------

void WriteDacValues(int16_t Data_A, int16_t Data_B) // data are normalized to 16 bit
{                                                   //even if the DAC is 12 bit
    PORT->Group[0].OUTCLR.reg = DAC_CS;             // enable chip select
    WriteToDac(0b00000000);                         // data for channel A
    WriteToDac((Data_A >> 8) & 0x00FF);             //
    WriteToDac(Data_A & 0x00FF);                    //
    PORT->Group[0].OUTSET.reg = DAC_CS;             // disable chip select

    delayMicroseconds(2);

    PORT->Group[0].OUTCLR.reg = DAC_CS; // enable chip select
    WriteToDac(0b00000010);             // data for channel B
    WriteToDac((Data_B >> 8) & 0x00FF); //
    WriteToDac(Data_B & 0x00FF);        //
    PORT->Group[0].OUTSET.reg = DAC_CS; // disable chip select
}

//- DAC bit bang ----------------------------------------------------------------------

void WriteToDac(int8_t Data)

{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (Data & 0x80)
            PORT->Group[0].OUTSET.reg = DAC_DATA;
        else
            PORT->Group[0].OUTCLR.reg = DAC_DATA;

        PORT->Group[0].OUTCLR.reg = DAC_CK;
        PORT->Group[0].OUTSET.reg = DAC_CK;

        Data = Data << 1;
    }
}

//- ADC input ----------------------------------------------------------------------

int ReadAdc(uint8_t Channel) // data are normalized to signed 16 bit
                             // even if the ADC is 12 bit unsigned
{
    int16_t Result;

    ADC->INPUTCTRL.reg = 0x00001800 | (Channel + 2); // select channel - gain 1 - muxneg=internal gnd
    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;           // reset ready flag
    ADC->SWTRIG.bit.START = 1;                       // start conversion

    while (ADC->INTFLAG.bit.RESRDY == 0)
    {
    }                         // wait for convertion to complete
    Result = ADC->RESULT.reg; // read the result

    // tweak the result a little bit

    Result -= 11;   // offset  -9
    if (Result < 0) //
    {               //
        Result = 0; //
    }               //

    Result = (float)Result * 1.0066; // gain  1.003
    if (Result > 0x0FFF)             //
    {                                //
        Result = 0x0FFF;             //
    }                                //

    Result = Result << 3; // normalize the result to 16 bit
    return Result;        //
}

/*int bitArrayToInt32(bool arr[], int count)
{
  int ret = 0;
  int tmp;
  for (int i = 0; i < count; i++)
  {
    tmp = arr[i];
    ret |= tmp << (count - i - 1);
  }
  return ret;
}*/
