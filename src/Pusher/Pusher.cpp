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

ArtPusher::ArtPusher(int id, const char name[],ArtSensor *OnPusherPtr ,ArtBasicConveyor *ConvPointPtr, ArtBasicConveyor *NextConvPointPtr, ArtCylinder *PusherCylPtr):ArtPusher(id, name)
{
    ArtPusher::OnPusherPtr = OnPusherPtr;
    ArtPusher::NextConvPointPtr = NextConvPointPtr;
    ArtPusher::PusherCylPtr = PusherCylPtr;
    ArtPusher::ConvPointPtr = ConvPointPtr;
    PusherState = HOME;
}

void ArtPusher::doLogic()
{
    if((OnPusherPtr->SensorState() == true) && (NextConvPointPtr->ConveyorGetReadyReceive())&& PusherState == HOME)
    {
        PusherCylPtr->ARTCylinderOpen();

        PusherState = PUSHED;
    }

    if (PusherState == PUSHED)
    {
        PusherCylPtr->ARTCylinderClose();
    }

}