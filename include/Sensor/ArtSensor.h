#ifndef ArtSens
#define ArtSens
#include <cmath>
#include "InOut.h"

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
		delayFe;

public:
	enum SensorType
	{
		SENSOR_TYPE_BASIC,
		SENSOR_TYPE_INVERSE
	};
	int input1,
		m_id;
	ArtSensor(int id, const char name[]);
	ArtSensor(int id, const char name[], int input, SensorType type, int delayRe, int delayFe);
	//~ArtSensor(){};
	int update();
	int SensorState();
};

#endif