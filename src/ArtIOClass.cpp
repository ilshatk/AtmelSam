#include "ArtIOClass.h"
uint16_t ArtIOClass::m_nCurrentOutputState = 0;
const uint8_t ArtIOClass::N_MIN_INPORT_NUM = 1;
const uint8_t ArtIOClass::N_MAX_INPORT_NUM = 16;
const uint8_t ArtIOClass::N_MIN_OUTPORT_NUM = 1;
const uint8_t ArtIOClass::N_MAX_OUTPORT_NUM = 16;

ArtIOClass::ArtIOClass()
{
}

ArtIOClass::~ArtIOClass()
{
}

bool ArtIOClass::getInputState(int nInputPort)
{
    if (nInputPort < N_MIN_INPORT_NUM && nInputPort > N_MAX_INPORT_NUM)
        return false;
    std::bitset<16> inputBitset(DigitalIn());
    return (inputBitset.test(nInputPort - 1));
}

bool ArtIOClass::getOutputState(int nOutputPort)
{
    if (nOutputPort < N_MIN_OUTPORT_NUM && nOutputPort > N_MAX_OUTPORT_NUM)
        return false;
    std::bitset<16> outputBitset(m_nCurrentOutputState);
    return (outputBitset.test(nOutputPort - 1));
}

void ArtIOClass::setOutputState(int nOutputPort, bool bSettedValue)
{
    std::bitset<16> settedValue;
    if (bSettedValue)
    {
        settedValue.reset();
        settedValue.set(nOutputPort - 1, bSettedValue);
        m_nCurrentOutputState |= settedValue.to_ulong();
    }
    else
    {
        settedValue.set();
        settedValue.set(nOutputPort - 1, bSettedValue);
        m_nCurrentOutputState &= settedValue.to_ulong();
    }
    DigitalOut(m_nCurrentOutputState);
}

void ArtIOClass::initializeBoard(EasyCAT *ptrEasyCAT)
{

    m_ptrEasyCat = ptrEasyCAT;
    Serial.begin(115200); // serial line initialization (USB)
                          // the USB is always enable but it is
                          // present only on the debug board

    //-- pins directions ----------------------------------------------

    PORT->Group[0].DIRSET.reg = (DATA_MASK | LOAD_H); // set the direction for the digital outputs pins
    PORT->Group[1].DIRSET.reg = (SYNC | LOAD_L);      //
                                                      //
    PORT->Group[0].DIRCLR.reg = FAULT_H;              //
    PORT->Group[1].DIRCLR.reg = FAULT_L;              //

    PORT->Group[0].DIRCLR.reg = DIGI_IN_NIBBLE_MASK; // set the direction for the digital inputs pins

    PORT->Group[0].DIRSET.reg = (DAC_CS | DAC_CK | DAC_DATA); // set the direction for the DAC pins

    PORT->Group[0].DIRSET.reg = WDOG; // set the direction for the WDOG pin
    PORT->Group[1].DIRSET.reg = LED;  // set the direction for the LED pin

    //-- pins configurations ------------------------------------------

    PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPMUX | 0x00010000 | 0x00000038;
    PORT->Group[1].WRCONFIG.reg = PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPMUX | 0x00010000 | 0x00000300;

    PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | 0x0000EFC4;
    PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | 0x000018FF;

    PORT->Group[1].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | 0x0000000C;
    PORT->Group[1].WRCONFIG.reg = PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | 0x000000C0;

    //-- ADC configuration --------------------------------------------

    ADC->CTRLA.bit.ENABLE = 1; // Enable ADC

    ADC->CTRLB.bit.PRESCALER = 3; // adc_clk = 48MHz/32 = 1.5MHz

    ADC->CTRLB.bit.RESSEL = 1;        // enable averaging
    ADC->AVGCTRL.bit.SAMPLENUM = 0x5; // 32 samples each conversion
    ADC->AVGCTRL.bit.ADJRES = 4;      //

    ADC->SAMPCTRL.reg = 0x07; // sample time

    ADC->REFCTRL.bit.REFCOMP = 1;   // reference offset compensation
    ADC->REFCTRL.bit.REFSEL = 0x03; // external reference

    PORT->Group[0].OUTCLR.reg = WDOG;
    PORT->Group[0].OUTSET.reg = WDOG; // reset the watchdog
    ConfDac();

    DigitalOut(m_nCurrentOutputState);
}

