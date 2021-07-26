#include "Conveyor\ArtConveyor.h"

//---------------------------------Basic Conveyor--------------------------------------------------
int ArtBasicConveyor::ActuatorsGet(int parameter, ArtDriver *pointer) //; TODO MOTOR CONTROL AS ACTUATOR
{
	switch (parameter)
	{
	case GET_CONV_ACTUATOR_READY:
	{
		return (pointer->ARTDriverGetREADY());
		break;
	}

	case GET_CONV_ACTUATOR_FWD:
	{
		return (pointer->ARTDriverGetFWD());
		break;
	}

	case GET_CONV_ACTUATOR_REV: //; TODO dont relase reverse.
	{
		return (pointer->ARTDriverGetREV());
		break;
	}

	case GET_CONV_ACTUATOR_STOP:
	{
		return (pointer->ARTDriverGetSTOP());
		break;
	}

	default:
		break;
	}
	return (0);
}

int ArtBasicConveyor::ConveyorGetReadyReceive()
{

	if (conveyorType == CONVEYOR_TYPE_2)
	{
		if (((conveyorState == ST_CONVEYOR_FREE)) && (CalcedNumProdInConveyor < SetedProdNumberCollect))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (conveyorType == CONVEYOR_TYPE_1)
	{
		if ((conveyorState == ST_CONVEYOR_FREE) || (conveyorState == ST_CONVEYOR_PROD_FWD))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (conveyorType == CONVEYOR_TYPE_4)
	{
		if ((conveyorState == ST_CONVEYOR_FREE))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (conveyorType == CONVEYOR_TYPE_3)
	{
		if ((conveyorState == ST_CONVEYOR_FREE))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (conveyorType == CONVEYOR_TYPE_1A)
	{
		if (AccumConvOn == true)
		{
			if (((conveyorState == ST_CONVEYOR_FREE)) && (CalcedNumProdInConveyor < SetedProdNumberCollect))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if ((conveyorState == ST_CONVEYOR_FREE))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}

	if ((conveyorState == ST_CONVEYOR_FREE))
	{
		return 1;
	}

	return 0;
}

int ArtBasicConveyor::ActuatorsSet(int parameter, ArtDriver *pointer) // TODO MOTOR CONTROL AS ACTUATOR
{

	switch (parameter)
	{
	case SET_CONV_ACTUATOR_FWD:
	{
		pointer->bDriverDirection = true;
		return (pointer->ARTConveySetRun());
		break;
	}

	case SET_CONV_ACTUATOR_REV: //TODO dont relase reverse.
	{
		pointer->bDriverRUN = true;
		pointer->bDriverDirection = false;
		return (pointer->ARTConveySetRun());
		break;
	}
	case SET_CONV_ACTUATOR_STOP:
	{
		pointer->ARTDriverSetSTOP();
		pointer->bDriverRUN = false;
		break;
	}
	default:
		break;
	}
	return (0);
}

bool ArtBasicConveyor::ARTTimerIsTimePassed(int timer, int timeSpan, int timeOut)
{
	int curTime, deltaTime;

	if (timer > 0)
	{
		curTime = millis();
		if (timer < curTime)
		{
			deltaTime = curTime - timer;
			return ((deltaTime > timeSpan) && (deltaTime <= timeOut));
		}
		else //; exceptions check
		{
			if (timer == curTime)
			{
				return (false);
			}
			else
			{
				timer = timer - 3600000;
			}
		}
	}
	return (false);
}

void ArtBasicConveyor::ConveyorSetProdEntering()
{
	productFctEnterConveyor = true;
}

void ArtConveyor1AType::AccumConv(bool flag)
{
	AccumConvOn = flag;
}

//---------------------------------1 Type--------------------------------------------------
ArtConveyor1Type::ArtConveyor1Type(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyor1Type::ArtConveyor1Type(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2, ArtSensor *point3, ArtBasicConveyor *point4, int PassTime, int RunTimer) : ArtConveyor1Type(id, name)
{
	conveyorType = type;
	ActPoint = point;		 //указатель на драйвер
	EnterSensPoint = point2; //указатель на входной сенсор
	ExitSensPoint = point3;	 //указатель на выходной сенсор
	NextConvPoint = point4;	 //указатель на следующий конвейер
	productPassTime = PassTime;
	conveyorRunTimer = 0;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	SetedProdNumberCollect = 0;
	CalcedNumProdInConveyor = 0;
}

void ArtConveyor1Type::doLogic()
{
	//int retVal;
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1)
	{
		ArtIOClass::Error(0);
	}
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
		ArtIOClass::Error(m_id);
	}

	if (productFctEnterConveyor)
	{
		productEnterSensConvey = true;
		productFctEnterConveyor = false;
	}
	else
	{
		productEnterSensConvey = EnterSensPoint->SensorState();
	}

	productExitSensConvey = ExitSensPoint->SensorState();

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (ConveyorGetReadyReceive() == 1)
		{
			conveyorState = ST_CONVEYOR_FREE;
		}
		else
		{
			if (productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_BUSY;
			}
			else
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}

		break;
	}
	case ST_CONVEYOR_FREE:
	{
		if ((NextConvPoint->ConveyorGetReadyReceive() != 1) && (productExitSensConvey))
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}

		if (productEnterSensConvey)
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
		}
		break;
	}

	case ST_CONVEYOR_PROD_FWD:
	{
		if (conveyorRunTimer == 0)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!* !Change to actual function
		}

		if (productEnterSensConvey)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_FREE;
		}
		else
		{
			if ((NextConvPoint->ConveyorGetReadyReceive() != 1) && productExitSensConvey)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
				conveyorRunTimer = 0;
				conveyorState = ST_CONVEYOR_BUSY;
			}
		}
		break;
	}
	case ST_CONVEYOR_BUSY:
	{
		if (NextConvPoint->ConveyorGetReadyReceive() == 1)
		{
			ConveyorSetProdEntering();
			conveyorState = ST_CONVEYOR_PROD_FWD;
			conveyorRunTimer = 0;
		}
		else
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
		}
		break;
	}
	case ST_CONVEYOR_ERROR:
	{
		if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1) //; !* !Change ARTActuatorsGet to actual function
		{

			conveyorState = ST_CONVEYOR_UNKNOWN;
		}
		break;
	}
	default:
		break;
	}
}

