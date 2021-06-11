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
        isBotlleConvServiceMode,
        isPallONConvey,
        DISP_STATE,
        isButtonRESET;
    /*isClamp1Open,
        isClamp2Open;*/

    int m_id,
        DispPallState,
        ClampState,
        m_disp_pos_error,
        isClamp1Close_timer,
        isClamp1Open_timer,
        isTOPCylinderUP_timer,
        isTOPCylinderDOWN_timer,
        isTOPcylinderError_timer,
        isBOT_timer,
        isMID_timer,
        DispCur,
        isPallONConvey_timer,
        isPallONConvey_TIME,
        ResButtonInput;

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
    enum DisStates
    {
        ready
    };

public:
    PalletMagazine(int id, const char name[]);
    PalletMagazine(int id, const char name[], ArtCylinder *clamp1, ArtCylinder *clamp2, ArtCylinder *TOPCylinder,
                    ArtCylinder *BOTCylinder, int ResButtonInput);
    void doLogic();
    void DISP_POS_STATE_SPS();
    void CLAMP_POS_STATE_SPS();
    void DISP_MAIN_CYCLE_SPS();
    void DISPGOMID();
    void DISPGOTOP();
    void DISPGOBOT();
    void DISPGOSTOP();
    void DISPGORUN();
    void doOPENCLAMPS();
    void doCLOSECLAMPS();
    void ART_DISP_BTN_SPS();

};

#endif //Art_PalletMagazine_H