void ArtIOClass::setPulse(int nOutputNumber, bool pulsedValue, int nMilliseconds)
{
    nPulseId[nOutputNumber - 1].m_nTimerId = 0;
    nPulseId[nOutputNumber - 1].m_TimeoutMs = nMilliseconds;
    nPulseId[nOutputNumber - 1].m_bSettedvalue = pulsedValue;
    nPulseId[nOutputNumber - 1].m_bValueOnStart = getOutputState(nOutputNumber);
    setOutputState(nOutputNumber, pulsedValue);
}

bool ArtIOClass::isTimerPassed(int &nTimerId, int nTimeoutMs)
{
    if (nTimerId == 0)
    {
        nTimerId = millis();
        return false;
    }
    else
    {
        int nCurrentTime, nDeltaTime;
        nCurrentTime = millis();
        if (nTimerId < nCurrentTime)
        {
            nDeltaTime = nCurrentTime - nTimerId;
            return (nDeltaTime > nTimeoutMs);
        }
        else
        {
            if (nTimerId == nCurrentTime)
            {
                return false;
            }
            else
            {
                nTimerId = nTimerId - 16777215; //maxCurretnTimerValue 2147483647 (form Ilshat) //24-bits
            }
        }
    }
    return false;
}

bool ArtIOClass::ExtSens(uint16_t sensinput)
{
    if (m_ptrEasyCat->BufferOut.Cust.SensSignalFromPrevBarda == (1 << (sensinput - 1)))
    {
        // ArtIOClass::setOutputState(16, true);
        return (true);
    }
    else
    {
        // ArtIOClass::setOutputState(16, false);
        return (false);
    }
}

bool ArtIOClass::TookProd()
{
    if (m_ptrEasyCat->BufferOut.Cust.TookProduct == 1)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}

void ArtIOClass::StackReady(bool ready)
{
    if (ready)
    {
        m_ptrEasyCat->BufferIn.Cust.GaveStack = 1;
    }
    else
    {
        m_ptrEasyCat->BufferIn.Cust.GaveStack = 0;
    }
}

void ArtIOClass::GaveStack(bool gave)
{
    if (gave)
    {
        m_ptrEasyCat->BufferIn.Cust.GaveStack = 1;
    }
    else
    {
        m_ptrEasyCat->BufferIn.Cust.GaveStack = 0;
    }
}

bool ArtIOClass::readySignalFromNext()
{
    if (m_ptrEasyCat->BufferOut.Cust.NextConvReadySignal & 1 == 1)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}

bool ArtIOClass::readySignalFromNext(int convnum, int boardnum) //сигнал готовности следующих четырех конвейеров
{
    if (boardnum == 1)
    {
        if (m_ptrEasyCat->BufferOut.Cust.NextConvReadySignal1 & convnum == 1)
        {
            return (true);
        }
        else
        {
            return (false);
        }
    }

    if (boardnum == 2)
    {
        if (m_ptrEasyCat->BufferOut.Cust.NextConvReadySignal1 & convnum == 1)
        {
            return (true);
        }
        else
        {
            return (false);
        }
    }
}

bool ArtIOClass::ExtDevReady() // для приема сигнала готов с диспенсера на следующий конвейер
{
    if (m_ptrEasyCat->BufferOut.Cust.Flags == 1)
    {
        // ArtIOClass::setOutputState(16, true);
        return (true);
    }
    else
    {
        // ArtIOClass::setOutputState(16, false);
        return (false);
    }
}

void ArtIOClass::DevReady(bool ready) // для передачи сигнала готов с диспенсера на следующий конвейер
{
    if (ready)
    {
        m_ptrEasyCat->BufferIn.Cust.DevReady = 1;
    }
    else
    {
        m_ptrEasyCat->BufferIn.Cust.DevReady = 0;
    }
}