//-----------------------2 type-----------------------------------------

ArtConveyor2Type::ArtConveyor2Type(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
}

ArtConveyor2Type::ArtConveyor2Type(int id, const char name[], ConveyorType type, ArtDriver *act2, ArtSensor *EnterSensPtr, ArtSensor *CountSensPtr, ArtSensor *ExitSensPtr, int PassTime, int NumProdInConveyor, int InSensAllowTime) : ArtConveyor2Type(id, name)
{
	productPassTime = PassTime;
	productInSensAllowTime = InSensAllowTime;
	m_id = id;
	conveyorType = type;
	ActPoint2 = act2;							   //указатель на драйвер
	ArtConveyor2Type::EnterSensPtr = EnterSensPtr; //указатель на входной сенсор
	ArtConveyor2Type::ExitSensPtr = ExitSensPtr;
	ArtConveyor2Type::CountSensPtr = CountSensPtr; //указатель на выходной(считающий) сенсор
	conveyorRunTimer = 0;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	SetedProdNumberCollect = NumProdInConveyor;
	fproductCounted = false;
	pp_stack_ready = false;
	ftook_product = false;
	CalcedNumProdInConveyor = 0;
}

void ArtConveyor2Type::doLogic()
{
	//int retVal;
	//float _time;

	if (productFctEnterConveyor)
	{
		productEnterSensConvey = true;
		productFctEnterConveyor = false;
	}
	else
	{
		productEnterSensConvey = EnterSensPtr->SensorState();
	}

	productExitSensConvey = ExitSensPtr->SensorState();
	productCountSensConvey = CountSensPtr->SensorState();

	if ((ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint2)) == 1) // ; TODO: Change ARTActuatorsGet to actual function
	{
		if (false /*isManualMode*/)
		{
			conveyorState = ST_CONVEYOR_CONT_MANUAL;
		}
		else
		{
			if (productCountSensConvey)
			{
				if (!fproductCounted)
				{
					CalcedNumProdInConveyor = CalcedNumProdInConveyor + 1;
					fproductCounted = true;
				}
			}
			else
			{
				fproductCounted = false;
			}
			if (CalcedNumProdInConveyor > SetedProdNumberCollect)
			{
				conveyorState = ST_CONVEYOR_ERROR;
			}
		}
	}

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (productEnterSensConvey || productExitSensConvey)
		{
			if (conveyorRunTimer != 0)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
				conveyorRunTimer = 0;
			}
			else
			{
				if (conveyorRunTimer == 0)
				{
					conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
					ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint2); //!*! Change to actual function
					conveyorState = ST_CONVEYOR_FREE;
				}
				if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
				{
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
					conveyorRunTimer = 0;
					CalcedNumProdInConveyor = 0;
					conveyorState = ST_CONVEYOR_FREE;
				}
			}
		}
		break;
	}
	case ST_CONVEYOR_FREE:
	{
		if (productEnterSensConvey)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint2); //!*! Change to actual function
		}
		if (ARTTimerIsTimePassed(conveyorRunTimer, 90000, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
			conveyorState = ST_CONVEYOR_ERROR;
		}
		if (CalcedNumProdInConveyor == SetedProdNumberCollect)
		{
			ArtIOClass::GaveStack(false);
			conveyorState = ST_CONVEYOR_BUSY;
			conveyorRunTimer = 0;
			pp_stack_ready = true;
			ArtIOClass::StackReady(true);
		}
		else
		{
			if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
				conveyorRunTimer = 0;
			}
		}
		break;
	}
	case ST_CONVEYOR_BUSY:
	{
		if (conveyorRunTimer == 0)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint2); //!*! Change to actual function
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, (int)(productPassTime * 1.2), 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
		}

		if (pp_stack_ready)
		{
			ftook_product = false;
			conveyorState = ST_CONVEYOR_STACK_READY;
			pp_stack_ready = false;
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, (int)(productPassTime * 1.2), 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
		}
		break;
	}

	case ST_CONVEYOR_STACK_READY:
	{
		ArtConveyorTookProduct();
		if (ftook_product)
		{
			if (productCountSensConvey || productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_ERROR;
			}
			else
			{
				CalcedNumProdInConveyor = 0;
				ftook_product = false;
				conveyorState = ST_CONVEYOR_FREE;
				ArtIOClass::StackReady(false);
				ArtIOClass::GaveStack(true);
			}
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, (int)(productPassTime * 1.2), 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2);
		}
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2);

		if (DigitalIn() & 8)
		{
			CalcedNumProdInConveyor = 0;

			conveyorState = ST_CONVEYOR_UNKNOWN;
		}
		break;
	}
	default:
		break;
	}
}

