#include "PalletMagazine\ArtPalletMagazine.h"

int PalletMagazine::getName()
{
	return (0);
}

int PalletMagazine::getID()
{
	return (m_id);
}

void PalletMagazine::update()
{
}

PalletMagazine::PalletMagazine(int id, const char name[])
{
    IHasCycleLogicHelper::addDevice(this);
    m_id = id;
}

PalletMagazine::PalletMagazine(int id, const char name[],ArtCylinder *Clamp1,ArtCylinder *Clamp2,ArtCylinder *TOPCylinder,
                                ArtCylinder *BOTCylinder, int ResButtonInput, ArtSensor *PallONConvey, ArtSensor *PalletsInStack, int isAutoMode, int isBotlleConvServiceMode):PalletMagazine(id, name)//(id,name,ptr to clamps cylinder1,cylinder2,ptr to top cylinder,ptr to bot cylinder,reset button in,)
{
    PalletMagazine::Clamp1 = Clamp1;
    PalletMagazine::Clamp2 = Clamp2;
    PalletMagazine::TOPCylinder = TOPCylinder;
    PalletMagazine::BOTCylinder = BOTCylinder;
    PalletMagazine::ResButtonInput = ResButtonInput;
    PalletMagazine::PallONConvey = PallONConvey;
    PalletMagazine::PalletsInStack = PalletsInStack;
    PalletMagazine::isAutoMode = isAutoMode;
    PalletMagazine::isBotlleConvServiceMode = isBotlleConvServiceMode;
    Clamp1->ACGetInitialState();
    Clamp2->ACGetInitialState();
    TOPCylinder->ACGetInitialState();
    BOTCylinder->ACGetInitialState();
}

void PalletMagazine::doLogic()
{
    CLAMP_POS_STATE_SPS();
    DISP_POS_STATE_SPS();
    ART_DISP_BTN_SPS();
}

void PalletMagazine::CLAMP_POS_STATE_SPS()
{
    if ((!ArtIOClass::getInputState(isAutoMode)) && (!ArtIOClass::getInputState(isBotlleConvServiceMode)))
    {
        if (Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)
        {
            if (ArtCylinder::CHK_ACTIVE_NTIME(Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED, isClamp1Open_timer, DEF_TIME_POS_SENS))
            {
                if (Clamp2->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)
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
                if (ArtCylinder::CHK_ACTIVE_NTIME(Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED, isClamp1Open_timer, DEF_TIME_POS_SENS))
                {
                    //;reset timer
                }
            }
            isClamp1Close_timer = 0;
        }

        if (Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_OPEN)
        {
            if (ArtCylinder::CHK_ACTIVE_NTIME(Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_OPEN, isClamp1Close_timer, DEF_TIME_POS_SENS))
            {
                if (Clamp2->getCylState() == ArtCylinder::ARTCYL_ST_OPEN)
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
                if (ArtCylinder::CHK_ACTIVE_NTIME(Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_OPEN, isClamp1Close_timer, DEF_TIME_POS_SENS))
                {
                    //;reset timer
                }
            }
            isClamp1Open_timer = 0;
        }
    }
    else
    {
        doTOPCylinderUP = false;
        doTOPCylinderDOWN = false;
        doBOTCylinderUP = false;
        doBOTCylinderDOWN = false;
        doOpenClamps = false;

        if (DispCur == TOP)
        {
            if (PalletsInStack->SensorState())
            {
                DispPallState = HAS_PALL;
            }

            if (ArtIOClass::getInputState(PallONConveyIn) && !PalletsInStack->SensorState())
            {
                DispPallState = LOW_PALL;
            }

            if (!PalletsInStack->SensorState() && !ArtIOClass::getInputState(PallONConveyIn))
            {
                DispPallState = NO_PALL;
            }
        }
    }
}
void PalletMagazine::DISP_POS_STATE_SPS()
{
    isBOTCylinderUP = BOTCylinder->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED;
    isTOPCylinderUP = BOTCylinder->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED;
    isBOTCylinderDOWN = BOTCylinder->getCylState() == ArtCylinder::ARTCYL_ST_OPEN;
    isTOPCylinderDOWN = BOTCylinder->getCylState() == ArtCylinder::ARTCYL_ST_OPEN;

    if (isTOPCylinderUP)
    {
        if (ArtCylinder::CHK_ACTIVE_NTIME(isTOPCylinderUP, isTOPCylinderUP_timer, DEF_TIME_POS_SENS))
        {
            if (isBOTCylinderUP)
            {
                DispCur = TOP;
            }

            if (isBOTCylinderDOWN)
            {
                DispCur = ERROR;
                //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION") ;
                //artOpcMessage[200]=1      ;
            }

            if (!isBOTCylinderUP & !isBOTCylinderDOWN)
            {
                DispCur = ERROR;
                //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION")
                //artOpcMessage[200]=1
            }
        }
    }
    else
    {
        if (ArtCylinder::CHK_ACTIVE_NTIME(isBOTCylinderUP, isTOPCylinderUP_timer, DEF_TIME_POS_SENS))
        {
            // ;reset timer;
        }
    }

    if (isTOPCylinderDOWN)
    {
        if (ArtCylinder::CHK_ACTIVE_NTIME(isTOPCylinderDOWN, isTOPCylinderDOWN_timer, DEF_TIME_POS_SENS))
        {
            if (isBOTCylinderUP)
            {
                DispCur = MID;
            }

            if (isBOTCylinderDOWN)
            {
                DispCur = BOT;
            }

            if (!isBOTCylinderUP && !isBOTCylinderDOWN)
            {
                DispCur = ERROR;
                //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION")
                //artOpcMessage[200]=1}
            }
        }
    }
    else
    {
        if (ArtCylinder::CHK_ACTIVE_NTIME(isTOPCylinderDOWN, isTOPCylinderDOWN_timer, DEF_TIME_POS_SENS))
        {
            //;reset timer
        }
    }

    isTOPcylinderError = (!isTOPCylinderUP && !isTOPCylinderDOWN);

    if (ArtCylinder::CHK_ACTIVE_NTIME(isTOPcylinderError, isTOPcylinderError_timer, DEF_TIME_POS_SENS))
    {
        //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION")
        //artOpcMessage[200]=1
    }
}

