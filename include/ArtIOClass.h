#ifndef ART_IO_CLASS_H
#define ART_IO_CLASS_H

#include "InOut.h"

#include "EasyCAT/EasyCAT.h"

#undef min
#undef max

#include <vector>
#include <bitset>

struct Pulse_T
{
    int m_TimeoutMs{0}, m_nTimerId{0};
    bool m_bSettedvalue{false}, m_bValueOnStart{false};

    void nullifyMe()
    {
        m_TimeoutMs = 0;
        m_nTimerId = 0;
        m_bSettedvalue = false;
        m_bValueOnStart = false;
    };
};

static Pulse_T nPulseId[16]; //must be out of class
static EasyCAT *m_ptrEasyCat;

class ArtIOClass
{
protected:
    static uint16_t m_nCurrentOutputState;

    static const uint8_t N_MIN_INPORT_NUM, N_MAX_INPORT_NUM;
    static const uint8_t N_MIN_OUTPORT_NUM, N_MAX_OUTPORT_NUM;
    
public:
    static void initializeBoard(EasyCAT *ptrEasyCat); //DONE code form main

    //direct IOs
    static bool getInputState(int nInputPort);                                  //DONE
    static bool getOutputState(int nOutputPort);                                //DONE
    static void setOutputState(int nOutputPort, bool bSettedValue);             //DONE
    static void setPulse(int nOutputPort, bool bPulsedValue, int nPulseTimeMs); //DONE
    static void setOutputState(uint16_t commonstate);
    static bool isTimerPassed(int &nTimerId, int nTimeoutMs); //

    static void doIOLogic();

    static uint16_t getCommonOutputState();
protected:
    ArtIOClass();
    ~ArtIOClass();
};

#endif //ART_LOWLEVEL_CLASS_H