void ArtConveyor2Type::ArtConveyorTookProduct()
{
	if (ArtIOClass::TookProd())
	{
		ftook_product = true;
	}
}

int ArtBasicConveyor::ConveyorGetState()
{
	return (conveyorState);
}

//---------------------------------1E Type--------------------------------------------------

ArtConveyor1EType::ArtConveyor1EType(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyor1EType::ArtConveyor1EType(int id, const char name[], ConveyorType type, ArtDriver *Act3, ArtSensor *Sens3_Enter, ArtSensor *Sens3_Exit, ArtBasicConveyor *Next3_Conv, int PassTime, int RunTimer) : ArtConveyor1EType(id, name)
{
	conveyorType = type;
	ActPoint3 = Act3;			   //указатель на драйвер
	EnterSensPoint3 = Sens3_Enter; //указатель на входной сенсор
	ExitSensPoint3 = Sens3_Exit;   //указатель на выходной сенсор
	NextConv1Point = Next3_Conv;   //указатель на следующий конвейер
	productPassTime3 = PassTime;
	conveyorRunTimer = 0;
	nconveyorExtModeEnabled = true;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	SetedProdNumberCollect = 0;
	CalcedNumProdInConveyor = 0;
}

void ArtConveyor1EType::doLogic()
{
	//ArtIOClass::ConvState(conveyorState);
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint3) == 1)
	{
		if (/*isManualMode*/ false)
		{
			conveyorState = ST_CONVEYOR_CONT_MANUAL;
			return;
		}
		else
		{
			if (conveyorState == ST_CONVEYOR_CONT_MANUAL)
			{
				conveyorState = ST_CONVEYOR_UNKNOWN;
			}
		}

		if (/*nconveyorExtModeEnabled*/ (DigitalIn() & int(pow(2, 15))) == pow(2, 15) && (!/*isManualMode*/ false))
		{
			ConveyorSaveStateAndTime();
			conveyorState = ST_CONVEYOR_CONT_EXT;
		}
		else
		{
			if (conveyorState == ST_CONVEYOR_CONT_EXT)
			{
				ConvReturnStateAndTime();
			}
		}
	}
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
	}

	if (productFctEnterConveyor)
	{
		productEnterSensConvey = true;
		productFctEnterConveyor = false;
	}
	else
	{
		productEnterSensConvey = EnterSensPoint3->SensorState(); //Указатель на датчик
	}
	productExitSensConvey = ExitSensPoint3->SensorState(); //указатель на датчик

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if ((NextConv1Point->ConveyorGetReadyReceive()) == 1)
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
			//DebugMsgNotify("conveyorState[ %1 ] == ST_RUN","ARTConveyorType1EDoSPS",conveyorID)
		}
		else
		{
			if (productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_BUSY;
			}
			else
			{
				conveyorState = ST_CONVEYOR_PROD_FWD;
			}
		}
		break;
	}

	case ST_CONVEYOR_FREE:
	{
		if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint3) == 1)
		{
			if (productEnterSensConvey)
			{
				conveyorState = ST_CONVEYOR_PROD_FWD;
			}
		}
		else
		{
			if (productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_BUSY;
			}
		}
		break;
	}
	case ST_CONVEYOR_PROD_FWD:
	{
		if ((conveyorRunTimer == 0) || (productEnterSensConvey))
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint3);
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime3, 99000))
		{
			conveyorRunTimer = 0;
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint3);
			conveyorState = ST_CONVEYOR_FREE;
		}
		else
		{
			if (((NextConv1Point->ConveyorGetReadyReceive()) != 1) && productExitSensConvey)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint3);
				conveyorRunTimer = 0;
				conveyorState = ST_CONVEYOR_BUSY;
			}
		}
		break;
	}
	case ST_CONVEYOR_BUSY:
	{
		if (NextConv1Point->ConveyorGetReadyReceive() == 1)
		{
			ConveyorSetProdEntering();
			conveyorState = ST_CONVEYOR_PROD_FWD;
		}
		else
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint3);
			conveyorRunTimer = 0;
		}

		if (!productExitSensConvey)
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
		}
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint3) == 1)
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
		}
		break;
	}

	case ST_CONVEYOR_CONT_EXT:
	{
		if (/*nconveyorExtSTOP*/ (DigitalIn() & int(pow(2, 13))) == pow(2, 13))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint3);
		}
		if (/*nconveyorExtFWD*/ (DigitalIn() & int(pow(2, 14))) == pow(2, 14))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint3);
		}
		if (/*nconveyorExtREV*/ false)
		{
			//ActuatorsSet(SET_CONV_ACTUATOR_REV, ActPoint3);
		}
		break;
	}
	default:
		break;
	}
}

