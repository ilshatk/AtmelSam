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

PalletMagazine::PalletMagazine(int id, const char name[], ArtCylinder *Clamp1, ArtCylinder *Clamp2, ArtCylinder *TOPCylinder,
                               ArtCylinder *BOTCylinder, int ResButtonInput, ArtSensor *PallONConvey, ArtSensor *PalletsInStack,
                               int isAutoMode, int isBotlleConvServiceMode, int StartButtonPressed, int StopButtonPressed) : PalletMagazine(id, name)
{
    PalletMagazine::Clamp1 = Clamp1;
    PalletMagazine::Clamp2 = Clamp2;
    PalletMagazine::TOPCylinder = TOPCylinder;
    PalletMagazine::BOTCylinder = BOTCylinder;
    PalletMagazine::ResButtonInput = ResButtonInput;
    PalletMagazine::isPallONConvey = PallONConvey;
    PalletMagazine::PalletsInStack = PalletsInStack;
    PalletMagazine::isAutoMode = isAutoMode;
    PalletMagazine::isBotlleConvServiceMode = isBotlleConvServiceMode;
    PalletMagazine::DISP_STATE = READY;
    PalletMagazine::isClamp1Open_timer = 0;
    PalletMagazine::isClamp1Close_timer = 0;
    PalletMagazine::isTOPCylinderUP_timer = 0;
    PalletMagazine::isTOPCylinderDOWN_timer = 0;
    PalletMagazine::DEF_TIME_POS_SENS = 3000; // 1000
    PalletMagazine::DEF_TIME_POS_SENS_DOWN = 3000;
    PalletMagazine::isBOT_timer = 0;
    PalletMagazine::isMID_timer = 0;
    PalletMagazine::isBOTandNOPALLPLAC_TIME = 88260161;
    PalletMagazine::isBOTandNOPALLPLAC_timer = 0;
    PalletMagazine::isTOPcylinderError_timer = 0;
    PalletMagazine::isPallONConvey_timer = 0;
    PalletMagazine::StartButtonPressed = StartButtonPressed;
    PalletMagazine::StopButtonPressed = StopButtonPressed;
    Clamp1->ACGetInitialState();
    Clamp2->ACGetInitialState();
    TOPCylinder->ACGetInitialState();
    BOTCylinder->ACGetInitialState();
    timer = 0;
    PalletMagazine::StopButtonFlag = false;
    PalletMagazine::ErrorNum = 0;
    PalletMagazine::FlagOpenClamps = false;
    PalletMagazine::FlagGoMid = false;
    PalletMagazine::FlagGoBot = false;
    PalletMagazine::FlagGoTop = false;
    PalletMagazine::flagBlink = false;
    blinkON = ArtIOClass::ARTTimerGetTime();
}

void PalletMagazine::doLogic()
{
    ArtIOClass::ConvState(DISP_STATE,1);
    DISP_POS_STATE_SPS();   // Главные цилиндры
    CLAMP_POS_STATE_SPS();  // Захват диспенсера
    TIMERS_FOR_CYLINDERS(); // Захват диспенсера

    if (ART_DISP_ERR_SPS())
    {
        return;
    }

    if (ART_DISP_BTN_STOP_SPS())
    {
        return;
    }

    if (ART_DISP_BTN_START_SPS())
    {
        return;
    }
    DISP_MAIN_CYCLE_SPS(); // Основной цикловик диспенсера
    ART_DISP_BTN_SPS();    // SPS-ка кнопки сброса
                           //
}

