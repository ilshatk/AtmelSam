#include "interface/IHasCycleLogic.h"
#include "Conveyor\ArtConveyor.h"
#include "Actuator\ArtActDriver.h"
#include "Sensor\ArtSensor.h"

std::set<IHasCycleLogic *> IHasCycleLogicHelper::devices;

int IHasCycleLogicHelper::addDevice(IHasCycleLogic *device)
{
	if (device != 0)
	{
		devices.insert(device);
	}
	return (0);
}

int IHasCycleLogicHelper::removeDevice(IHasCycleLogic *device)
{
	if (device != 0)
	{
		devices.erase(device);
	}
	return (0);
}

void IHasCycleLogicHelper::doLogic()
{
	for (std::set<IHasCycleLogic *>::iterator it = devices.begin(); it != devices.end(); ++it)
	{
		(*it)->doLogic();
	}
}

void IHasCycleLogicHelper::initializeDevices()
{
	
}
