#ifndef Art_PalletMagazine_H
#define Art_PalletMagazine_H
#include "interface\IHasCycleLogic.h"
#include "Actuator\ArtActCylinder.h"
#include "Sensor\ArtSensor.h"
#undef READY
class PalletMagazine : public IHasCycleLogic
{
protected:
    bool isTOPcylinderError,
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
        DISP_STATE,
        isButtonRESET,
        PallONConvey;
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
        DispCurPosition,
        isPallONConvey_timer,
        isPallONConvey_TIME,
        ResButtonInput,
        isAutoMode,
        isBotlleConvServiceMode,
        PallONConveyIn,
        MagazineState,
        isBOTandNOPALLPLAC_timer,
        isBOTandNOPALLPLAC_TIME,
        DEF_TIME_POS_SENS_DOWN,
        timer;

    ArtCylinder *Clamp1;
    ArtCylinder *Clamp2;
    ArtCylinder *TOPCylinder;
    ArtCylinder *BOTCylinder;
    ArtSensor *isPallONConvey;
    ArtSensor *PalletsInStack;

    int DEF_TIME_POS_SENS = 1000;

    enum DispPallSt
    {
        HAS_PALL,
        LOW_PALL,
        NO_PALL
    };

    enum ClampStates
    {
        CL_OPEN,
        CL_CLOSE,
        ERROR_CLAMPS
    };

    enum DispCurPos
    {
        TOP,
        MID,
        BOT,
        ERROR_DISP
    };

    enum DispStates
    {
        READY,
        ERROR_STATE
    };
    enum MagazineStates
    {
        CRITICAL,
        NORMAL,
    };

public:
    void doLogic();
    void update();
    int getName();
    int getID();
    int StartButtonPressed, StopButtonPressed, CylMidRunTimer, CylTopRunTimer, CylBotRunTimer, ErrorNum, CylOpenClampsRunTimer, CylCloseClampsRunTimer,blinkOFF,blinkON;
    bool StopButtonFlag, FlagGoTop, FlagGoMid, FlagGoBot, FlagOpenClamps, FlagCloseClamps,flagBlink;

    PalletMagazine(int id, const char name[]);
    PalletMagazine(int id, const char name[], ArtCylinder *Clamp1, ArtCylinder *Clamp2, ArtCylinder *TOPCylinder,
                   ArtCylinder *BOTCylinder, int ResButtonInput, ArtSensor *PallONConvey, ArtSensor *PalletsInStack, int isAutoMode, int isBotlleConvServiceMode, int StartButtonPressed, int StopButtonPressed);

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
    bool ART_DISP_ERR_SPS();
    bool ART_DISP_BTN_START_SPS();
    bool ART_DISP_BTN_STOP_SPS();
    bool TIMERS_FOR_CYLINDERS();
};

class PalletMagazine_V2 : public IHasCycleLogic
{
protected:
    bool isTOPcylinderError,
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
        DISP_STATE,
        isButtonRESET,
        PallONConvey;

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
        DispCurPosition,
        isPallONConvey_timer,
        isPallONConvey_TIME,
        ResButtonInput,
        isAutoMode,
        isBotlleConvServiceMode,
        PallONConveyIn,
        MagazineState,
        isBOTandNOPALLPLAC_timer,
        isBOTandNOPALLPLAC_TIME,
        DEF_TIME_POS_SENS_DOWN,
        timer;

    ArtCylinder *Clamp1;
    ArtCylinder *Clamp2;
    ArtCylinder *TOPCylinder;
    ArtCylinder *BOTCylinder;
    ArtSensor *isPallONConvey;
    ArtSensor *PalletsInStack;

    int DEF_TIME_POS_SENS = 1000;

    enum DispPallSt
    {
        HAS_PALL,
        LOW_PALL,
        NO_PALL
    };

    enum ClampStates
    {

        CL_OPEN,
        CL_CLOSE,
        ERROR_CLAMPS
    };

    enum DispCurPos
    {
        TOP,
        MID,
        BOT,
        ERROR_POS
    };
    enum DispStates
    {
        DISP_GO,
        DISP_UP,
        DISP_SEARCH_POSITION,
        DISP_READY,
        DISP_ERROR
    };
    enum MagazineStates
    {
        CRITICAL,
        NORMAL,
    };

public:
    void doLogic();
    void update();
    int getName();
    int getID();
    int StartButtonPressed, StopButtonPressed, Disp_Cur_State;
    bool StopButtonFlag;

    PalletMagazine_V2(int id, const char name[]);
    PalletMagazine_V2(int id, const char name[], ArtCylinder *Clamp1, ArtCylinder *Clamp2, ArtCylinder *TOPCylinder,
                      ArtCylinder *BOTCylinder, int ResButtonInput, ArtSensor *PallONConvey, ArtSensor *PalletsInStack, int isAutoMode, int isBotlleConvServiceMode, int StartButtonPressed, int StopButtonPressed);

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
    void ART_DISP_ERR_SPS();
    bool ART_DISP_BTN_START_SPS();
    bool ART_DISP_BTN_STOP_SPS();
};

#endif // Art_PalletMagazine_H