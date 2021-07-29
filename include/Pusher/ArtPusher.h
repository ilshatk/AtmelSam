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
    int m_id, PusherState;
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
    ArtPusher(int id, const char name[], ArtSensor *OnPusherPtr, ArtBasicConveyor *ConvPointPtr, ArtBasicConveyor *NextConvPointPtr, ArtCylinder *PusherCylPtr);
    void doLogic();
    void update();
    int getName();
    int getID();
    int getCylState();
};

class ArtPodzhim : public IHasCycleLogic
{
protected:
    int m_id, PodzhimState;
    char name[256], m_name[256];
    ArtCylinder *PodzhimCylPtr;
    ArtSensor *OnPodzhimPtr;

    enum PodzhimStates
    {
        PUSHED,
        HOME
    };

public:
    ArtPodzhim(int id, const char name[]);
    ArtPodzhim(int id, const char name[], ArtSensor *OnPodzhimPtr, ArtCylinder *PodzhimCylPtr);
    void doLogic();
    void update();
    int getName();
    int getID();
    int getCylState();
};


class ArtStopper : public IHasCycleLogic
{
protected:
    int m_id, StopperState;
    char name[256], m_name[256];
    ArtCylinder *StopperCylPtr;
    ArtSensor *OnStopperPtr;

    enum StopperStates
    {
        STOPED,
        HOME
    };

public:
    ArtStopper(int id, const char name[]);
    ArtStopper(int id, const char name[], ArtSensor *OnStopperPtr, ArtCylinder *StopperCylPtr);
    void doLogic();
    void update();
    int getName();
    int getID();
    int getCylState();
};

class ArtLift : public IHasCycleLogic
{
protected:
    int m_id, LiftState,posnum;
    char name[256], m_name[256];
    ArtCylinder *LiftCylPtr;

    enum LiftStates
    {
        UP,
        DOWN
    };

public:
    ArtLift(int id, const char name[]);
    ArtLift(int id, const char name[], ArtCylinder *LiftCylPtr);
    void doLogic();
    void update();
    int getName();
    int getID();
    int getCylState();
};

#endif