#ifndef Art_PalletMagazine_H
#define Art_PalletMagazine_H
#include "interface\IHasCycleLogic.h"
#include "Actuator\ArtActCylinder.h"

class PalletMagazine : public IHasCycleLogic
{
protected:
    bool isTOPcylinderError,
        PallONConvey,
        isPallONConvey_flag,
        disp_run_bool,
        doTOPCylinderUP_start,
        doTOPCylinderDOWN_start,
        doBOTCylinderUP_start,
        doBOTCylinderDOWN_start,
        isBOTCylinderUP,
        isTOPCylinderUP,
        isBOTCylinderDOWN,
        isTOPCylinderDOWN,
        doTOPCylinderUP,
        doTOPCylinderDOWN,
        doBOTCylinderUP,
        doBOTCylinderDOWN,
        doOpenClamps,
        isPalletsInStack,
        isPalletOnConvey,
        isAutoMode,
        isBotlleConvServiceMode;
    /*isClamp1Open,
        isClamp2Open;*/

    int DispPallState,
        ClampState,
        m_disp_pos_error,
        isClamp1Close_timer,
        isClamp1Open_timer,
        isTOPCylinderUP_timer,
        isTOPCylinderDOWN_timer,
        isTOPcylinderError_timer,
        DispCur;
    ArtCylinder *Clamp1;
    ArtCylinder *Clamp2;
    ArtCylinder *TOPCylinder;
    ArtCylinder *BOTCylinder;
    int DEF_TIME_POS_SENS = 1000;

    enum DispPallSt
    {
        HAS_PALL,
        LOW_PALL,
        NO_PALL
    };

    enum ClampSt
    {
        CL_OPEN,
        CL_CLOSE,
        ERROR
    };

    enum DispCurPos
    {
        TOP,
        MID,
        BOT
    };

public:
    void doLogic();
    void DISP_POS_STATE_SPS();
    void CLAMP_POS_STATE_SPS();
    void DISP_MAIN_CYCLE_SPS();
};

#endif //Art_PalletMagazine_H