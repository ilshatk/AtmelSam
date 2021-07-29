#ifndef Art_Act_Cylinder_H
#define Art_Act_Cylinder_H
#include "interface\IHasCycleLogic.h"
#include "ArtIOClass.h"
#include "Sensor\ArtSensor.h"

class ArtCylinder : public IHasCycleLogic
{
private:
    int m_id,
        cylCloseTimer,
        cylOpenTimer,
        cylState,
        cylOpenDelay,
        cylCloseDelay,
        cylOpenOut,
        cylCloseOut,
        distrType,
        ActedOut;

    bool bCylTimeoutControl,
        isCylinderSet,
        cylCloseValue,
        isCylinderActed,
        isCylinderOpened,
        isCylinderClosed;

    char name[256], m_name[256];

    ArtSensor *cylOpenIn;
    ArtSensor *cylCloseIn;

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
    ArtCylinder(int id, const char name[], int CloseTime, int OpenTime, bool TimeoutControl,
                bool CylinderSet, distType type, int cylOpenOut, int cylCloseOut,
                ArtSensor *cylOpenIn, ArtSensor *cylCloseIn);
    ArtCylinder(int id, const char name[], int OpenTime, bool TimeoutControl,
                bool CylinderSet, distType type, int cylOpenOut,
                ArtSensor *cylOpenIn, ArtSensor *cylCloseIn, int cylCloseDelay, int cylOpenDelay);
    void doLogic();
    void update();
    int getName();
    int getID();
    int ACGetInitialState();
    void ARTCylinderOpen();
    void ARTCylinderClose();
    int getCylState();
    void ARTCylinderOFF();
};

#endif