void ArtConveyor1EType::ConveyorSaveStateAndTime()
{
	float _time;
	nconveyorLastState = conveyorState;
	if (conveyorState == ST_CONVEYOR_PROD_FWD)
	{
		_time = ArtIOClass::ARTTimerGetTime();
		productDeltaPassTime = _time - conveyorRunTimer;
	}
}

void ArtConveyor1EType::ConvReturnStateAndTime()
{
	float _time;
	conveyorState = nconveyorLastState;

	if (conveyorState == ST_CONVEYOR_PROD_FWD)
	{
		_time = ArtIOClass::ARTTimerGetTime();
		productDeltaPassTime = _time - productDeltaPassTime;
	}
}

//---------------------------------1A Type-----------------------------------------------------------

ArtConveyor1AType::ArtConveyor1AType(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyor1AType::ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2, ArtSensor *point3, ArtBasicConveyor *point4, int PassTime, int RunTimer, int SetedProdNumberCollect) : ArtConveyor1AType(id, name)
{
	conveyorType = type;
	ActPoint = point;						   //указатель на драйвер
	EnterSensPoint = point2;				   //указатель на входной сенсор
	ExitSensPoint = point3;					   //указатель на выходной сенсор
	ArtConveyor1AType::NextConvPoint = point4; //указатель на следующий конвейер
	productPassTime = PassTime;
	conveyorRunTimer = 0;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtBasicConveyor::SetedProdNumberCollect = SetedProdNumberCollect;
	CalcedNumProdInConveyor = 0;
}

ArtConveyor1AType::ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2, ArtSensor *point3, bool readySignalFromNextBarda, int PassTime, int RunTimer, int SetedProdNumberCollect) : ArtConveyor1AType(id, name)
{
	conveyorType = type;
	ActPoint = point;														//указатель на драйвер
	EnterSensPoint = point2;												//указатель на входной сенсор
	ExitSensPoint = point3;													//указатель на выходной сенсор
	ArtConveyor1AType::readySignalFromNextBarda = readySignalFromNextBarda; //указатель на следующий конвейер
	productPassTime = PassTime;
	conveyorRunTimer = 0;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtBasicConveyor::SetedProdNumberCollect = SetedProdNumberCollect;
	CalcedNumProdInConveyor = 0;
}

