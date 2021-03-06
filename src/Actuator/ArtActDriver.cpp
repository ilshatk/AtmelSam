#include "Actuator\ArtActDriver.h"

ArtDriver::ArtDriver(int id, const char name[])
{
    //IHasCycleLogicHelper::addDevice(this);
    memset(nDriverFWDSpeed, 0, sizeof(nDriverFWDSpeed));
    memset(nDriverREVSpeed, 0, sizeof(nDriverREVSpeed));
    //memset(OutState, 0, sizeof(OutState));
    //memcpy(m_name,name,sizeof(name));
}

ArtDriver::ArtDriver(int id, const char name[], DriverType Driver_Type, int ReadySignalIN, int ResetSignalOut, int FwdOut, int RevOut, bool Direction, int CurrentSpeed, int FWDSpeed, int value, int inout1, int inout2, int inout3, int inout4) : ArtDriver(id, name)
{
    nDriverType = Driver_Type;
    ArtDriver::ResetSignalOut = ResetSignalOut;
    nDriverReadySignalIN = ReadySignalIN;
    bDriverDirection = Direction;
    nDriverCurrentSpeed = CurrentSpeed;
    m_id = id;
    ArtDriver::FwdOut = FwdOut;
    ArtDriver::RevOut = RevOut;
    nDriverFWDSpeed[value][0] = inout1; // TODO check to not initilize, if use only one $out
    nDriverFWDSpeed[value][1] = inout2;
    nDriverFWDSpeed[value][2] = inout3;
    nDriverFWDSpeed[value][3] = inout4;
    nDriverREVSpeed[value][0] = inout1; // TODO check to not initilize, if use only one $out
    nDriverREVSpeed[value][1] = inout2;
    nDriverREVSpeed[value][2] = inout3;
    nDriverREVSpeed[value][3] = inout4;
    bisDriverSet = true;
    nDriverState = ST_DRIVER_UNKNOWN;
    bDriverManualMode = false;
    memset(OUT, 0, sizeof(OUT));
    bDriverRUN = false;
}

void ArtDriver::doLogic()
{
    switch (nDriverState)
    {
    case ST_DRIVER_UNKNOWN:
    {
        if (ArtIOClass::getInputState(nDriverReadySignalIN))
        {
            if (bDriverManualMode)
            {
                nDriverState = ST_DRIVER_MANUAL;
            }
            else
            {
                if (bDriverDirection)
                {
                    if (bcheckFWDSpeedOutEnable())
                    {
                        nDriverState = ST_DRIVER_RUN;
                    }
                    else
                    {
                        if (bcheckFWDSpeedOutDisable())
                        {
                            nDriverState = ST_DRIVER_STOP;
                        }
                        else
                        {
                            nDriverState = ST_DRIVER_ERROR;
                        }
                    }
                }
                else
                {
                    if (bcheckREVSpeedOutEnable())
                    {
                        nDriverState = ST_DRIVER_RUN;
                    }
                    else
                    {
                        if (bcheckREVSpeedOutDisable())
                        {
                            nDriverState = ST_DRIVER_STOP;
                        }
                        else
                        {
                            nDriverState = ST_DRIVER_ERROR;
                        }
                    }
                }
            }
        }
        else
        {
            nDriverState = ST_DRIVER_ERROR;
        }
        break;
    }
    case ST_DRIVER_STOP:
    {
        if (ArtIOClass::getInputState(nDriverReadySignalIN)) //; TODO change to $IN
        {
            ConveySetSTOP();
            if (bDriverManualMode)
            {
                nDriverState = ST_DRIVER_MANUAL;
            }
            else
            {
                if (bDriverRUN)
                {
                    nDriverState = ST_DRIVER_RUN;
                }
            }
        }
        else
        {
            nDriverState = ST_DRIVER_ERROR;
        }
        break;
    }
    case ST_DRIVER_RUN:
    {
        if (ArtIOClass::getInputState(nDriverReadySignalIN)) // THEN; TODO change to $IN
        {
            ConveySetRun();
            if (bDriverManualMode)
            {
                nDriverState = ST_DRIVER_MANUAL;
            }
            else
            {
                if (bDriverRUN == false)
                {
                    nDriverState = ST_DRIVER_STOP;
                }
            }
        }
        else
        {
            nDriverState = ST_DRIVER_ERROR;
        }
        break;
    }
    case ST_DRIVER_MANUAL:
    {
        if (bDriverManualMode == false)
        {
            nDriverState = ST_DRIVER_UNKNOWN;
        }
        break;
    }

    case ST_DRIVER_ERROR:
    {
        if (ArtIOClass::ResetDrv(pow(2, ResetSignalOut - 1)))
        {
            ArtIOClass::setOutputState(ResetSignalOut, true);
        }
        else
        {
            ArtIOClass::setOutputState(ResetSignalOut, false);
        }

        if (ArtIOClass::getInputState(nDriverReadySignalIN)) //; TODO change to $IN
        {
            nDriverState = ST_DRIVER_UNKNOWN;
        }
        break;
    }
    default:
        break;
    }
}

