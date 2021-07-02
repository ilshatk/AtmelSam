#include "Sensor/ArtSensor.h"

ArtSensor::ArtSensor(int id, const char name[]) // Создаем сенсор (конструктор по умолчанию)
{
	sensorTimerFE = 0;
	sensorTimerRE = 0;
}

ArtSensor::ArtSensor(int id, const char name[], int SensorInput, SensorType type, int delayRe = 0, int delayFe = 0,  bool SensorExternal = false) : ArtSensor( id, name) //Задаем параметры(Конструктор)
{
	sensorType = type;
	sensorTimerFE = delayFe;
	sensorTimerRE = delayRe;
	ArtSensor::SensorExternal = SensorExternal;
	m_id = id;
	isRE = false;
	isFE = false;
	ArtSensor::SensorInput = SensorInput;
	if (delayRe > 0) //Передний фронт
	{
		sensorDelayRE = delayRe;
	}
	else
	{
		sensorDelayRE = 0;
	}

	if (delayFe > 0) //Задний фронт
	{
		sensorDelayFE = delayFe;
	}
	else
	{
		sensorDelayFE = 0;
	}
}

int ArtSensor::update()
{
	return (SensorState());
}

bool ArtSensor::SensorState()
{
	bool state;
	if (SensorExternal == false)
	{
		state = ArtIOClass::getInputState(SensorInput); // на первый вход enter sensor
		if ((sensorType & 0x1) != 0)					//0x1 это инверсный тип датчика
		{
			return (!state);
		}
		else
		{
			return (state);
		}
	}
	else
	{
		if (ArtIOClass::ExtSens(SensorInput) == true)
		{
			return (true);
		}
		else
		{
			return (false);
		}
	}	
}