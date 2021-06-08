#ifndef Art_Act_H
#define Art_Act_H
#include "interface\IHasCycleLogic.h"
#include "ArtIOClass.h"
#include <cmath>
#include "InOut.h"
#include <cstring>
bool static OutState[15];
//uint16_t m_nCurrentOutputState = 0;
class ArtDriver : public IHasCycleLogic
{
protected:
    int m_id,
        nDriverType,
        nDriverState,
        nDriverCurrentSpeed,
        nDriverFWDSpeed[3][4],
        nDriverREVSpeed[3][4],
        number,
        OUT[20];

    bool bisDriverSet,
        bDriverManualMode,
        IN[20];

    char name[256], m_name[256];

public:
int nDriverReadySignalIN;

    bool bDriverDirection,
        bDriverRUN;
    enum DriverState
    {
        ST_DRIVER_UNKNOWN,
        ST_DRIVER_ERROR,
        ST_DRIVER_RUN,
        ST_DRIVER_STOP,
        ST_DRIVER_MANUAL
    };

    enum DriverType
    {
        DRIVER_TYPE_0,
        DRIVER_TYPE_1,
        DRIVER_TYPE_2,
        DRIVER_TYPE_3
    };

    ArtDriver(int id, const char name[]);
    ArtDriver(int id, const char name[], DriverType Driver_Type, int ReadySignalIN, bool Direction, int CurrentSpeed, int FWDSpeed, int value, int inout1, int inout2, int inout3, int inout4);
    void doLogic();
    bool bcheckFWDSpeedOutEnable();
    bool bcheckREVSpeedOutEnable();
    bool bcheckFWDSpeedOutDisable();
    bool bcheckREVSpeedOutDisable();
    void ConveySetSTOP();
    int ConveySetDriverFWD(bool bParametr);
    int ConveySetDriverREV(bool bParametr);
    void ConveySetRun();
    int ARTDriverGetREADY();
    int ARTDriverGetFWD();
    int ARTDriverGetREV();
    int ARTDriverGetSTOP();
    int ARTConveySetRun();
    int ARTDriverSetSTOP();
    void LES();
    int getName();
    int getID();
    void update();
};

#endif