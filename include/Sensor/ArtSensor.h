#ifndef ArtSens
#define ArtSens
#include <cmath>
#include "InOut.h"
#include "ArtIOClass.h"

class ArtSensor
{
protected:
	bool isON,
		isFE,
		isRE,
		value,
		SensorExternal,
		valueOFF,
		valueON,
		flag,
		lastSensorState;
	int sensorTimerFE,
		sensorTimerRE,
		sensorType,
		sensorDelayRE,
		sensorDelayFE,
		delayRe,
		delayFe;

public:
	enum SensorType
	{
		SENSOR_TYPE_BASIC,
		SENSOR_TYPE_INVERSE
	};
	int input1,
		m_id,
		SensorInput;
	ArtSensor(int id, const char name[]);
	ArtSensor(int id, const char name[], int SensorInput, SensorType type, int delayRe, int delayFe, bool SensorExternal);
	//~ArtSensor(){};
	bool update();
	bool SensorState();
};

class ArtAnalogSensor
{
protected:
	bool isON,
		value;

public:
	int input1,
		m_id,
		SensorInput;
	ArtAnalogSensor(int id, const char name[]);
	ArtAnalogSensor(int id, const char name[], int SensorInput);
	int update();
	int SensorState();
};

#endif