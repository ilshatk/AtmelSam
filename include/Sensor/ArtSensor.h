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
		value;
	int sensorTimerFE,
		sensorTimerRE,
		sensorType,
		sensorDelayRE,
		sensorDelayFE,
		delayRe,
		delayFe,
		SensorInput;

public:
	enum SensorType
	{
		SENSOR_TYPE_BASIC,
		SENSOR_TYPE_INVERSE
	};
	int input1,
		m_id;
	ArtSensor(int id, const char name[]);
	ArtSensor(int id, const char name[], int SensorInput, SensorType type, int delayRe, int delayFe);
	//~ArtSensor(){};
	int update();
	bool SensorState();
};

#endif