void ArtIOClass::DevReady(int posnum) // для передачи сигнала какая позиция готова передать с цепного конвейера на следующий конвейер
{
    m_ptrEasyCat->BufferIn.Cust.DevReady | posnum ;
}

void ArtIOClass::OnPosition(uint8_t pos)
{
    m_ptrEasyCat->BufferIn.Cust.OnPosition = pos;
}

uint16_t ArtIOClass::ReqPos()
{
    return (m_ptrEasyCat->BufferOut.Cust.ReqPosition);
}

bool ArtIOClass::LoaUnloadind()
{
    if (m_ptrEasyCat->BufferOut.Cust.Flags & 1 == 1)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}

void ArtIOClass::Error(uint8_t error)
{
    m_ptrEasyCat->BufferIn.Cust.OutFault = error;
}

int ArtIOClass::ARTTimerGetTime()
{
    int curTime;
    curTime = millis();
    if (curTime == 0)
    {
        return (1);
    }

    return (curTime);
}

void ArtIOClass::doIOLogic()
{
    if (m_ptrEasyCat != nullptr)
    {
        if (m_ptrEasyCat->MainTask() == ESM_OP)
        {
            m_ptrEasyCat->BufferIn.Cust.OutState = ArtIOClass::getCommonOutputState();
            m_ptrEasyCat->BufferIn.Cust.DigiIn = DigitalIn();
            m_ptrEasyCat->BufferIn.Cust.SensSignalOnNextBarda = DigitalIn();

            //-----------------Это сброс ошибки конвейера, нужно переделать, но пока так
            if (m_ptrEasyCat->BufferOut.Cust.DigiOut == 32 || m_ptrEasyCat->BufferOut.Cust.DigiOut == 16 || m_ptrEasyCat->BufferOut.Cust.DigiOut == 8 || m_ptrEasyCat->BufferOut.Cust.DigiOut == 24 || m_ptrEasyCat->BufferOut.Cust.DigiOut == 48 || m_ptrEasyCat->BufferOut.Cust.DigiOut == 40 || m_ptrEasyCat->BufferOut.Cust.DigiOut == 56)
            {
                //DigitalOut(m_ptrEasyCat->BufferOut.Cust.DigiOut);
            }
            else
            {
                /* ArtIOClass::setOutputState(4, false);
                ArtIOClass::setOutputState(5, false);
                ArtIOClass::setOutputState(6, false);*/
            }
        }
    }

    //pulse logic control
    for (int i = 0; i < N_MAX_OUTPORT_NUM; i++)
    {
        if (nPulseId[i].m_TimeoutMs > 0)
        {
            setOutputState(i + 1, nPulseId[i].m_bSettedvalue);
            if (isTimerPassed(nPulseId[i].m_nTimerId, nPulseId[i].m_TimeoutMs))
            {
                //drop pulse
                setOutputState(i + 1, nPulseId[i].m_bValueOnStart);
                nPulseId[i].nullifyMe();
            }
        }
    }
}

uint16_t ArtIOClass::getCommonOutputState()
{
    return m_nCurrentOutputState;
}

void ArtIOClass::setOutputState(uint16_t nOutputCommonState)
{
    m_nCurrentOutputState = nOutputCommonState;
    DigitalOut(m_nCurrentOutputState);
}


bool ArtIOClass::CHK_ACTIVE_NTIME(bool sens_in, int *timer_in, int delta_time) //sensor,timer for check, active time before return true
{
   int curTime, deltaTime;

   if (!sens_in)
   {
      (*timer_in) = 0;
   }
   else
   {
      if ((*timer_in) == 0)
      {
         (*timer_in) = millis();
      }

      curTime = millis();

      if ((*timer_in) < curTime)
      {
         deltaTime = curTime - (*timer_in);
         return (deltaTime > delta_time);
      }
      else //exceptions check
      {
         if ((*timer_in) == curTime)
         {
            return (false);
         }
         else
         {
            if ((*timer_in) > curTime)
            {
               timer_in = timer_in - 2147483647;
               deltaTime = curTime - (*timer_in);
               return (deltaTime > delta_time);
            }
         }
      }
   }
   return (false);
}