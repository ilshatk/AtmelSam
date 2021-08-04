#ifndef ART_PLP
#define ART_PLP
#include "Actuator\ArtActDriver.h"
#include "Actuator\ArtActCylinder.h"
#include "Sensor\ArtSensor.h"
#include "interface\IHasCycleLogic.h"

class ArtPLP : public IHasCycleLogic
{
private:
    int PLPState;

public:
    enum PLPStates
    {
        PLP_ST_UNKNOWN,
        PLP_ST_NEEDPALLET,
        PLP_ST_PALLET_PLACED,
        PLP_ST_PALLET_MOVE_IN,
        PLP_ST_PALLET_UNFIXED,
        PLP_ST_PALLET_FIXED,
        PLP_ST_PALLET_READY,
        PLP_ST_PALLET_BUSY,
        PLP_ST_PALLET_FULL,
        PLP_ST_PALLET_MOVE_OUT,
        PLP_ST_CLEAR_CHECK,
        ST_ERROR

    };
    ArtSensor *PallOnPlp;
    ArtPLP(int id, const char name[], ArtSensor *PallOnPlp,ArtSensor *MoveIn,);
    int getName();
    int getID();
    void update();
    void doLogic();
};

#endif