void PalletMagazine::CLAMP_POS_STATE_SPS()
{
    if ((!ArtIOClass::getInputState(isAutoMode)) /*&& (!ArtIOClass::getInputState(isBotlleConvServiceMode))*/) // Проверка включен ли ручной режим
    {
        if (!Clamp1->getCylState()) // Состояние цилиндра
        {
            if (ArtIOClass::CHK_ACTIVE_NTIME(!Clamp1->getCylState(), &isClamp1Open_timer, DEF_TIME_POS_SENS)) //Проверка отсчитал ли таймер isClamp1Open_timer до DEF_TIME_POS_SENS
            {
                if (!Clamp2->getCylState())
                {
                    ClampState = CL_CLOSE;
                }
                else
                {
                    ClampState = ERROR_CLAMPS;
                }
            }
            else
            {
                if (ArtIOClass::CHK_ACTIVE_NTIME(!Clamp1->getCylState(), &isClamp1Open_timer, DEF_TIME_POS_SENS))
                {
                    //;reset timer
                }
            }
            isClamp1Close_timer = 0;
        }

        if (Clamp1->getCylState())
        {
            if (ArtIOClass::CHK_ACTIVE_NTIME(Clamp1->getCylState(), &isClamp1Close_timer, DEF_TIME_POS_SENS))
            {
                if (Clamp2->getCylState())
                {
                    ClampState = ClampStates::CL_OPEN;
                }
                else
                {
                    ClampState = ERROR_CLAMPS;
                }
            }
            else
            {
                if (ArtIOClass::CHK_ACTIVE_NTIME(Clamp1->getCylState(), &isClamp1Close_timer, DEF_TIME_POS_SENS))
                {
                    // reset timer
                }
            }
            isClamp1Open_timer = 0;
        }
    }
    else
    {
        Clamp1->ARTCylinderOFF();
        BOTCylinder->ARTCylinderOFF();
        TOPCylinder->ARTCylinderOFF();

        if (DispCurPosition == TOP)
        {
            if (PalletsInStack->SensorState())
            {
                DispPallState = DispPallSt::HAS_PALL;
            }

            if (isPallONConvey->SensorState() && !PalletsInStack->SensorState())
            {
                DispPallState = DispPallSt::LOW_PALL;
            }

            if (!PalletsInStack->SensorState() && !isPallONConvey->SensorState())
            {
                DispPallState = DispPallSt::NO_PALL;
            }
        }
    }
}

void PalletMagazine::DISP_POS_STATE_SPS()
{
    isTOPCylinderUP = !TOPCylinder->getCylState();  // верхний цилиндр вверху
    isTOPCylinderDOWN = TOPCylinder->getCylState(); // верхний цилиндр внизу
    isBOTCylinderUP = !BOTCylinder->getCylState();  // нижний цилиндр вверху
    isBOTCylinderDOWN = BOTCylinder->getCylState(); // нижний цилиндр внизу

    if (isTOPCylinderUP)
    {
        if (ArtIOClass::CHK_ACTIVE_NTIME(isTOPCylinderUP, &isTOPCylinderUP_timer, DEF_TIME_POS_SENS))
        {
            if (isBOTCylinderUP)
            {
                DispCurPosition = TOP;
            }

            if (isBOTCylinderDOWN)
            {
                DispCurPosition = ERROR_DISP;
            }

            if (!isBOTCylinderUP & !isBOTCylinderDOWN)
            {
                DispCurPosition = ERROR_DISP;
            }
        }
    }
    else
    {
        if (ArtIOClass::CHK_ACTIVE_NTIME(isBOTCylinderUP, &isTOPCylinderUP_timer, DEF_TIME_POS_SENS))
        {
            // ;reset timer;
        }
    }

    if (isTOPCylinderDOWN)
    {
        if (ArtIOClass::CHK_ACTIVE_NTIME(isTOPCylinderDOWN, &isTOPCylinderDOWN_timer, DEF_TIME_POS_SENS))
        {
            if (isBOTCylinderUP)
            {
                DispCurPosition = MID;
            }

            if (isBOTCylinderDOWN)
            {
                DispCurPosition = BOT;
            }

            if (!isBOTCylinderUP && !isBOTCylinderDOWN)
            {
                DispCurPosition = ERROR_DISP;
                // ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION")
                // artOpcMessage[200]=1}
            }
        }
    }
    else
    {
        if (ArtIOClass::CHK_ACTIVE_NTIME(isTOPCylinderDOWN, &isTOPCylinderDOWN_timer, DEF_TIME_POS_SENS))
        {
            //;reset timer
        }
    }

    isTOPcylinderError = (!isTOPCylinderUP && !isTOPCylinderDOWN);

    if (ArtIOClass::CHK_ACTIVE_NTIME(isTOPcylinderError, &isTOPcylinderError_timer, DEF_TIME_POS_SENS))
    {
        // ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION")
        // artOpcMessage[200]=1
    }
}

