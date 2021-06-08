#ifndef IHasCycleLogic_H
#define IHasCycleLogic_H

#include <set>

class IHasCycleLogic //интерфейс ARTDevice
{
protected:
	int nDeviceId{0};

public:
	IHasCycleLogic(){};
	virtual int getName() = 0;
	virtual int getID() { return nDeviceId; }; //Даем ID устройству
	virtual void doLogic() = 0;
	virtual ~IHasCycleLogic(){};
};

class IHasCycleLogicHelper
{
public:
	IHasCycleLogicHelper(){};
	virtual ~IHasCycleLogicHelper(){};
	static std::set<IHasCycleLogic *> devices;
	static int addDevice(IHasCycleLogic *device);
	static int removeDevice(IHasCycleLogic *device);
	void doLogic();
	void initializeDevices();
};

#endif