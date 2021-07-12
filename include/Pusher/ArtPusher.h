#ifndef Art_Act_Pusher
#define Art_Act_Pusher
#include "interface\IHasCycleLogic.h"
#include "ArtIOClass.h"
#include "Conveyor\ArtConveyor.h"
#include "Actuator\ArtActCylinder.h"
#include "Sensor\ArtSensor.h"

class ArtPusher : public IHasCycleLogic
{
protected:
    int m_id,PusherState;
    char name[256], m_name[256];
    ArtCylinder *PusherCylPtr;
    ArtSensor *OnPusherPtr;
    ArtBasicConveyor *NextConvPointPtr;
    ArtBasicConveyor *ConvPointPtr;

    enum PusherStates
    {
        PUSHED,
        HOME
    };
public:
    ArtPusher(int id, const char name[]);
    ArtPusher(int id, const char name[],ArtSensor *OnPusherPtr , ArtBasicConveyor *ConvPointPtr, ArtBasicConveyor *NextConvPointPtr, ArtCylinder *PusherCylPtr);
    void doLogic();
    void update();
    int getName();
    int getID();
    int getCylState();
};

#endif