void PalletMagazine::DISP_MAIN_CYCLE_SPS()
{
    bool isBOT, isTOP, isMID;

    if (DispCurPosition == BOT)
    {
        isBOT = true;
    }
    else
    {
        isBOT = false;
        if (DispCurPosition == MID)
        {
            isMID = true;
        }
        else
        {
            isMID = false;
            if (DispCurPosition == TOP)
            {
                isTOP = true;
            }
            else
            {
                isTOP = false;
            }
        }
    }

    if (!isPallONConvey->SensorState())
    {
        isPallONConvey_flag = true;
    }
    else
    {
        isPallONConvey_flag = false;
    }

    if (isPallONConvey->SensorState())
    {
        PallONConvey = true;
    }

    if (ArtIOClass::CHK_ACTIVE_NTIME(isPallONConvey_flag, &isPallONConvey_timer, isPallONConvey_TIME))
    {
        PallONConvey = false;
    }

    if ((!ArtIOClass::getInputState(isAutoMode)) /*&& (!ArtIOClass::getInputState(isBotlleConvServiceMode))*/)
    {
        ArtIOClass::DevReady(false);
        if (DISP_STATE == DispStates::READY)
        {
            switch (DispCurPosition)
            {
            case BOT:
            {
                FlagGoBot = false;
                ArtIOClass::DevReady(false);
                if (ArtIOClass::CHK_ACTIVE_NTIME(isBOT, &isBOT_timer, DEF_TIME_POS_SENS_DOWN))
                {
                    if (PalletsInStack->SensorState())
                    {
                        DispPallState = DispPallSt::HAS_PALL;
                    }

                    if ((isPallONConvey->SensorState()) && !PalletsInStack->SensorState())
                    {
                        DispPallState = DispPallSt::LOW_PALL;
                    }

                    if (!PalletsInStack->SensorState() && (!isPallONConvey->SensorState()))
                    {
                        DispPallState = DispPallSt::NO_PALL;
                    }

                    if (ClampState == ClampStates::CL_OPEN)
                    {
                        FlagOpenClamps = false;
                    }

                    if (ClampState == ClampStates::CL_OPEN && (DispPallState == DispPallSt::HAS_PALL || DispPallSt::LOW_PALL) && isPallONConvey->SensorState())
                    {
                        FlagOpenClamps = false;
                        DISPGOMID();
                        CylMidRunTimer = ArtIOClass::ARTTimerGetTime();
                        FlagGoMid = true;
                    }
                    else
                    {
                        if (isPallONConvey->SensorState())
                        {
                            doOPENCLAMPS();
                            CylOpenClampsRunTimer = ArtIOClass::ARTTimerGetTime();
                            FlagOpenClamps = true;
                        }
                    }
                }
                break;
            }

            case MID:
            {
                FlagGoMid = false;
                ArtIOClass::DevReady(false);
                if (ArtIOClass::CHK_ACTIVE_NTIME(isMID, &isMID_timer, DEF_TIME_POS_SENS))
                {
                    if (ClampState == CL_OPEN)
                    {
                        if ((DispPallState == DispPallSt::HAS_PALL) || (DispPallState == DispPallSt::LOW_PALL))
                        {
                            doCLOSECLAMPS();
                            FlagCloseClamps = true;
                            CylCloseClampsRunTimer = ArtIOClass::ARTTimerGetTime();
                        }

                        if (DispPallState == DispPallSt::NO_PALL)
                        {
                            if (ClampState == ClampStates::CL_OPEN)
                            {
                                doCLOSECLAMPS();
                                FlagCloseClamps = true;
                                CylCloseClampsRunTimer = ArtIOClass::ARTTimerGetTime();
                            }
                            else
                            {
                                FlagCloseClamps = false;
                                DISPGOTOP();
                                FlagGoTop = true;
                                CylTopRunTimer = ArtIOClass::ARTTimerGetTime();
                            }
                        }
                    }
                    else
                    {
                        FlagCloseClamps = false;
                        if ((DispPallState == DispPallSt::HAS_PALL) || (DispPallState == DispPallSt::LOW_PALL))
                        {
                            if (isPallONConvey->SensorState() /*&& (/*PALL_conveyors[4].state == CONV_RUN 1)*/)
                            {
                                DISPGOTOP();
                                FlagGoTop = true;
                                CylTopRunTimer = ArtIOClass::ARTTimerGetTime();
                            }
                        }
                        else
                        {
                            if (!isPallONConvey->SensorState())
                            {
                                DISPGOBOT();
                                FlagGoBot = true;
                                CylBotRunTimer = ArtIOClass::ARTTimerGetTime();
                            }
                        }
                    }
                }
                break;
            }

            case TOP:
            {
                FlagGoTop = false;
                if (PalletsInStack->SensorState())
                {
                    DispPallState = DispPallSt::HAS_PALL;
                }

                if (isPallONConvey->SensorState() && !PalletsInStack->SensorState())
                {
                    DispPallState = DispPallSt::LOW_PALL;
                }

                if (!PalletsInStack->SensorState() && !isPallONConvey->SensorState())
                {
                    DispPallState = DispPallSt::NO_PALL;
                }

                if (isPallONConvey->SensorState() && true /*(PALL_conveyors[4].state == CONV_RUN1)*/)
                {
                    ArtIOClass::DevReady(true);
                }
                else
                {
                    ArtIOClass::DevReady(false);

                    if (!isPallONConvey->SensorState())
                    {

                        if (!isPallONConvey->SensorState())
                        {
                            DISPGOBOT();
                            FlagGoBot = true;
                            CylBotRunTimer = ArtIOClass::ARTTimerGetTime();
                        }
                    }
                }
                break;
            }
            default:
                break;
            }
        }
    }
}