void ArtConveyor1AType::doLogic()
{
	if (conveyorType == CONVEYOR_TYPE_1_EXTERNAL_BARDA)
	{
		readySignalFromNextBarda = ArtIOClass::readySignalFromNext();
	}

	if (productFctEnterConveyor)
	{
		productEnterSensConvey = true;
		productFctEnterConveyor = false;
	}
	else
	{
		productEnterSensConvey = EnterSensPoint->SensorState();
	}

	productExitSensConvey = ExitSensPoint->SensorState();

	if (AccumConvOn) //если конвейер накопительный
	{
		productCountSensConvey = EnterSensPoint->SensorState();

		if (/*(ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint)) == 1*/ true)
		{
			if (false /*isManualMode*/)
			{
				conveyorState = ST_CONVEYOR_CONT_MANUAL;
			}
			else
			{
				if (productCountSensConvey)
				{
					if (!fproductCounted)
					{
						CalcedNumProdInConveyor = CalcedNumProdInConveyor + 1;
						fproductCounted = true;
					}
				}
				else
				{
					fproductCounted = false;
				}
				if (CalcedNumProdInConveyor > SetedProdNumberCollect)
				{
					conveyorState = ST_CONVEYOR_ERROR;
				}
			}
		}

		switch (conveyorState)
		{
		case ST_CONVEYOR_UNKNOWN:
		{
			if (productEnterSensConvey || productExitSensConvey)
			{
				if (conveyorRunTimer != 0)
				{
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!*! Change to actual function
					conveyorRunTimer = 0;
				}
				else
				{
					if (conveyorRunTimer == 0)
					{
						conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!*! Change to actual function
						conveyorState = ST_CONVEYOR_FREE;
					}
					if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!*! Change to actual function
						conveyorRunTimer = 0;
						CalcedNumProdInConveyor = 0;
						conveyorState = ST_CONVEYOR_FREE;
					}
				}
			}
			break;
		}
		case ST_CONVEYOR_FREE:
		{
			if (productEnterSensConvey)
			{
				conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
				ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!*! Change to actual function
			}
			/*if (ARTTimerIsTimePassed(conveyorRunTimer, 90000, 99000))
				{
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!*! Change to actual function
					conveyorState = ST_CONVEYOR_ERROR;
				}*/
			if (CalcedNumProdInConveyor == SetedProdNumberCollect)
			{
				conveyorState = ST_CONVEYOR_BUSY;
				conveyorRunTimer = 0;
				pp_stack_ready = true;
				//ArtConveyorSetStackReady(conveyorID);
			}
			else
			{
				if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
				{
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!*! Change to actual function
					conveyorRunTimer = 0;
				}
			}
			break;
		}
		case ST_CONVEYOR_BUSY:
		{
			if (conveyorRunTimer == 0)
			{
				conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
				ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!*! Change to actual function
			}

			if ((ARTTimerIsTimePassed(conveyorRunTimer, (int)(productPassTime * 1.2), 99000)))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!*! Change to actual function
			}

			if (pp_stack_ready)
			{
				//ftook_product = false;
				conveyorState = ST_CONVEYOR_STACK_READY;
			}

			//pp_stack_ready = false;
			break;
		}

		case ST_CONVEYOR_STACK_READY:
		{
			if (!productExitSensConvey)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
			}
			if (conveyorType == CONVEYOR_TYPE_1_EXTERNAL_BARDA)
			{
				if (readySignalFromNextBarda)
				{
					ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
					CalcedNumProdInConveyor = 0;
					conveyorState = ST_CONVEYOR_FREE;
					conveyorRunTimer = 0;
				}
			}
			else
			{
				if (NextConvPoint->ConveyorGetReadyReceive())
				{
					ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
					CalcedNumProdInConveyor = 0;
					conveyorState = ST_CONVEYOR_FREE;
					conveyorRunTimer = 0;
				}
			}
			/*	if (ftook_product)
				{

					/*if (productCountSensConvey || productExitSensConvey)
					{
						conveyorState = ST_CONVEYOR_ERROR;
					}
					else
					{*/

			/*CalcedNumProdInConveyor = 0;
					ftook_product = false;
					conveyorState = ST_CONVEYOR_FREE;
					//}
				}*/
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			if (ARTTimerIsTimePassed(conveyorRunTimer, (int)(productPassTime * 1.2), 99000))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			}
			break;
		}

		case ST_CONVEYOR_ERROR:
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);

			if (DigitalIn() & 8)
			{
				CalcedNumProdInConveyor = 0;

				conveyorState = ST_CONVEYOR_UNKNOWN;
			}
			break;
		}
		default:
			break;
		}
	}
	else // если конвейер проходной
	{
		switch (conveyorState)
		{
		case ST_CONVEYOR_UNKNOWN:
		{
			if (ConveyorGetReadyReceive() == 1)
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
			else
			{
				if (productExitSensConvey)
				{
					conveyorState = ST_CONVEYOR_BUSY;
				}
				else
				{
					conveyorState = ST_CONVEYOR_FREE;
				}
			}

			break;
		}
		case ST_CONVEYOR_FREE:
		{
			if (conveyorType == CONVEYOR_TYPE_1_EXTERNAL_BARDA)
			{
				if (readySignalFromNextBarda)
				{
					ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
					CalcedNumProdInConveyor = 0;
					conveyorState = ST_CONVEYOR_FREE;
					conveyorRunTimer = 0;
				}
			}
			else
			{
				if ((NextConvPoint->ConveyorGetReadyReceive() != 1) && (productExitSensConvey))
				{
					conveyorState = ST_CONVEYOR_BUSY;
				}
			}
			if (productEnterSensConvey)
			{
				conveyorState = ST_CONVEYOR_PROD_FWD;
			}
			break;
		}

		case ST_CONVEYOR_PROD_FWD:
		{
			if (conveyorRunTimer == 0)
			{
				conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
				ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!* !Change to actual function
			}

			if (productEnterSensConvey)
			{
				conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			}

			if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
				conveyorRunTimer = 0;
				conveyorState = ST_CONVEYOR_FREE;
			}
			else
			{
				if ((NextConvPoint->ConveyorGetReadyReceive() != 1) && productExitSensConvey)
				{
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
					conveyorRunTimer = 0;
					conveyorState = ST_CONVEYOR_BUSY;
				}
			}
			break;
		}
		case ST_CONVEYOR_BUSY:
		{
			if (NextConvPoint->ConveyorGetReadyReceive() == 1)
			{
				ConveyorSetProdEntering();
				conveyorState = ST_CONVEYOR_PROD_FWD;
				conveyorRunTimer = 0;
			}
			else
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
				conveyorRunTimer = 0;
			}
			break;
		}
		case ST_CONVEYOR_ERROR:
		{
			if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1) //; !* !Change ARTActuatorsGet to actual function
			{
				conveyorState = ST_CONVEYOR_UNKNOWN;
			}
			break;
		}
		default:
			break;
		}
	}
}

//---------------------------------Shuttle--------------------------------------------------

