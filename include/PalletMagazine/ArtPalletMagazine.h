#ifndef Art_PalletMagazine_H
#define Art_PalletMagazine_H
#include "interface\IHasCycleLogic.h"

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
        isClamp1Open,
        isClamp2Open;

    int DispPallState,
        ClampState,
        m_disp_pos_error,
        isClamp1Close_timer,
        isClamp1Open_timer;

    int DEF_TIME_POS_SENS=1000;

    enum DispPallSt
    {
        HAS_PALL,
        LOW_PALL,
        NO_PALL
    };
    enum ClampSt
    {
        CL_OPEN,
        ERROR
    };

public:
    void doLogic();
};

#endif //Art_PalletMagazine_H