bool PalletMagazine::TIMERS_FOR_CYLINDERS()
{
    if (FlagGoTop)
    {
        if (ArtIOClass::ARTTimer(CylTopRunTimer, 30000, 99000))
        {
            DISP_STATE = ERROR_STATE;
            ErrorNum = 2;
        }
    }
    else
    {
        CylTopRunTimer = 0;
    }

    if (FlagGoMid)
    {
        if (ArtIOClass::ARTTimer(CylMidRunTimer, 30000, 99000))
        {
            DISP_STATE = ERROR_STATE;
            ErrorNum = 1;
        }
    }
    else
    {
        CylMidRunTimer = 0;
    }

    if (FlagGoBot)
    {
        if (ArtIOClass::ARTTimer(CylBotRunTimer, 60000, 99000))
        {
            DISP_STATE = ERROR_STATE;
            ErrorNum = 3;
        }
    }
    else
    {
        CylBotRunTimer = 0;
    }

    if (FlagOpenClamps)
    {
        if (ArtIOClass::ARTTimer(CylOpenClampsRunTimer, 20000, 99000))
        {
            DISP_STATE = ERROR_STATE;
            ErrorNum = 4;
        }
    }
    else
    {
        CylOpenClampsRunTimer = 0;
    }

    if (FlagCloseClamps)
    {
        if (ArtIOClass::ARTTimer(CylCloseClampsRunTimer, 20000, 99000))
        {
            DISP_STATE = ERROR_STATE;
            ErrorNum = 4;
        }
    }
    else
    {
        CylCloseClampsRunTimer = 0;
    }
}