bool ArtDriver::bcheckFWDSpeedOutEnable()
{
    int tmpINT, tmpBOOL;
    tmpINT = 0;
    tmpBOOL = 0;

    if (nDriverFWDSpeed[nDriverCurrentSpeed][0] > 1)
    {
        tmpINT = tmpINT | 1;
        if (OUT[nDriverFWDSpeed[nDriverCurrentSpeed][0]])
        {
            tmpBOOL = tmpBOOL | 1;
        }
    }

    if (nDriverFWDSpeed[nDriverCurrentSpeed][1] > 1)
    {
        tmpINT = tmpINT | 2;
        if (OUT[nDriverFWDSpeed[nDriverCurrentSpeed][1]])
        {
            tmpBOOL = tmpBOOL | 2;
        }
    }

    if (nDriverFWDSpeed[nDriverCurrentSpeed][2] > 1)
    {
        tmpINT = tmpINT | 4;
        if (OUT[nDriverFWDSpeed[nDriverCurrentSpeed][2]])
        {
            tmpBOOL = tmpBOOL | 4;
        }
    }
    if (nDriverFWDSpeed[nDriverCurrentSpeed][3] > 1)
    {
        tmpINT = tmpINT | 8;
        if (OUT[nDriverFWDSpeed[nDriverCurrentSpeed][3]])
        {
            tmpBOOL = tmpBOOL | 8;
        }
    }
    return (tmpINT == tmpBOOL);
}

bool ArtDriver::bcheckREVSpeedOutEnable()
{
    int tmpINT, tmpBOOL;
    tmpINT = 0;
    tmpBOOL = 0;

    if (nDriverREVSpeed[nDriverCurrentSpeed][0] > 1)
    {
        tmpINT = tmpINT | 1;
        if ((OUT[nDriverREVSpeed[nDriverCurrentSpeed][0]]))
        {
            tmpBOOL = tmpBOOL | 1;
        }
    }

    if (nDriverREVSpeed[nDriverCurrentSpeed][1] > 1)
    {
        tmpINT = tmpINT | 2;
        if ((OUT[nDriverREVSpeed[nDriverCurrentSpeed][1]]))
        {
            tmpBOOL = tmpBOOL | 2;
        }
    }

    if (nDriverREVSpeed[nDriverCurrentSpeed][2] > 1)
    {
        tmpINT = tmpINT | 4;
        if ((OUT[nDriverREVSpeed[nDriverCurrentSpeed][2]]))
        {
            tmpBOOL = tmpBOOL | 4;
        }
    }

    if (nDriverREVSpeed[nDriverCurrentSpeed][3] > 1)
    {
        tmpINT = tmpINT | 8;
        if ((OUT[nDriverREVSpeed[nDriverCurrentSpeed][3]]))
        {
            tmpBOOL = tmpBOOL | 8;
        }
    }
    return (tmpINT == tmpBOOL);
}

bool ArtDriver::bcheckFWDSpeedOutDisable()
{
    int tmpINT, tmpBOOL;
    tmpINT = 0;
    tmpBOOL = 0;

    if (nDriverFWDSpeed[nDriverCurrentSpeed][0] > 1)
    {
        tmpINT = tmpINT | 1;
        if ((OUT[nDriverFWDSpeed[nDriverCurrentSpeed][0]]) == false)
        {
            tmpBOOL = tmpBOOL | 1;
        }
    }

    if (nDriverFWDSpeed[nDriverCurrentSpeed][1] > 1)
    {
        tmpINT = tmpINT | 2;
        if ((OUT[nDriverFWDSpeed[nDriverCurrentSpeed][1]]) == false)
        {
            tmpBOOL = tmpBOOL | 2;
        }
    }

    if (nDriverFWDSpeed[nDriverCurrentSpeed][2] > 1)
    {
        tmpINT = tmpINT | 4;
        if ((OUT[nDriverFWDSpeed[nDriverCurrentSpeed][2]]) == false)
        {
            tmpBOOL = tmpBOOL | 4;
        }
    }

    if (nDriverFWDSpeed[nDriverCurrentSpeed][3] > 1)
    {
        tmpINT = tmpINT | 8;
        if ((OUT[nDriverFWDSpeed[nDriverCurrentSpeed][3]]) == false)
        {
            tmpBOOL = tmpBOOL | 8;
        }
    }
    return (tmpINT == tmpBOOL);
}

bool ArtDriver::bcheckREVSpeedOutDisable()
{
    int tmpINT, tmpBOOL;
    tmpINT = 0;
    tmpBOOL = 0;

    if (nDriverREVSpeed[nDriverCurrentSpeed][0] > 1)
    {
        tmpINT = tmpINT | 1;
        if ((OUT[nDriverREVSpeed[nDriverCurrentSpeed][0]]) == false)
        {
            tmpBOOL = tmpBOOL | 1;
        }
    }

    if (nDriverREVSpeed[nDriverCurrentSpeed][1] > 1)
    {
        tmpINT = tmpINT | 2;
        if ((OUT[nDriverREVSpeed[nDriverCurrentSpeed][1]]) == false)
        {
            tmpBOOL = tmpBOOL | 2;
        }
    }

    if (nDriverREVSpeed[nDriverCurrentSpeed][2] > 1)
    {
        tmpINT = tmpINT | 4;
        if (((OUT[nDriverREVSpeed[nDriverCurrentSpeed][2]])) == false)
        {
            tmpBOOL = tmpBOOL | 4;
        }
    }

    if (nDriverREVSpeed[nDriverCurrentSpeed][3] > 1)
    {
        tmpINT = tmpINT | 8;
        if ((OUT[nDriverREVSpeed[nDriverCurrentSpeed][3]]) == false)
        {
            tmpBOOL = tmpBOOL | 8;
        }
    }
    return (tmpINT == tmpBOOL);
}

