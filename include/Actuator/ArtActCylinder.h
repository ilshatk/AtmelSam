#ifndef Art_Act_Cylinder_H
#define Art_Act_Cylinder_H
#include "interface\IHasCycleLogic.h"
#include "ArtIOClass.h"
#include "Conveyor\ArtConveyor.h"
class ArtCylinder : public IHasCycleLogic
{
protected:
    int m_id,
        cylCloseTimer,
        CylOpenTimer,
        cylState;

    bool bCylTimeoutControl,
         isCylinderSet,
         cylCloseValue;

    char name[256], m_name[256];

public:
    enum cylinderState
    {
        ARTCYL_ST_OPEN,
        ARTCYL_ST_CLOSED,
        ARTCYL_ST_MOVING,
        ARTCYL_ST_ERROR,
        ARTCYL_ST_UNKNOWN
    };

    //ArtCylinder(int id, const char name[]);
    //ArtCylinder(int id, const char name[], DriverType Driver_Type, int ReadySignalIN, bool Direction, int CurrentSpeed, int FWDSpeed, int value, int inout1, int inout2, int inout3, int inout4);
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