ArtConveyorShuttleType::ArtConveyorShuttleType(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyorShuttleType::ArtConveyorShuttleType(int id, const char name[], ArtDriver *ShuttlePtr, ArtDriver *OverShuttlePtr, ArtAnalogSensor *PositionSens,
											   ArtSensor *PalletOnConv, ArtBasicConveyor *NextConvPtr, bool readySignalFromNextBarda, int PassTimeOverShuttle, int PassTimeShuttle,
											   int ConveyorRunTimerShuttle, int ConveyorRunTimerOverShuttle) : ArtConveyorShuttleType(id, name)
{
	ArtConveyorShuttleType::ShuttlePtr = ShuttlePtr;		 //указатель на драйвер шатла
	ArtConveyorShuttleType::OverShuttlePtr = OverShuttlePtr; //указатель на драйвер конвейера над шатлом
	ArtConveyorShuttleType::PositionSens = PositionSens;	 //указатель на аналоговый датчик
	ArtConveyorShuttleType::PalletOnConv = PalletOnConv;	 //указатель на датчик паллеты
	ArtConveyorShuttleType::NextConvPtr = NextConvPtr;		 //указатель на следующий конвейер
	ArtConveyorShuttleType::productPassOverShuttle = PassTimeOverShuttle;
	ArtConveyorShuttleType::PassTimeShuttle = PassTimeShuttle;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	CalcedNumProdInConveyor = 0;
	ArtConveyorShuttleType::CurPos = PositionSens->SensorState();
	On_Position = false;
	ArtConveyorShuttleType::ConveyorRunTimerShuttle = ConveyorRunTimerShuttle;
	ArtConveyorShuttleType::ConveyorRunTimerOverShuttle = ConveyorRunTimerOverShuttle;
}

void ArtConveyorShuttleType::doLogic()
{
	ReqPos = ArtIOClass::ReqPos();
	//CurPos = expRunningAverage(findMedianN_optim(((31  * CurPos + PositionSens->SensorState()) >> 5)));
	CurPos = expRunningAverage(((31 * CurPos + PositionSens->SensorState()) >> 5));
	//CurPos = ((31  * CurPos + PositionSens->SensorState()) >> 5);
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ShuttlePtr) == 1)
	{
	}
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
	}
	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (PalletOnConv->SensorState() == false)
		{
			if (conveyorRunTimer != 0)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr); //!*! Change to actual function
				conveyorRunTimer = 0;
			}
			else
			{
				if (abs(CurPos - ReqPos) >= 40)
				{
					if (conveyorRunTimer == 0)
					{
						//ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!*! Change to actual function
						conveyorState = ST_CONVEYOR_RUN;
					}
				}
				else
				{
					conveyorState = ST_CONVEYOR_AT_THE_POINT;
				}
			}
		}
		else
		{
			if (PalletOnConv->SensorState() == true)
			{
				if (abs(CurPos - ReqPos) <= 40)
				{
					conveyorState = ST_CONVEYOR_AT_THE_POINT;
				}
				else
				{
					if (!(OverShuttlePtr->ARTDriverGetFWD() || OverShuttlePtr->ARTDriverGetREV()))
					{
						conveyorState = ST_CONVEYOR_RUN;
						ArtIOClass::OnPosition(0);
					}
				}
			}
		}
		break;
	}
	case ST_CONVEYOR_RUN:
	{
		On_Position = false;

		/*if (productEnterSensConvey)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime()();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr); //!*! Change to actual function
		}*/
		if ((CurPos - ReqPos) > 40 || (CurPos - ReqPos) < (-40))
		{
			ConveyorRunTimerShuttle = ArtIOClass::ARTTimerGetTime();
			if ((CurPos - ReqPos) < (-40))
			{

				if ((CurPos - ReqPos) <= (-10000))
				{
					ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
					WriteDacValues(0, 32600);
				}

				if ((CurPos - ReqPos) <= (-500) && (CurPos - ReqPos) > (-10000))
				{
					ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
					WriteDacValues(0, 25000);
				}

				if ((CurPos - ReqPos) < (-40) && (CurPos - ReqPos) > (-500))
				{
					ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
					WriteDacValues(0, 8000);
				}
			}
			else
			{
				if ((CurPos - ReqPos) > 40)
				{
					if ((CurPos - ReqPos) >= 10000)
					{
						ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
						WriteDacValues(0, 32600);
					}

					if ((CurPos - ReqPos) >= 500 && (CurPos - ReqPos) < 10000)
					{
						ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
						WriteDacValues(0, 25000);
					}

					if ((CurPos - ReqPos) > 40 && (CurPos - ReqPos) < 500)
					{
						ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
						WriteDacValues(0, 8000);
					}
				}
			}

			if (ARTTimerIsTimePassed(ConveyorRunTimerShuttle, PassTimeShuttle, 99000))
			{
				WriteDacValues(0, 0);
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr); //!*! Change to actual function
				conveyorState = ST_CONVEYOR_ERROR;
			}
		}
		else
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
		}

		if ((CurPos - ReqPos) < 40 && (CurPos - ReqPos) > (-40))
		{
			WriteDacValues(0, 0);
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_AT_THE_POINT;
		}

		break;
	}
	case ST_CONVEYOR_AT_THE_POINT:
	{
		if ((CurPos - ReqPos) < 40 && (CurPos - ReqPos) > (-40))
		{
			ArtIOClass::OnPosition(1);
			if (ArtIOClass::LoaUnloadind() == true && PalletOnConv->SensorState() == false) // true - загрузка, false - выгрузка
			{
				ConveyorRunTimerOverShuttle = ArtIOClass::ARTTimerGetTime();
				ActuatorsSet(SET_CONV_ACTUATOR_FWD, OverShuttlePtr);
			}

			if (ArtIOClass::LoaUnloadind() == true && PalletOnConv->SensorState() == true)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr);
				ConveyorRunTimerOverShuttle = 0;
			}

			if (ArtIOClass::LoaUnloadind() == false && PalletOnConv->SensorState() == true)
			{
				ConveyorRunTimerOverShuttle = ArtIOClass::ARTTimerGetTime();
				ActuatorsSet(SET_CONV_ACTUATOR_FWD, OverShuttlePtr);
			}

			if (ArtIOClass::LoaUnloadind() == false && PalletOnConv->SensorState() == false)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr);
				ConveyorRunTimerOverShuttle = 0;
			}

			if (ARTTimerIsTimePassed(ConveyorRunTimerOverShuttle, productPassOverShuttle, 99000))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr); //!*! Change to actual function
				conveyorRunTimer = 0;
				conveyorState = ST_CONVEYOR_ERROR;
			}
		}
		else
		{
			ArtIOClass::OnPosition(0);
			conveyorState = ST_CONVEYOR_RUN;
		}
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);

		if (DigitalIn() & 8)
		{
			CalcedNumProdInConveyor = 0;
			conveyorState = ST_CONVEYOR_UNKNOWN;
		}
		break;
	}
	default:
		break;
	}
}