void PalletMagazine::ART_DISP_BTN_SPS()
{
    if (ArtIOClass::getInputState(ResButtonInput) && (DispPallState == NO_PALL))
    {
        DispPallState = DispPallSt::HAS_PALL;
    }

    if (ArtIOClass::getInputState(ResButtonInput))
    {
        DISP_STATE = DispStates::READY;
        // PallONConvey = false;
        isMID_timer = 0;
        isBOT_timer = 0;
    }
}

bool PalletMagazine::ART_DISP_BTN_STOP_SPS()
{
    if (ArtIOClass::getInputState(StartButtonPressed))
    {
        StopButtonFlag = false;
        ArtIOClass::setOutputState(10, false); //кнопка зеленая
        ArtIOClass::setOutputState(11, false); //кнопка красная
        ArtIOClass::setOutputState(6, false);  //световая колонна красная
    }

    if (!ArtIOClass::getInputState(StopButtonPressed) || StopButtonFlag)
    {
        StopButtonFlag = true;

        ArtIOClass::setOutputState(11, true);
        ArtIOClass::setOutputState(6, true);

        if (ArtIOClass::ARTTimer(blinkON, 500, 99000) && !flagBlink)
        {
            ArtIOClass::setOutputState(10, true);
            blinkON = 0;
            flagBlink = true;
            blinkOFF = ArtIOClass::ARTTimerGetTime();
        }

        if (ArtIOClass::ARTTimer(blinkOFF, 500, 99000) && flagBlink)
        {
            ArtIOClass::setOutputState(10, false);
            blinkOFF = 0;
            flagBlink = false;
            blinkON = ArtIOClass::ARTTimerGetTime();
        }
        if (Clamp1->cylCloseIn->SensorState())
        {
            ArtIOClass::setOutputState(1, false);
            ArtIOClass::setOutputState(2, false);
            ArtIOClass::setOutputState(3, false);
            ArtIOClass::setOutputState(4, false);
            ArtIOClass::setOutputState(5, false);
            ArtIOClass::setOutputState(6, false);
            ArtIOClass::setOutputState(7, false);
            ArtIOClass::setOutputState(8, false);
            ArtIOClass::setOutputState(9, false);
            ArtIOClass::setOutputState(12, false);
            ArtIOClass::setOutputState(13, false);
            ArtIOClass::setOutputState(14, false);
        }
        else
        {
            ArtIOClass::setOutputState(1, false);
            ArtIOClass::setOutputState(2, false);
            ArtIOClass::setOutputState(3, false);
            ArtIOClass::setOutputState(4, false);
            ArtIOClass::setOutputState(5, true);
            ArtIOClass::setOutputState(6, false);
            ArtIOClass::setOutputState(7, false);
            ArtIOClass::setOutputState(8, false);
            ArtIOClass::setOutputState(9, false);
            ArtIOClass::setOutputState(12, false);
            ArtIOClass::setOutputState(13, false);
            ArtIOClass::setOutputState(14, false);
        }

        return (true);
    }

    return (false);
}