void PalletMagazine::DISP_MAIN_CYCLE_SPS()
{
    bool isBOT, isTOP, isMID;
    if (DispCur == BOT)
    {
        isBOT = true;
    }
    else
    {
        isBOT = false;
        if (DispCur == MID)
        {
            isMID = true;
        }
        else
            isMID = false;
        if (DispCur == TOP)
        {
            isTOP = true;
        }
        else
        {
            isTOP = false;
        }
    }

    /*if ((!PallONConvey->SensorState()) && (1 /*doRunConv4*//*))
    /*{
        isPallONConvey_flag = true;
    }
    else
    {
        isPallONConvey_flag = false;
    }

    if (PallONConvey)
    {
        PallONConvey = true;
    }

    if (ArtCylinder::CHK_ACTIVE_NTIME(isPallONConvey_flag, isPallONConvey_timer, isPallONConvey_TIME))
    {
        PallONConvey = false;
    }*/

    if ((!ArtIOClass::getInputState(isAutoMode)) && (!ArtIOClass::getInputState(isBotlleConvServiceMode)))
    {
        if (DISP_STATE == ready)
        {
            switch (DispCur)
            {

            case BOT:
            {
                if (ArtCylinder::CHK_ACTIVE_NTIME(isBOT, isBOT_timer, DEF_TIME_POS_SENS))
                {
                    if (PalletsInStack->SensorState())
                    {
                        DispPallState = HAS_PALL;
                    }

                    if ((PallONConvey->SensorState()) && !PalletsInStack->SensorState())
                    {
                        DispPallState = LOW_PALL;
                    }

                    if (!PalletsInStack->SensorState())
                    {
                        DispPallState = NO_PALL;
                    }

                    if (ClampState == CL_OPEN)
                    {
                        DISPGOMID();
                    }
                    else
                    {
                        doOPENCLAMPS();
                    }
                }
            }

            case MID:
            {
                if (ArtCylinder::CHK_ACTIVE_NTIME(isMID, isMID_timer, DEF_TIME_POS_SENS))
                {
                    if (ClampState == CL_OPEN)
                    {
                        if ((DispPallState == HAS_PALL) || (DispPallState == LOW_PALL))
                        {
                            doCLOSECLAMPS();
                        }

                        if (DispPallState == NO_PALL)
                        {
                            if (ClampState == CL_OPEN)
                            {
                                doCLOSECLAMPS();
                            }
                            else
                            {
                                DISPGOTOP();
                            }
                        }
                    }
                    else
                    {
                        if ((DispPallState == HAS_PALL) || (DispPallState == LOW_PALL))
                        {
                            if (PallONConvey || (/*PALL_conveyors[4].state == CONV_RUN*/ 1))
                            {
                                DISPGOTOP();
                            }
                            else
                            {
                                if (!PallONConvey->SensorState())
                                {
                                    DISPGOBOT();
                                }
                            }
                        }
                    }
                }
            }

            case TOP:
                if (PallONConvey->SensorState() || (/*PALL_conveyors[4].state == CONV_RUN*/ 1))
                {
                }
                else
                {
                    if (!PallONConvey->SensorState())
                    {
                        if ((DispPallState == HAS_PALL) || (DispPallState == LOW_PALL))
                        {
                            if (!PallONConvey->SensorState())
                            {
                                DISPGOBOT();
                            }
                        }
                    }
                }
            }
        }
    }
}

