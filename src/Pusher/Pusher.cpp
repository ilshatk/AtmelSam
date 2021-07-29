#include "Pusher/ArtPusher.h"

int ArtPusher::getName()
{
    return (0);
}

int ArtPusher::getID()
{
    return (m_id);
}

void ArtPusher::update()
{
}

ArtPusher::ArtPusher(int id, const char name[])
{
    IHasCycleLogicHelper::addDevice(this);
    m_id = id;
}

ArtPusher::ArtPusher(int id, const char name[], ArtSensor *OnPusherPtr, ArtBasicConveyor *ConvPointPtr, ArtBasicConveyor *NextConvPointPtr, ArtCylinder *PusherCylPtr) : ArtPusher(id, name)
{
    ArtPusher::OnPusherPtr = OnPusherPtr;
    ArtPusher::NextConvPointPtr = NextConvPointPtr;
    ArtPusher::PusherCylPtr = PusherCylPtr;
    ArtPusher::ConvPointPtr = ConvPointPtr;
    PusherState = HOME;
}

void ArtPusher::doLogic()
{
    if ((OnPusherPtr->SensorState() == true) && (NextConvPointPtr->ConveyorGetReadyReceive()) && PusherState == HOME)
    {
        PusherCylPtr->ARTCylinderOpen();

        PusherState = PUSHED;
    }

    if (PusherState == PUSHED)
    {
        PusherCylPtr->ARTCylinderClose();
        PusherState = HOME;
    }
}
//-----------------------------ПОджим----------------------------------
ArtPodzhim::ArtPodzhim(int id, const char name[])
{
    IHasCycleLogicHelper::addDevice(this);
    m_id = id;
}

ArtPodzhim::ArtPodzhim(int id, const char name[], ArtSensor *OnPodzhimPtr, ArtCylinder *PodzhimCylPtr) : ArtPodzhim(id, name)
{
    ArtPodzhim::OnPodzhimPtr = OnPodzhimPtr;
    ArtPodzhim::PodzhimCylPtr = PodzhimCylPtr;
    if (PodzhimCylPtr->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)
    {
        PodzhimState == HOME;
    }
    else
    {
        PodzhimCylPtr->ARTCylinderClose();
    }
}

void ArtPodzhim::doLogic()
{
    switch (PodzhimState)
    {
    case HOME:
    {
        if ((OnPodzhimPtr->SensorState() == true))
        {
            PodzhimCylPtr->ARTCylinderOpen();
            PodzhimState = PUSHED;
        }
        break;
    }

    case PUSHED:
    {
        PodzhimCylPtr->ARTCylinderClose();
        while (!PodzhimCylPtr->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)
        {
            break;
        }
        PodzhimState = HOME;
        break;
    }
    default:
        break;
    }
}

int ArtPodzhim::getName()
{
    return (0);
}

int ArtPodzhim::getID()
{
    return (m_id);
}

void ArtPodzhim::update()
{
}

//-----------------------------------------------------------------------------
ArtStopper::ArtStopper(int id, const char name[])
{
    IHasCycleLogicHelper::addDevice(this);
    m_id = id;
}

ArtStopper::ArtStopper(int id, const char name[], ArtSensor *OnStopperPtr, ArtCylinder *StopperCylPtr) : ArtStopper(id, name)
{
    ArtStopper::OnStopperPtr = OnStopperPtr;
    ArtStopper::StopperCylPtr = StopperCylPtr;
    if (StopperCylPtr->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)
    {
        StopperState == HOME;
    }
    else
    {
        StopperCylPtr->ARTCylinderClose();
    }
}

void ArtStopper::doLogic()
{
    switch (StopperState)
    {
    case HOME:
    {
        if ((OnStopperPtr->SensorState() == true))
        {
            StopperCylPtr->ARTCylinderOpen();
            StopperState = STOPED;
        }
        break;
    }

    case STOPED:
    {
        if ((OnStopperPtr->SensorState() == false))
        {
            StopperCylPtr->ARTCylinderClose();
            StopperState = HOME;
        }
        break;
    }
    default:
        break;
    }
}

int ArtStopper::getName()
{
    return (0);
}

int ArtStopper::getID()
{
    return (m_id);
}

void ArtStopper::update()
{
}

//-----------------------------------------------------------------------------
ArtLift::ArtLift(int id, const char name[])
{
    IHasCycleLogicHelper::addDevice(this);
    m_id = id;
}

ArtLift::ArtLift(int id, const char name[], ArtCylinder *LiftCylPtr) : ArtLift(id, name)
{
    ArtLift::LiftCylPtr = LiftCylPtr;
    if (LiftCylPtr->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)
    {
        LiftState == DOWN;
    }
    else
    {
        LiftCylPtr->ARTCylinderClose();
    }
}

void ArtLift::doLogic()
{
    switch (LiftState)
    {
    case UP:
    {// доделать

    }

    case DOWN:
    {
        if (ArtIOClass::ExtDevReady(posnum))
        {
            LiftCylPtr->ARTCylinderOpen();
            while (!LiftCylPtr->getCylState() == ArtCylinder::ARTCYL_ST_OPEN)
            {
                break;
            }
            LiftState = UP;
        }
        break;
    }
    default:
        break;
    }
}

int ArtLift::getName()
{
    return (0);
}

int ArtLift::getID()
{
    return (m_id);
}

void ArtLift::update()
{
}