#include "PalletMagazine\ArtPalletMagazine.h"

void PalletMagazine::doLogic()
{
    if ((!isAutoMode)&&(!isBotlleConvServiceMode))
        {
            if (isClamp1Open)
            {
                if (CHK_ACTIVE_NTIME(isClamp1Open, isClamp1Open_timer, DEF_TIME_POS_SENS))
                {
                    if (isClamp2Open)
                    {
                        ClampState = CL_OPEN;
                    }
                    else
                    {
                        ClampState = ERROR;
                        //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "CLAMPS_OPEN_ERROR")
                        //artOpcMessage[201]=1
                    }
                }
                else
                {
                    if (CHK_ACTIVE_NTIME(isClamp1Open, isClamp1Open_timer, DEF_TIME_POS_SENS))
                    {
                        //;reset timer
                    }
                }
                isClamp1Close_timer = 0;
            }

            if (isClamp1Close)
            {
                if (CHK_ACTIVE_NTIME(isClamp1Close, isClamp1Close_timer, DEF_TIME_POS_SENS))
                {
                    if (isClamp2Close)
                    {
                        ClampState = CL_CLOSE;
                    }
                    else
                    {
                        ClampState = ERROR;
                        //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "CLAMPS_OPEN_ERROR")
                        //artOpcMessage[201]=1
                    }
                }
                else
                {
                    if (CHK_ACTIVE_NTIME(isClamp1Close, isClamp1Close_timer, DEF_TIME_POS_SENS))
                    {
                        //;reset timer
                    }
                }
                isClamp1Open_timer = 0;
            }
        }
    else
    {
        //; OFF signals for Rustem
        doTOPCylinderUP = false;
        doTOPCylinderDOWN = false;
        doBOTCylinderUP = false;
        doBOTCylinderDOWN = false;
        doOpenClamps = false;

        if (DispCurPos == TOP)
        {
            if (isPalletsInStack)
            {
                DispPallState = HAS_PALL;
            }

            if (isPalletOnConvey && !isPalletsInStack)
            {
                DispPallState = LOW_PALL;
            }

            if (!isPalletsInStack && !isPalletOnConvey)
            {
                DispPallState = NO_PALL;
            }
        }
    }
}