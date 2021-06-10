#ifndef Art_Act_Cylinder_H
#define Art_Act_Cylinder_H
#include "interface\IHasCycleLogic.h"
#include "ArtIOClass.h"
#include "Conveyor\ArtConveyor.h"
//ArtBasicConveyor BasicConveyor;
class ArtCylinder : public IHasCycleLogic
{
protected:
    int m_id,
        cylCloseTimer,
        cylOpenTimer,
        cylState,
        cylOpenDelay,
        cylCloseDelay,
        cylOpenOut,
        cylCloseOut,
        distrType;
        

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

      enum distType
    {
        MONO_STABLE,
        BI_STABLE
    };

    ArtCylinder(int id, const char name[]);
    ArtCylinder(int id, const char name[], int CloseTime, int OpenTime, bool TimeoutControl, bool CylinderSet, distType type, int cylOpenOut, int cylCloseOut);
   // ArtCylinder(int id, const char name[], int CloseTime , int OpenTime,bool TimeoutControl,bool CylinderSet,distType distrType,int OpenOut,int CloseOut);
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
    bool static CHK_ACTIVE_NTIME(bool sens_in, int timer_in, int delta_time);
    int ACGetInitialState();
    void ARTCylinderOpen();
    void ARTCylinderClose();
    int getCylState();
};
#endif