void ArtDriver::ConveySetSTOP()
{
    ConveySetDriverFWD(false);
    ConveySetDriverREV(false);
}

int ArtDriver::ConveySetDriverFWD(bool bParametr)
{
    int i, j;
    bool btmpVal;
    for (i = 0; i < 3; i++)
    {
        if ((i == nDriverCurrentSpeed) && bParametr)
        {
            btmpVal = true;
        }
        else
        {
            btmpVal = false;
        }

        for (j = 0; j <= 3; j++)
        {
            if (nDriverFWDSpeed[i][j] > 1)
            {
                if (btmpVal)
                {
                    OUT[nDriverFWDSpeed[i][j]] = btmpVal;
                    ArtIOClass::setOutputState(FwdOut, true);
                }
                else
                {
                    if (nDriverCurrentSpeed == i)
                    {
                        OUT[nDriverFWDSpeed[i][j]] = btmpVal;
                        ArtIOClass::setOutputState(FwdOut, false);
                    }
                    else
                    {
                        if (nDriverFWDSpeed[nDriverCurrentSpeed][j] != nDriverFWDSpeed[i][j])
                        {
                            OUT[nDriverFWDSpeed[i][j]] = btmpVal;
                        }
                    }
                }
            }
        }
    }
    return (0);
}

int ArtDriver::ConveySetDriverREV(bool bParametr)
{
    int i, j;
    bool btmpVal;

    for (i = 0; i < 3; i++)
    {
        if ((i == nDriverCurrentSpeed) && bParametr)
        {
            btmpVal = true;
        }
        else
        {
            btmpVal = false;
        }

        for (j = 0; j <= 3; j++)
        {
            if (nDriverREVSpeed[i][j] > 1)
            {
                if (btmpVal)
                {
                    number = 0;
                    // OutState[m_id] = 1;
                    ArtIOClass::setOutputState(RevOut, true);
                    for (int k = 0; k <= 5; k++)
                    {
                        //number += pow(2,k)*OutState[k];
                    }
                    // DigitalOut(number);
                    PORT->Group[1].OUTSET.reg = LED;
                }
                else
                {
                    if (nDriverCurrentSpeed == i)
                    {
                        number = 0;
                        //OutState[m_id] = 0;
                        ArtIOClass::setOutputState(RevOut, false);
                        for (int k = 0; k <= 5; k++)
                        {
                            // number += pow(2,k)*OutState[k];
                        }
                        // DigitalOut(number);
                        PORT->Group[1].OUTCLR.reg = LED;
                    }
                    if (nDriverREVSpeed[nDriverCurrentSpeed, j] != nDriverREVSpeed[i, j])
                    {
                        OUT[nDriverREVSpeed[i][j]] = btmpVal;
                    }
                }
            }
        }
    }
    return (0);
}

void ArtDriver::ConveySetRun()
{
    if (bDriverDirection)
    {
        ConveySetDriverFWD(true);
    }
    else
    {
        ConveySetDriverREV(true);
    }
}

int ArtDriver::ARTDriverGetREADY()
{
    if (nDriverReadySignalIN < 1)
    {
        return (0);
    }
    else
    {
        if (ArtIOClass::getInputState(nDriverReadySignalIN)) //$IN[nDriverReadySignalIN[DriverID]])//???????? ??????????
        {
            return (1);
        }
        else
        {
            return (0);
        }
    }
}

int ArtDriver::ARTDriverGetFWD()
{
    if (bDriverDirection && bDriverRUN)
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

int ArtDriver::ARTDriverGetREV()
{
    if (!bDriverDirection && bDriverRUN)
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

int ArtDriver::ARTDriverGetSTOP()
{
    if (bDriverRUN)

    {
        return (0);
    }
    else
    {
        return (1);
    }
}

int ArtDriver::ARTConveySetRun()
{
    bDriverRUN = true;
    if (bDriverDirection)
    {
        ConveySetDriverREV(false);
        ConveySetDriverFWD(true);
    }
    else
    {
        ConveySetDriverFWD(false);
        ConveySetDriverREV(true);
    }

    return (0);
}

int ArtDriver::ARTDriverSetSTOP()
{
    ConveySetSTOP();
    bDriverRUN = false;
    return (0);
}
int ArtDriver::getName()
{
    return (0);
}

int ArtDriver::getID()
{
    return (0);
}

void ArtDriver::update()
{
}