bool PalletMagazine::ART_DISP_BTN_START_SPS()
{
    if ((DispCurPosition == BOT) && (DispPallState == DispPallSt::NO_PALL))
    {
        ArtIOClass::setOutputState(8, false);
        ErrorNum = 7;
        ArtIOClass::Error(ErrorNum, true);
        doOPENCLAMPS();
        ArtIOClass::setOutputState(11, true);
        ArtIOClass::setOutputState(6, true);
        if (ArtIOClass::ARTTimer(blinkON, 500, 99000) && !flagBlink)
        {
            ArtIOClass::setOutputState(10, true);
            ArtIOClass::setOutputState(7, true);
            blinkON = 0;
            flagBlink = true;
            blinkOFF = ArtIOClass::ARTTimerGetTime();
        }

        if (ArtIOClass::ARTTimer(blinkOFF, 500, 99000) && flagBlink)
        {
            ArtIOClass::setOutputState(7, false);
            ArtIOClass::setOutputState(10, false);
            blinkOFF = 0;
            flagBlink = false;
            blinkON = ArtIOClass::ARTTimerGetTime();
        }

        if (!ArtIOClass::getInputState(StartButtonPressed))
        {
            return (true);
        }
        else
        {
            ArtIOClass::Error(ErrorNum, false);
            ArtIOClass::setOutputState(7, false);
            ArtIOClass::setOutputState(11, false);
            ArtIOClass::setOutputState(10, false);
            DispPallState = DispPallSt::LOW_PALL;
            return (false);
        }
    }
    flagBlink = false;
    blinkON = ArtIOClass::ARTTimerGetTime();
    ArtIOClass::setOutputState(10, true);
    ArtIOClass::setOutputState(8, true);
    return (false);
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
    Clamp1->ARTCylinderOpen();
}

void PalletMagazine::doCLOSECLAMPS()
{
    doOpenClamps = false;
    Clamp1->ARTCylinderClose();
}

bool PalletMagazine::ART_DISP_ERR_SPS()
{
    /*if (DISP_STATE == READY)
    {
        ArtIOClass::setOutputState(8, true);
    }
    else
    {
        ArtIOClass::setOutputState(8, false);
    }

    if (DispPallState == DispPallSt::NO_PALL && DISP_STATE == READY)
    {
        if (ArtIOClass::getOutputState(7))
        {
            if (ArtIOClass::CHK_ACTIVE_NTIME(true, &timer, 500))
            {
                ArtIOClass::setOutputState(7, false);
            }
        }
        else
        {
            if (ArtIOClass::CHK_ACTIVE_NTIME(true, &timer, 500))
            {
                ArtIOClass::setOutputState(7, true);
            }
        }
    }
    else
    {
        ArtIOClass::setOutputState(7, false);
        // ArtIOClass::setPulse(7,true,1000);
    }

    if (DispPallState == DispPallSt::NO_PALL)
    {
        if (ArtIOClass::getOutputState(6))
        {
            if (ArtIOClass::CHK_ACTIVE_NTIME(true, &timer, 500))
            {
                ArtIOClass::setOutputState(6, false);
            }
        }
        else
        {
            if (ArtIOClass::CHK_ACTIVE_NTIME(true, &timer, 500))
            {
                ArtIOClass::setOutputState(6, true);
            }
        }
    }
    else
    {
        ArtIOClass::setOutputState(6, false);
    }*/

    if (DISP_STATE == ERROR_STATE)
    {
        ArtIOClass::Error(ErrorNum, true);

        if (ArtIOClass::getInputState(StartButtonPressed))
        {
            ArtIOClass::Error(ErrorNum, false);
            DISP_STATE = DispStates::READY;
            return (false);
        }
        return (true);
    }
    return (false);

    /*if (ArtIOClass::CHK_ACTIVE_NTIME((DispCurPosition == BOT), &isBOTandNOPALLPLAC_timer, isBOTandNOPALLPLAC_TIME))
    {
        if (!isPallONConvey->SensorState())
        {
            DISP_STATE = ERROR_STATE;
        }
    }

    /*if (ARTSystemState == CRITICAL)
      ;DISPGOSTOP()
      disp_run_bool = TRUE
      ;DISP_STATE=#ERROR;
   ELSE
      IF (ARTSystemState == #ARTSYS_STATE_NORMAL)  AND (disp_run_bool == TRUE) THEN
       disp_run_bool = FALSE
       ;DISP_STATE = #READY
       ;DISPGORUN()
      ENDIF
   ENDIF*/
}


