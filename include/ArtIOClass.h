#ifndef ART_IO_CLASS_H
#define ART_IO_CLASS_H

#include "InOut.h"

#include "EasyCAT\EasyCAT.h"

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

static Pulse_T nPulseId[16]; // must be out of class
static EasyCAT *m_ptrEasyCat;

class ArtIOClass
{
protected:
    static uint16_t m_nCurrentOutputState;
    static int buffer;
    static int Convstate;
    static int errors;
    static uint8_t m_CurrentPosition;
    static const uint8_t N_MIN_INPORT_NUM, N_MAX_INPORT_NUM;
    static const uint8_t N_MIN_OUTPORT_NUM, N_MAX_OUTPORT_NUM;
    static int16_t Pos[5];

public:
    static int boxqnt;
    static bool FlagConvState;
    static bool FlagErrors;
    static void initializeBoard(EasyCAT *ptrEasyCat); // DONE code form main

    // direct IOs
    static bool getInputState(int nInputPort);                                  // DONE
    static bool getOutputState(int nOutputPort);                                // DONE
    static void setOutputState(int nOutputPort, bool bSettedValue);             // DONE
    static void setPulse(int nOutputPort, bool bPulsedValue, int nPulseTimeMs); // DONE
    static void setOutputState(uint16_t commonstate);
    static bool isTimerPassed(int &nTimerId, int nTimeoutMs); //
    static void doIOLogic();
    static uint16_t getCommonOutputState();
    static bool ExtSens(uint16_t sensinput);
    static bool ExtSens(uint16_t sensinput, int boardnum);
    static void Error(uint8_t error, bool flag);
    static void Error(int error, int DevNum);
    static int readySignalFromNext();
    static bool readySignalFromNext(int convnum, int boardnum);
    static bool TookProd();
    static void GaveStack(bool gave);
    static void StackReady(bool ready);
    static int ARTTimerGetTime();
    static void DevReady(bool ready);
    static void DevReady(int posnum);
    static void DevReady(int posnum, bool enable);
    static bool ExtDevReady();
    static bool ExtDevReady(int posnum);
    static bool CHK_ACTIVE_NTIME(bool sens_in, int *timer_in, int delta_time);
    static bool ResetDrv(int ResOut);
    static bool PallFull(int PLPPos);
    static bool PLPReady(int PLPPos, int boardnum);
    static int ReqPosition(int Pos);
    static bool PosSetted();
    static void PosSet();
    static void ShuttlePosition(int Position, bool enable);
    static void ConvReady(int Ready);
    static void NeedPal(int posnum, bool enable);
    static void ConvState(int state, int DevNum);
    static int BoxCountSet();
    static void PLPReady(int Position, bool enable);
    static void BoxQnt(int QNT);
    static int SetProdNumberCollect();
    static void ActiveScheme(int Scheme);
    static bool ARTTimer(int timer, int timeSpan, int timeOut);
    static void PallState(int PallState);

protected:
    ArtIOClass();
    ~ArtIOClass();
};

#endif // ART_LOWLEVEL_CLASS_H
