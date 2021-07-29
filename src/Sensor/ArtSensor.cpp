#include "Sensor/ArtSensor.h"

ArtSensor::ArtSensor(int id, const char name[]) // Создаем сенсор (конструктор по умолчанию)
{
	sensorTimerFE = 0;
	sensorTimerRE = 0;
}

ArtSensor::ArtSensor(int id, const char name[], int SensorInput, SensorType type, int delayRe = 0, int delayFe = 0, bool SensorExternal = false) : ArtSensor(id, name) //Задаем параметры(Конструктор)
{
	sensorType = type;
	ArtSensor::SensorExternal = SensorExternal;
	m_id = id;
	isRE = false;
	isFE = false;
	ArtSensor::SensorInput = SensorInput;
	ArtSensor::boardnum = 0;
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
	valueON = false;
	valueOFF = true;
	lastSensorState = false;
	flag = false;
}

ArtSensor::ArtSensor(int id, const char name[], int SensorInput, SensorType type, int delayRe = 0, int delayFe = 0, bool SensorExternal = false, int boardnum) : ArtSensor(id, name) //Задаем параметры(Конструктор)
{
	sensorType = type;
	ArtSensor::SensorExternal = SensorExternal;
	m_id = id;
	isRE = false;
	isFE = false;
	ArtSensor::SensorInput = SensorInput;
	ArtSensor::boardnum = boardnum;
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
	valueON = false;
	valueOFF = true;
	lastSensorState = false;
	flag = false;
}

bool ArtSensor::update()
{
	value = ArtIOClass::getInputState(SensorInput);

	if (sensorDelayRE > 0)
	{
		valueON = ArtIOClass::CHK_ACTIVE_NTIME(value, &sensorTimerRE, sensorDelayRE);
	}

	if (sensorDelayFE > 0 && lastSensorState == true && value == false)
	{
		flag = true;
	}
	if (flag)
	{
		valueOFF = ArtIOClass::CHK_ACTIVE_NTIME(!value, &sensorTimerFE, sensorDelayFE);
		if (valueOFF)
		{
			flag = false;
			sensorTimerFE = 0;
		}
	}
	lastSensorState = value;
	return (!valueOFF || value || valueON);
}

bool ArtSensor::SensorState()
{
	bool state;
	if (!SensorExternal)
	{
		state = update(); /*ArtIOClass::getInputState(SensorInput)*/ // на первый вход enter sensor
		if ((sensorType & 0x1) != 0)								 //0x1 это инверсный тип датчика
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
		if (boardnum > 0)
		{
			if (ArtIOClass::ExtSens(SensorInput, boardnum) == true)
			{
				return (true);
			}
			else
			{
				return (false);
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
}

ArtAnalogSensor::ArtAnalogSensor(int id, const char name[]) // Создаем сенсор (конструктор по умолчанию)
{
}

ArtAnalogSensor::ArtAnalogSensor(int id, const char name[], int SensorInput) : ArtAnalogSensor(id, name) //Задаем параметры(Конструктор)
{
	m_id = id;
	ArtAnalogSensor::SensorInput = SensorInput;
}

int ArtAnalogSensor::update()
{
	return (SensorState());
}

int ArtAnalogSensor::SensorState()
{
	return (ReadAdc(SensorInput)); // на первый вход enter sensor
}