void PalletMagazine::ART_DISP_BTN_SPS()
{
    if( ArtIOClass::getInputState(ResButtonInput) && (DispPallState == NO_PALL)) 
    {
        DispPallState = HAS_PALL;      
    }
   
    if (ArtIOClass::getInputState(ResButtonInput)) 
    {
        DISP_STATE=READY;
        //PallONConvey = false;
        isMID_timer = 0;
        isBOT_timer = 0;
    }
}

void PalletMagazine::DISPGOMID()
{
    TOPCylinder->ARTCylinderOpen();
    BOTCylinder->ARTCylinderClose();
    doTOPCylinderUP = false;
    doTOPCylinderDOWN = true;

    doBOTCylinderDOWN = false;
    doBOTCylinderUP = true;
}

void PalletMagazine::DISPGOTOP()
{
    TOPCylinder->ARTCylinderClose();
    BOTCylinder->ARTCylinderClose();
    doTOPCylinderDOWN = false;
    doTOPCylinderUP = true;

    doBOTCylinderDOWN = false;
    doBOTCylinderUP = true;
}

void PalletMagazine::DISPGOBOT()
{
    TOPCylinder->ARTCylinderOpen();
    BOTCylinder->ARTCylinderOpen();

    doTOPCylinderUP = false;
    doTOPCylinderDOWN = true;
    doBOTCylinderUP = false;
    doBOTCylinderDOWN = true;
}

void PalletMagazine::DISPGOSTOP()
{
    doTOPCylinderUP_start = doTOPCylinderUP;
    doTOPCylinderDOWN_start = doTOPCylinderDOWN;
    doBOTCylinderUP_start = doBOTCylinderUP;
    doBOTCylinderDOWN_start = doBOTCylinderDOWN;

    doTOPCylinderUP = false;
    doTOPCylinderDOWN = false;

    doBOTCylinderUP = false;
    doBOTCylinderDOWN = false;
    disp_run_bool = true;
}

void PalletMagazine::DISPGORUN()
{
    doTOPCylinderUP = doTOPCylinderUP_start;
    doTOPCylinderDOWN = doTOPCylinderDOWN_start;
    doBOTCylinderUP = doBOTCylinderUP_start;
    doBOTCylinderDOWN = doBOTCylinderDOWN_start;
}

void PalletMagazine::doOPENCLAMPS()
{
    doOpenClamps = true;
    Clamp1->ARTCylinderClose();
}

void PalletMagazine::doCLOSECLAMPS()
{
    doOpenClamps = false;
    Clamp1->ARTCylinderOpen();
}