//---------------------------------Shuttle--------------------------------------------------

//---------------------------------ArtConveyor1TypeNextExtDev--------------------------------------------------

ArtConveyor1TypeNextExtDev::ArtConveyor1TypeNextExtDev(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyor1TypeNextExtDev::ArtConveyor1TypeNextExtDev(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, int PassTime, int RunTimer, int ExtDevReady) : ArtConveyor1TypeNextExtDev(id, name)
{
	conveyorType = type;
	ArtConveyor1TypeNextExtDev::ActPoint = ActPoint;			 //указатель на драйвер
	ArtConveyor1TypeNextExtDev::EnterSensPoint = EnterSensPoint; //указатель на входной сенсор
	ArtConveyor1TypeNextExtDev::ExitSensPoint = ExitSensPoint;	 //указатель на выходной сенсор
	ArtConveyor1TypeNextExtDev::ExtDevReady = ExtDevReady;		 //указатель на следующее устройство
	productPassTime = PassTime;
	conveyorRunTimer = 0;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
}

void ArtConveyor1TypeNextExtDev::doLogic()
{
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1)
	{
		ArtIOClass::Error(0);
	}
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
		ArtIOClass::Error(m_id);
	}

	if (productFctEnterConveyor)
	{
		productEnterSensConvey = true;
		productFctEnterConveyor = false;
	}
	else
	{
		productEnterSensConvey = EnterSensPoint->SensorState();
	}

	productExitSensConvey = ExitSensPoint->SensorState();

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (ConveyorGetReadyReceive() == 1)
		{
			conveyorState = ST_CONVEYOR_FREE;
		}
		else
		{
			if (productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_BUSY;
			}
			else
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}

		break;
	}
	case ST_CONVEYOR_FREE:
	{
		if ((ArtIOClass::getInputState(ExtDevReady) != true) && (productExitSensConvey))
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}

		if (productEnterSensConvey)
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
		}
		break;
	}

	case ST_CONVEYOR_PROD_FWD:
	{
		if (conveyorRunTimer == 0)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!* !Change to actual function
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_FREE;
		}
		else
		{
			if ((ArtIOClass::getInputState(ExtDevReady) != 1) && productExitSensConvey)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
				conveyorRunTimer = 0;
				conveyorState = ST_CONVEYOR_BUSY;
			}
		}
		break;
	}
	case ST_CONVEYOR_BUSY:
	{
		if (ArtIOClass::getInputState(ExtDevReady) == 1)
		{
			ConveyorSetProdEntering();
			conveyorState = ST_CONVEYOR_PROD_FWD;
			conveyorRunTimer = 0;
		}
		else
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
		}
		break;
	}
	case ST_CONVEYOR_ERROR:
	{
		if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1) //; !* !Change ARTActuatorsGet to actual function
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
		}
		break;
	}
	default:
		break;
	}
}

float k = 0.9; // коэффициент фильтрации, 0.0-1.0
// бегущее среднее
static int filVal = 0;

int ArtConveyorShuttleType::expRunningAverage(int newVal)
{
	filVal += (newVal - filVal) * k;
	return filVal;
}

int ArtConveyorShuttleType::findMedianN_optim(int newVal)
{
	static int buffer[50]; // статический буфер
	static byte count = 0;
	buffer[count] = newVal;
	if ((count < 50 - 1) and (buffer[count] > buffer[count + 1]))
	{
		for (int i = count; i < 50 - 1; i++)
		{
			if (buffer[i] > buffer[i + 1])
			{
				float buff = buffer[i];
				buffer[i] = buffer[i + 1];
				buffer[i + 1] = buff;
			}
		}
	}
	else
	{
		if ((count > 0) and (buffer[count - 1] > buffer[count]))
		{
			for (int i = count; i > 0; i--)
			{
				if (buffer[i] < buffer[i - 1])
				{
					float buff = buffer[i];
					buffer[i] = buffer[i - 1];
					buffer[i - 1] = buff;
				}
			}
		}
	}
	if (++count >= 50)
		count = 0;
	return buffer[(int)50 / 2];
}

//---------------------------------ArtConveyor1TypeNextExtDev--------------------------------------------------

//---------------------------------ArtPalletConveyorWithStoppers--------------------------------------------------

