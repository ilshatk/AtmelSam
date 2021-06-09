#include "Conveyor\ArtConveyor.h"

ArtConveyor1Type::ArtConveyor1Type(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyor1Type::ArtConveyor1Type(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2, ArtSensor *point3, ArtConveyor2Type *point4, int PassTime, int RunTimer) : ArtConveyor1Type(id, name)
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

void ArtConveyor1Type::doLogic()
{
	//int retVal;
	/*if (ActuatorsGet(GET_CONV_ACTUATOR_READY,ActPoint) == 1)
	{
		conveyorState = ST_CONVEYOR_ERROR;
	}*/
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
			conveyorRunTimer = ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!* !Change to actual function
		}

		if (productEnterSensConvey)
		{
			conveyorRunTimer = ARTTimerGetTime();
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
	/*case ST_CONVEYOR_CONT_MANUAL:
    {
      if (nconveyorExtSTOP)
      {
        retVal = ActuatorsSet( SET_CONV_ACTUATOR_STOP); //!* !Change to actual function
      }

      if (nconveyorExtFWD)
      {
        retVal = ActuatorsSet(SET_CONV_ACTUATOR_FWD); //!* !Change to actual function
      }

      if (nconveyorExtREV)
      {
        retVal = ActuatorsSet(SET_CONV_ACTUATOR_REV); //!* !Change to actual function
      }
	  break;
    }*/
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
		return (pointer->ARTConveySetRun());
		pointer->bDriverRUN = true;
		pointer->bDriverDirection = false;
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

//-----------------------2 type-----------------------------------------

ArtConveyor2Type::ArtConveyor2Type(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
}

ArtConveyor2Type::ArtConveyor2Type(int id, const char name[], ConveyorType type, ArtDriver *act2, ArtSensor *entersens, ArtSensor *exitsens, int PassTime, int NumProdInConveyor, int InSensAllowTime) : ArtConveyor2Type(id, name)
{
	productPassTime = PassTime;
	productInSensAllowTime = InSensAllowTime;
	m_id = id;
	conveyorType = type;
	ActPoint2 = act2;			 //указатель на драйвер
	EnterSensPoint2 = entersens; //указатель на входной сенсор
	ExitSensPoint2 = exitsens;	 //указатель на выходной(считающий) сенсор
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
		productEnterSensConvey = EnterSensPoint2->SensorState();
	}

	productExitSensConvey = ExitSensPoint2->SensorState();
	productContSensConvey = ExitSensPoint2->SensorState();

	if ((ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint2)) == 1) // ; TODO: Change ARTActuatorsGet to actual function
	{
		if (false /*isManualMode*/)
		{
			conveyorState = ST_CONVEYOR_CONT_MANUAL;
		}
		else
		{
			if (productContSensConvey)
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
					conveyorRunTimer = ARTTimerGetTime();
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
			conveyorRunTimer = ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint2); //!*! Change to actual function
		}
		if (ARTTimerIsTimePassed(conveyorRunTimer, 90000, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
			conveyorState = ST_CONVEYOR_ERROR;
		}
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
			conveyorRunTimer = ARTTimerGetTime();
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
		}
		if (ARTTimerIsTimePassed(conveyorRunTimer, (int)(productPassTime * 1.2), 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
		}

		pp_stack_ready = false;
		break;
	}

	case ST_CONVEYOR_STACK_READY:
	{
		if (ftook_product)
		{

			/*if (productContSensConvey || productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_ERROR;
			}
			else
			{*/

			CalcedNumProdInConveyor = 0;
			ftook_product = false;
			conveyorState = ST_CONVEYOR_FREE;
			//}
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
	ftook_product = true;
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

ArtConveyor1EType::ArtConveyor1EType(int id, const char name[], ConveyorType type, ArtDriver *Act3, ArtSensor *Sens3_Enter, ArtSensor *Sens3_Exit, ArtConveyor1Type *Next3_Conv, int PassTime, int RunTimer) : ArtConveyor1EType(id, name)
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

		if (/*nconveyorExtModeEnabled*/(DigitalIn() & int(pow(2, 15))) == pow(2, 15) && (!/*isManualMode*/ false))
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
			conveyorRunTimer = ARTTimerGetTime();
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
		if (/*nconveyorExtSTOP*/(DigitalIn() & int(pow(2, 13))) == pow(2, 13))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint3);
		}
		if (/*nconveyorExtFWD*/(DigitalIn() & int(pow(2, 14)) )== pow(2, 14))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint3);
		}
		if (/*nconveyorExtREV*/false)
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
		_time = ARTTimerGetTime();
		productDeltaPassTime = _time - conveyorRunTimer;
	}
}

void ArtConveyor1EType::ConvReturnStateAndTime()
{
	float _time;
	conveyorState = nconveyorLastState;

	if (conveyorState == ST_CONVEYOR_PROD_FWD)
	{
		_time = ARTTimerGetTime();
		productDeltaPassTime = _time - productDeltaPassTime;
	}
}