ArtPalletConveyorWithStoppers::ArtPalletConveyorWithStoppers(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtPalletConveyorWithStoppers::ArtPalletConveyorWithStoppers(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *Pos1Ptr, ArtSensor *Pos2Ptr, ArtSensor *Pos3Ptr, ArtSensor *Pos4Ptr, ArtCylinder *StopperPos1, ArtCylinder *StopperPos2, ArtCylinder *StopperPos3, ArtCylinder *StopperPos4, int PassTime, int RunTimer, int ExtDevReady) : ArtPalletConveyorWithStoppers(id, name)
{
	conveyorType = type;
	ArtPalletConveyorWithStoppers::Pos1Ptr = Pos1Ptr; //указатель на драйвер
	ArtPalletConveyorWithStoppers::Pos2Ptr = Pos2Ptr; //указатель на входной сенсор
	ArtPalletConveyorWithStoppers::Pos3Ptr = Pos3Ptr; //указатель на выходной сенсор
	ArtPalletConveyorWithStoppers::Pos4Ptr = Pos4Ptr; //указатель на следующее устройство
	ArtPalletConveyorWithStoppers::StopperPos1 = StopperPos1;
	ArtPalletConveyorWithStoppers::StopperPos2 = StopperPos2;
	ArtPalletConveyorWithStoppers::StopperPos3 = StopperPos3;
	ArtPalletConveyorWithStoppers::StopperPos4 = StopperPos4;
	ArtPalletConveyorWithStoppers::ActPoint = ActPoint;
	conveyorRunTimer = 0;
	Pos1Sens = false;
	Pos2Sens = false;
	Pos3Sens = false;
	Pos4Sens = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
}

void ArtPalletConveyorWithStoppers::doLogic() //на переменную flags приходит состояние диспенсера надо распарсить
{
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1)
	{
		ArtIOClass::Error(0);
	}
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
		ArtIOClass::Error(m_id);
	}

	Pos1Sens = Pos1Ptr->SensorState();
	Pos2Sens = Pos2Ptr->SensorState();
	Pos3Sens = Pos3Ptr->SensorState();
	Pos4Sens = Pos4Ptr->SensorState();
	Conv1Free = ArtIOClass::readySignalFromNext(1, 1);
	Conv2Free = ArtIOClass::readySignalFromNext(2, 1);
	Conv3Free = ArtIOClass::readySignalFromNext(1, 2);
	Conv4Free = ArtIOClass::readySignalFromNext(2, 2);
	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (ConveyorGetReadyReceive() == 1 && (!Pos1Sens && !Pos2Sens && !Pos3Sens && !Pos4Sens))
		{
			conveyorState = ST_CONVEYOR_FREE;
		}
		else
		{
			if (productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_BUSY;
			}
			else
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}
		break;
	}
	case ST_CONVEYOR_FREE:
	{
		if (Conv1Free)
		{
			StopperPos1->ARTCylinderOpen();
		}
		if (!Pos1Sens)
		{
		}

		if (!Pos4Sens)
		{
			StopperPos4->ARTCylinderOpen();

			if (Pos3Sens)
			{
				StopperPos3->ARTCylinderClose();

				if (Pos2Sens)
				{
					StopperPos2->ARTCylinderOpen();
				}
				else
				{
					StopperPos2->ARTCylinderClose();
				}

				if (Pos1Sens)
				{
					StopperPos1->ARTCylinderOpen();
				}
				else
				{
					StopperPos1->ARTCylinderOpen();
				}

				while ((!StopperPos4->getCylState() == ArtCylinder::ARTCYL_ST_OPEN) && (!StopperPos1->getCylState() == ArtCylinder::ARTCYL_ST_OPEN) && ((!StopperPos2->getCylState() == ArtCylinder::ARTCYL_ST_OPEN) || (!StopperPos2->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)) && (!StopperPos3->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED))
				{
					break;
				}
				ActPoint->ConveySetDriverFWD(true);
			}
			else
			{
				if (Pos4Sens)
				{
					StopperPos3->ARTCylinderClose();
					if (Pos2Sens)
					{
						StopperPos2->ARTCylinderClose();
					}
					else
					{
						StopperPos2->ARTCylinderClose();
						StopperPos1->ARTCylinderClose();
					}
				}
			}
		}
		else
		{
			StopperPos4->ARTCylinderOpen();
			StopperPos3->ARTCylinderOpen();
		}

		if ((ArtIOClass::getInputState(ExtDevReady) != true) && (productExitSensConvey))
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}

		if (productEnterSensConvey)
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
		}
		break;
	}

	case ST_CONVEYOR_PROD_FWD:
	{
		if (conveyorRunTimer == 0)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!* !Change to actual function
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_FREE;
		}
		else
		{
			if ((ArtIOClass::getInputState(ExtDevReady) != 1) && productExitSensConvey)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
				conveyorRunTimer = 0;
				conveyorState = ST_CONVEYOR_BUSY;
			}
		}
		break;
	}
	case ST_CONVEYOR_BUSY:
	{
		if (ArtIOClass::getInputState(ExtDevReady) == 1)
		{
			ConveyorSetProdEntering();
			conveyorState = ST_CONVEYOR_PROD_FWD;
			conveyorRunTimer = 0;
		}
		else
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
		}
		break;
	}
	case ST_CONVEYOR_ERROR:
	{
		if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1) //; !* !Change ARTActuatorsGet to actual function
		{

			conveyorState = ST_CONVEYOR_UNKNOWN;
		}
		break;
	}
	default:
		break;
	}
}

//---------------------------------ArtConveyor1TypeNextExtDev--------------------------------------------------

//---------------------------------ArtConveyor--------------------------------------------------

int ArtConveyor::getName()
{
	return (0);
}

int ArtConveyor::getID()
{
	return (m_id);
}

void ArtConveyor::update()
{
}

void ArtConveyor::doLogic()
{
}