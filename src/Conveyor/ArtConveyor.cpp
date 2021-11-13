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

	if (conveyorType == CONVEYOR_TYPE_CROSS)
	{
		if (conveyorState == ST_CONVEYOR_FREE)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (conveyorType == CONVEYOR_TYPE_1_EXTERNAL_BARDA)
	{
		if (conveyorState == ST_CONVEYOR_FREE || conveyorState == ST_CONVEYOR_PROD_FWD)
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

	if (conveyorType == CONVEYOR_TYPE_1_EXTE)
	{
		if ((conveyorState == ST_CONVEYOR_PROD_FWD) || (conveyorState == ST_CONVEYOR_FREE))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (conveyorType == CONVEYOR_TYPE_WITH_PUSHER)
	{
		if ((conveyorState == ST_CONVEYOR_MOVE_IN))
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

	case SET_CONV_ACTUATOR_REV: // TODO dont relase reverse.
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

ArtConveyor1Type::ArtConveyor1Type(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2,
								   ArtSensor *point3, ArtBasicConveyor *point4, int PassTime, int RunTimer, int DevNum) : ArtConveyor1Type(id, name)
{
	conveyorType = type;
	ActPoint = point;		 //указатель на драйвер
	ArtConveyor1Type::EnterSensPoint = point2; //указатель на входной сенсор
	ArtConveyor1Type::ExitSensPoint = point3;	 //указатель на выходной сенсор
	ArtConveyor1Type::NextConvPoint = point4;	 //указатель на следующий конвейер
	productPassTime = PassTime;
	conveyorRunTimer = 0;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	SetedProdNumberCollect = 0;
	CalcedNumProdInConveyor = 0;
	ArtConveyor1Type::DevNum = DevNum;
}

void ArtConveyor1Type::doLogic()
{
	ArtIOClass::ConvState(conveyorState, DevNum);

	// int retVal;
	if ((conveyorState == ST_CONVEYOR_FREE) || (conveyorState == ST_CONVEYOR_PROD_FWD))
	{
		// ArtIOClass::ConvReady(1);//раскоментить если нужен сигнал на предыдущий конвейер
	}
	else
	{
		// ArtIOClass::ConvReady(0); //раскоментить если нужен сигнал на предыдущий конвейер
	}
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) != 1)
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

		if ((NextConvPoint->ConveyorGetReadyReceive() == 1) && (productExitSensConvey))
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
			conveyorRunTimer = 0;
		}

		if (productEnterSensConvey) 
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
			conveyorRunTimer = 0;
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
			conveyorRunTimer = 0;
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_FREE;
		}
		else
		{
			if ((NextConvPoint->ConveyorGetReadyReceive() != 1) && (productExitSensConvey))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
				conveyorRunTimer = 0;
				conveyorState = ST_CONVEYOR_BUSY;
			}
			else
			{

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
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
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

ArtConveyor2Type::ArtConveyor2Type(int id, const char name[], ConveyorType type, ArtDriver *act2, ArtSensor *EnterSensPtr, ArtSensor *Perep,
								   ArtSensor *CountSensPtr, ArtSensor *ExitSensPtr, int PassTime, int NumProdInConveyor, int InSensAllowTime, int DevNum) : ArtConveyor2Type(id, name)
{
	productPassTime = PassTime;
	productInSensAllowTime = InSensAllowTime;
	m_id = id;
	conveyorType = type;
	ActPoint2 = act2;							   //указатель на драйвер
	ArtConveyor2Type::EnterSensPtr = EnterSensPtr; //указатель на входной сенсор
	ArtConveyor2Type::ExitSensPtr = ExitSensPtr;
	ArtConveyor2Type::CountSensPtr = CountSensPtr; //указатель на считающий сенсор
	ArtConveyor2Type::Perep = Perep;
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
	ArtConveyor2Type::DevNum = DevNum;
	ArtConveyor2Type::error = 0;
	enterSensTimer = 0;
	entersensFlag = false;
}

void ArtConveyor2Type::doLogic()
{
	if (ArtIOClass::SetProdNumberCollect())
	{
		SetedProdNumberCollect = ArtIOClass::SetProdNumberCollect();
	}

	if (SetedProdNumberCollect == 0)
	{
		return;
	}

	if (ArtIOClass::BoxCountSet())
	{
		CalcedNumProdInConveyor = ArtIOClass::BoxCountSet();
	}

	if (ArtIOClass::ResetDrv(ActPoint2->ResetSignalOut)) //сброс ошибки
	{
		if (!productCountSensConvey && !productExitSensConvey)
		{
			CalcedNumProdInConveyor = 0;
		}

		ArtIOClass::StackReady(false);
		conveyorState = ST_CONVEYOR_UNKNOWN;
		ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
	}

	if ((CalcedNumProdInConveyor == SetedProdNumberCollect) && (!Perep->SensorState()) && (productExitSensConvey))
	{
		ArtIOClass::GaveStack(false);
		conveyorState = ST_CONVEYOR_BUSY;
		// conveyorRunTimer = 0;
		pp_stack_ready = true;
		ArtIOClass::StackReady(true);
	}

	ArtIOClass::ConvState(conveyorState, DevNum);
	ArtIOClass::BoxQnt(CalcedNumProdInConveyor);

	if (productFctEnterConveyor)
	{
		productEnterSensConvey = true;
		productFctEnterConveyor = false;
	}
	else
	{
		productEnterSensConvey = EnterSensPtr->SensorState();
	}

	if (!productEnterSensConvey)
	{
		enterSensTimer = 0;
		entersensFlag = false;
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
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
		error = 1;
	}

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		/*if (ArtIOClass::boxqnt == (-1)) // количество коробов которое осталось не указано
		{
			return;
		}*/

		if (productEnterSensConvey)
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
		else
		{
			conveyorState = ST_CONVEYOR_FREE;
		}

		/*	if (!productEnterSensConvey || !productExitSensConvey || !productCountSensConvey)
			{
				conveyorState = ST_CONVEYOR_FREE;
			}*/
		break;
	}
	case ST_CONVEYOR_FREE:
	{
		if (productEnterSensConvey || CountSensPtr->SensorState())
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint2); //!*! Change to actual function
		}

		/*if (productEnterSensConvey && !entersensFlag)
		{
			enterSensTimer = ArtIOClass::ARTTimerGetTime();
		}*/

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
															 /*conveyorState = ST_CONVEYOR_ERROR;
															 error = 8;*/
		}

		/*if (ARTTimerIsTimePassed(enterSensTimer, 5000, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
			conveyorState = ST_CONVEYOR_ERROR;
		}*/

		if (CalcedNumProdInConveyor == SetedProdNumberCollect && (!Perep->SensorState() || productExitSensConvey))
		{
			ArtIOClass::GaveStack(false);
			conveyorState = ST_CONVEYOR_BUSY;
			conveyorRunTimer = 0;
			pp_stack_ready = true;
			ArtIOClass::StackReady(true);
		}
		else
		{
			if (CalcedNumProdInConveyor == SetedProdNumberCollect && !Perep->SensorState())
			{
				conveyorState = ST_CONVEYOR_ERROR;
			}

			if (!CountSensPtr->SensorState() && !productEnterSensConvey) // (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
			{
				if (ARTTimerIsTimePassed(conveyorRunTimer, (productPassTime - 10500), 99000))
				{
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
					conveyorRunTimer = 0;
				}
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

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
		}

		if (pp_stack_ready)
		{
			ftook_product = false;
			conveyorState = ST_CONVEYOR_STACK_READY;
			pp_stack_ready = false;
		}
		break;
	}

	case ST_CONVEYOR_STACK_READY:
	{
		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2); //!*! Change to actual function
		}
		ArtConveyorTookProduct();
		if (ftook_product)
		{
			if (productCountSensConvey || productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_ERROR;
				error = 8;
			}
			else
			{
				CalcedNumProdInConveyor = 0;
				ftook_product = false;
				conveyorState = ST_CONVEYOR_FREE;
				ArtIOClass::StackReady(false); // добавить переменную для стек реди
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
		QntFlag = true;
		ArtIOClass::StackReady(false);
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint2);	  //!*! Change to actual function

		if (ArtIOClass::ResetDrv(ActPoint2->ResetSignalOut)) //сброс ошибки
		{
			if (!productCountSensConvey && !productExitSensConvey)
			{
				CalcedNumProdInConveyor = 0;
			}
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
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

ArtConveyor1EType::ArtConveyor1EType(int id, const char name[], ConveyorType type, ArtDriver *Act3, ArtSensor *Sens3_Enter, ArtSensor *Sens3_Exit, ArtBasicConveyor *Next3_Conv, int PassTime, int RunTimer, int DevNum) : ArtConveyor1EType(id, name)
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
	ArtConveyor1EType::DevNum = DevNum;
	ArtConveyor1EType::error = 0;
}

void ArtConveyor1EType::doLogic()
{
	// ArtIOClass::ConvState(conveyorState);
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
			// DebugMsgNotify("conveyorState[ %1 ] == ST_RUN","ARTConveyorType1EDoSPS",conveyorID)
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
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита

		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut)) //сброс ошибки
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
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
			// ActuatorsSet(SET_CONV_ACTUATOR_REV, ActPoint3);
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

ArtConveyor1AType::ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2, ArtSensor *point3, ArtBasicConveyor *point4, int PassTime, int RunTimer, int SetedProdNumberCollect, int DevNum) : ArtConveyor1AType(id, name)
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
	ArtConveyor1AType::DevNum = DevNum;
	ArtConveyor1AType::error = 0;
}

ArtConveyor1AType::ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2, ArtSensor *point3, bool readySignalFromNextBarda, int PassTime, int RunTimer, int SetedProdNumberCollect, int DevNum) : ArtConveyor1AType(id, name)
{
	conveyorType = type;
	ArtConveyor1AType::DevNum = DevNum;
	ActPoint = point;														//указатель на драйвер
	EnterSensPoint = point2;												//указатель на входной сенсор
	ExitSensPoint = point3;													//указатель на выходной сенсор
	ArtConveyor1AType::readySignalFromNextBarda = readySignalFromNextBarda; //указатель на следующий конвейер
	productPassTime = PassTime;												//таймер на конвейер
	conveyorRunTimer = 0;													//
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtBasicConveyor::SetedProdNumberCollect = SetedProdNumberCollect;
	CalcedNumProdInConveyor = 0;
	ArtConveyor1AType::error = 0;
}

void ArtConveyor1AType::doLogic()
{

	//	ArtIOClass::ConvState(conveyorState,DevNum);
	AccumConvOn = ArtIOClass::ExtDevReady(1);			//если 1 бит флага true значит конвейер накопительный
	if (conveyorType == CONVEYOR_TYPE_1_EXTERNAL_BARDA) // если следующий конвейер на другой барде
	{
		readySignalFromNextBarda = ArtIOClass::readySignalFromNext(); //сигнал готовности со следующего конвейера
	}

	if (productFctEnterConveyor)
	{
		productEnterSensConvey = true;
		productFctEnterConveyor = false;
	}
	else
	{
		productEnterSensConvey = EnterSensPoint->SensorState(); //сигнал со входного датчика
	}

	productExitSensConvey = ExitSensPoint->SensorState(); //сигнал с выходного датчика

	if (AccumConvOn) //если конвейер накопительный
	{
		productCountSensConvey = EnterSensPoint->SensorState(); //сигнал со считающего датчика

		if ((ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint)) == 1)
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
					error = 10;
				}
			}
		}
		else
		{
			conveyorState = ST_CONVEYOR_ERROR;
			error = 1;
		}

		switch (conveyorState)
		{
		case ST_CONVEYOR_UNKNOWN:
		{
			if (productEnterSensConvey || productExitSensConvey)
			{
				if (conveyorRunTimer != 0) // если таймер не обнулён, тогда обнуляй
				{
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //останавливаем конвейер
					conveyorRunTimer = 0;
				}
				else
				{
					if (conveyorRunTimer == 0)
					{
						conveyorRunTimer = ArtIOClass::ARTTimerGetTime(); //приравниваем таймер к текущему времени
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);	  //запускаем конвейер
						conveyorState = ST_CONVEYOR_FREE;
					}
					if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000)) //если
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

			if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!*! Change to actual function
				conveyorState = ST_CONVEYOR_ERROR;
				error = 1;
			}

			if (CalcedNumProdInConveyor == SetedProdNumberCollect)
			{
				conveyorState = ST_CONVEYOR_BUSY;
				conveyorRunTimer = 0;
				pp_stack_ready = true; //стек готов
									   // ArtConveyorSetStackReady(conveyorID);
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

			if (pp_stack_ready) //если стек готов
			{
				// ftook_product = false;
				conveyorState = ST_CONVEYOR_STACK_READY;
				pp_stack_ready = false;
			}

			// pp_stack_ready = false;
			break;
		}

		case ST_CONVEYOR_STACK_READY:
		{
			if (!productExitSensConvey)
			{
				ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
			}
			else
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
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

			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			if (ARTTimerIsTimePassed(conveyorRunTimer, (int)(productPassTime * 1.2), 99000))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			}
			break;
		}

		case ST_CONVEYOR_ERROR:
		{
			//	ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита

			if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut)) //сброс ошибки
			{
				conveyorState = ST_CONVEYOR_UNKNOWN;
				// ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
			}
			break;
		}
		default:
			break;
		}
	}
	else // если конвейер проходной
	{
		if ((ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint)) == 1)
		{
		}
		else
		{
			conveyorState = ST_CONVEYOR_ERROR;
			error = 1;
		}
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
					// ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
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
				if ((!readySignalFromNextBarda) && productExitSensConvey)
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
			if (readySignalFromNextBarda)
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
			ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита

			if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut)) //сброс ошибки
			{
				conveyorState = ST_CONVEYOR_UNKNOWN;
				ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
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

ArtConveyorShuttleType::ArtConveyorShuttleType(int id, const char name[], ConveyorType type, ArtDriver *ShuttlePtr, ArtDriver *OverShuttlePtr, ArtAnalogSensor *PositionSens,
											   ArtSensor *PalletOnConv, ArtSensor *SensOnIN, ArtSensor *SensOnOUT, ArtBasicConveyor *NextConvPtr, ArtSensor *ShuttleEdge, ArtSensor *ExitSensPoint, bool readySignalFromNextBarda, int BrakeOUT, int PassTimeOverShuttle, int PassTimeShuttle,
											   int ConveyorRunTimerShuttle, int ConveyorRunTimerOverShuttle, int Range, int DevNum) : ArtConveyorShuttleType(id, name)
{
	ArtConveyorShuttleType::ExitSensPoint = ExitSensPoint;
	conveyorType = type;
	ArtConveyorShuttleType::ShuttlePtr = ShuttlePtr;					  //указатель на драйвер шаттла
	ArtConveyorShuttleType::OverShuttlePtr = OverShuttlePtr;			  //указатель на драйвер конвейера над шаттлом
	ArtConveyorShuttleType::PositionSens = PositionSens;				  //указатель на аналоговый датчик
	ArtConveyorShuttleType::SensOnIN = SensOnIN;						  //указатель на датчик на входе шаттла
	ArtConveyorShuttleType::SensOnOUT = SensOnOUT;						  //указатель на датчик на выходе шаттла
	ArtConveyorShuttleType::ShuttleEdge = ShuttleEdge;					  //указатель на датчик крайних положений шаттла(если сработал, то значит вышел за раб. зону)
	ArtConveyorShuttleType::PalletOnConv = PalletOnConv;				  //указатель на датчик паллеты
	ArtConveyorShuttleType::NextConvPtr = NextConvPtr;					  //указатель на следующий конвейер
	ArtConveyorShuttleType::productPassOverShuttle = PassTimeOverShuttle; //таймер для верхнего конвейера
	ArtConveyorShuttleType::PassTimeShuttle = PassTimeShuttle;			  //таймер для шаттла
	ArtConveyorShuttleType::BrakeOUT = BrakeOUT;						  //выход на тормоз
	ArtConveyorShuttleType::CurPos = PositionSens->SensorState();		  //Текущее местоположение
	conveyorState = ST_CONVEYOR_UNKNOWN;
	On_Position = false;
	ArtConveyorShuttleType::ConveyorRunTimerShuttle = ConveyorRunTimerShuttle;		   // таймер шаттла
	ArtConveyorShuttleType::ConveyorRunTimerOverShuttle = ConveyorRunTimerOverShuttle; // таймер конвейера над шаттлом
	ArtConveyorShuttleType::Range = Range;											   // диапазон позиционирования
	ArtConveyorShuttleType::DevNum = DevNum;
	ArtConveyorShuttleType::error = 0;
	ReqPos = expRunningAverage(((31 * CurPos + PositionSens->SensorState()) >> 5));

	// ArtConveyorShuttleType::signalfornext = false;
}

void ArtConveyorShuttleType::doLogic()
{
	if (ArtIOClass::ResetDrv(OverShuttlePtr->ResetSignalOut))
	{
		ConveyorRunTimerOverShuttle = 0;
		ConveyorRunTimerShuttle = 0;
		conveyorState = ST_CONVEYOR_UNKNOWN;
	}

	ArtIOClass::ConvState(conveyorState, DevNum);
	CurPos = expRunningAverage(((31 * CurPos + PositionSens->SensorState()) >> 5)); //текущее положение пропущенное через 2 фильтра

	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ShuttlePtr) == 1) // частотник готов
	{
	}
	else
	{
		conveyorState = ST_SHUTTLE_CONVEYOR_ERROR;
	}
	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (!ArtIOClass::PosSetted()) // позиция не задана
		{
			return;
		}
		conveyorState = ST_CONVEYOR_POS_SELECT;
		break;
	}

	case ST_CONVEYOR_RUN:
	{
		signalfornext = false;
		On_Position = false;			 // не на позиции
		if (!ShuttleEdge->SensorState()) // если датчик между PLP и шаттлом не сработал
		{
			if (ConveyorRunTimerShuttle == 0)
			{
				ConveyorRunTimerShuttle = ArtIOClass::ARTTimerGetTime();
			}

			if ((CurPos - ReqPos) > Range || (CurPos - ReqPos) < (-Range)) //если шаттл находится на позицие с диапазоном range
			{
				ArtIOClass::setOutputState(BrakeOUT, true); // отпускаем тормоз
				if ((CurPos - ReqPos) < (-Range))
				{
					if ((CurPos - ReqPos) <= (-10000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr); //запускаю конвейер вперед
						WriteDacValues(0, 26000);						 //функция для задания частоты на частотнике 32600 - максимальная частота
					}

					if ((CurPos - ReqPos) <= (-9000) && (CurPos - ReqPos) > (-10000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 24000);
					}

					if ((CurPos - ReqPos) <= (-8000) && (CurPos - ReqPos) > (-9000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 23000);
					}

					if ((CurPos - ReqPos) <= (-7000) && (CurPos - ReqPos) > (-8000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 21000);
					}

					if ((CurPos - ReqPos) <= (-5000) && (CurPos - ReqPos) > (-6000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 19000);
					}

					if ((CurPos - ReqPos) <= (-4500) && (CurPos - ReqPos) > (-5000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 17000);
					}

					if ((CurPos - ReqPos) <= (-4000) && (CurPos - ReqPos) > (-4500))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 15000);
					}

					if ((CurPos - ReqPos) <= (-3500) && (CurPos - ReqPos) > (-4000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 13000);
					}

					if ((CurPos - ReqPos) <= (-3000) && (CurPos - ReqPos) > (-3500))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 11000);
					}

					if ((CurPos - ReqPos) <= (-2500) && (CurPos - ReqPos) > (-3000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 10000);
					}

					if ((CurPos - ReqPos) <= (-1000) && (CurPos - ReqPos) > (-2500))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 8000);
					}

					if ((CurPos - ReqPos) <= (-900) && (CurPos - ReqPos) > (-1000))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 7000);
					}

					if ((CurPos - ReqPos) <= (-800) && (CurPos - ReqPos) > (-900))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 6000);
					}

					if ((CurPos - ReqPos) <= (-700) && (CurPos - ReqPos) > (-800))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 5000);
					}

					if ((CurPos - ReqPos) <= (-600) && (CurPos - ReqPos) > (-700))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 4000);
					}

					if ((CurPos - ReqPos) <= (-600) && (CurPos - ReqPos) > (-700))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 3000);
					}

					if ((CurPos - ReqPos) <= (-500) && (CurPos - ReqPos) > (-600))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 2500);
					}

					if ((CurPos - ReqPos) <= (-150) && (CurPos - ReqPos) > (-500))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 2000);
					}

					if ((CurPos - ReqPos) < (-Range) && (CurPos - ReqPos) > (-150))
					{
						ActuatorsSet(SET_CONV_ACTUATOR_FWD, ShuttlePtr);
						WriteDacValues(0, 1500);
					}
				}
				else
				{
					if ((CurPos - ReqPos) > Range)
					{
						if ((CurPos - ReqPos) >= 10000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 26000); // 32600
						}

						if ((CurPos - ReqPos) >= 9000 && (CurPos - ReqPos) < 10000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 24000);
						}

						if ((CurPos - ReqPos) >= 8000 && (CurPos - ReqPos) < 9000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 23000);
						}

						if ((CurPos - ReqPos) >= 7000 && (CurPos - ReqPos) < 8000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 21000);
						}

						if ((CurPos - ReqPos) >= 6000 && (CurPos - ReqPos) < 7000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 19000);
						}

						if ((CurPos - ReqPos) >= 5000 && (CurPos - ReqPos) < 6000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 17000);
						}

						if ((CurPos - ReqPos) >= 4000 && (CurPos - ReqPos) < 5000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 15000);
						}

						if ((CurPos - ReqPos) >= 3500 && (CurPos - ReqPos) < 4000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 13000);
						}

						if ((CurPos - ReqPos) >= 3000 && (CurPos - ReqPos) < 3500)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 11000);
						}

						if ((CurPos - ReqPos) >= 2500 && (CurPos - ReqPos) < 3000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 9000);
						}

						if ((CurPos - ReqPos) >= 2000 && (CurPos - ReqPos) < 2500)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 8000);
						}

						if ((CurPos - ReqPos) >= 1500 && (CurPos - ReqPos) < 2000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 7000);
						}

						if ((CurPos - ReqPos) >= 1000 && (CurPos - ReqPos) < 1500)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 6000);
						}

						if ((CurPos - ReqPos) >= 900 && (CurPos - ReqPos) < 1000)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 5000);
						}

						if ((CurPos - ReqPos) >= 800 && (CurPos - ReqPos) < 900)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 4000);
						}

						if ((CurPos - ReqPos) >= 700 && (CurPos - ReqPos) < 800)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 3500);
						}

						if ((CurPos - ReqPos) >= 600 && (CurPos - ReqPos) < 700)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 3000);
						}

						if ((CurPos - ReqPos) >= 500 && (CurPos - ReqPos) < 600)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 2500);
						}

						if ((CurPos - ReqPos) >= 150 && (CurPos - ReqPos) < 500)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 2000);
						}

						if ((CurPos - ReqPos) > Range && (CurPos - ReqPos) < 150)
						{
							ActuatorsSet(SET_CONV_ACTUATOR_REV, ShuttlePtr);
							WriteDacValues(0, 1500);
						}
					}
				}

				if (ARTTimerIsTimePassed(ConveyorRunTimerShuttle, PassTimeShuttle, 99000))
				{
					WriteDacValues(0, 0);
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr); //останавливаем конвейер
					ArtIOClass::setOutputState(BrakeOUT, false);	  //зажимаем тормоз
					conveyorState = ST_SHUTTLE_CONVEYOR_ERROR;
					error = 12;
				}
			}
			else
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
				ArtIOClass::setOutputState(BrakeOUT, false);
			}
		}
		else
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
			ArtIOClass::setOutputState(BrakeOUT, false);
			conveyorState = ST_SHUTTLE_CONVEYOR_ERROR;

			break;
		}

		if ((CurPos - ReqPos) <= Range && (CurPos - ReqPos) >= (-Range) && !PalletOnConv->SensorState())
		{
			WriteDacValues(0, 0);
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
			ConveyorRunTimerShuttle = 0;
			conveyorState = ST_CONVEYOR_AT_THE_POINT;
			ArtIOClass::setOutputState(BrakeOUT, false);
		}

		if ((CurPos - ReqPos) <= Range && (CurPos - ReqPos) >= (-Range) && PalletOnConv->SensorState() && ReqPos == ArtIOClass::ReqPosition(5))
		{
			WriteDacValues(0, 0);
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
			ConveyorRunTimerShuttle = 0;
			conveyorState = ST_CONVEYOR_MOVE_OUT;
			ArtIOClass::setOutputState(BrakeOUT, false);
		}
		break;
	}

	case ST_CONVEYOR_AT_THE_POINT:
	{
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
		if ((CurPos - ReqPos) <= Range && (CurPos - ReqPos) >= (-Range))
		{
			ArtIOClass::setOutputState(BrakeOUT, false); //зажать тормоз

			if (PalletOnConv->SensorState() == false && (ReqPos == ArtIOClass::ReqPosition(1) || ReqPos == ArtIOClass::ReqPosition(2) || ReqPos == ArtIOClass::ReqPosition(3) || ReqPos == ArtIOClass::ReqPosition(4))) // если нету паллеты на верхнем конвейере и необходимая позиция - это один из плейспоинтов
			{
				conveyorState = ST_CONVEYOR_MOVE_IN;
			}

			if (ARTTimerIsTimePassed(ConveyorRunTimerOverShuttle, productPassOverShuttle, 99000))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr); //!*! Change to actual function
				ConveyorRunTimerOverShuttle = 0;
				conveyorState = ST_OVERSHUTTLE_CONVEYOR_ERROR;
				error = 12;
			}
		}
		else
		{
			if ((!SensOnIN->SensorState() && !SensOnOUT->SensorState())) //если нет паллеты между шаттлом и конвейером
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr); // если паллета заехала на шаттл и все ок можно ехать на след. позицию
				conveyorState = ST_CONVEYOR_RUN;					  //можно ехать на след. позицию
			}
			else
			{
				conveyorState = ST_OVERSHUTTLE_CONVEYOR_ERROR;
				error = 13; // ошибка паллета между шаттлом и конвейером
			}
		}
		break;
	}

	case ST_CONVEYOR_MOVE_IN:
	{
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
		ArtIOClass::setOutputState(BrakeOUT, false);

		if (ReqPos == ArtIOClass::ReqPosition(1)) //если необходимая позиция == заданной
		{										  //
			ArtIOClass::ShuttlePosition(1, true); //отправляем информацию о позиции шаттла
		}

		if (ReqPos == ArtIOClass::ReqPosition(2)) //если необходимая позиция == заданной
		{										  //
			ArtIOClass::ShuttlePosition(2, true); //отправляем информацию о позиции шаттла
		}

		if (ReqPos == ArtIOClass::ReqPosition(3)) //если необходимая позиция == заданной
		{										  //
			ArtIOClass::ShuttlePosition(4, true); //отправляем информацию о позиции шаттла
		}										  //

		if (ReqPos == ArtIOClass::ReqPosition(4)) //если необходимая позиция == заданной
		{										  //
			ArtIOClass::ShuttlePosition(8, true); //отправляем информацию о позиции шаттла
		}

		if (ConveyorRunTimerOverShuttle == 0)
		{
			ConveyorRunTimerOverShuttle = ArtIOClass::ARTTimerGetTime(); //
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, OverShuttlePtr);		 //
		}

		if (PalletOnConv->SensorState())
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr);

			if (!SensOnIN->SensorState() && !SensOnOUT->SensorState())
			{
				ArtIOClass::ShuttlePosition(0, false);
				conveyorState = ST_CONVEYOR_POS_SELECT;
			}
			else
			{
				conveyorState = ST_OVERSHUTTLE_CONVEYOR_ERROR;
			}
		}

		if (ARTTimerIsTimePassed(ConveyorRunTimerOverShuttle, productPassOverShuttle, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr); //!*! Change to actual function
			ConveyorRunTimerOverShuttle = 0;
			conveyorState = ST_OVERSHUTTLE_CONVEYOR_ERROR;
			error = 12;
		}

		break;
	}

	case ST_CONVEYOR_MOVE_OUT:
	{
		ArtIOClass::setOutputState(BrakeOUT, false);
		if (NextConvPtr->ConveyorGetReadyReceive() && !ExitSensPoint->SensorState())
		{
			ConveyorRunTimerOverShuttle = ArtIOClass::ARTTimerGetTime(); //крутим верхний конвейер
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, OverShuttlePtr);
		}

		if (!NextConvPtr->ConveyorGetReadyReceive() && ExitSensPoint->SensorState())
		{
			if (PalletOnConv->SensorState() == false && ReqPos == ArtIOClass::ReqPosition(5)) //паллета не на конвейере и шаттл находится на позиции выгрузки
			{
				if (!SensOnOUT->SensorState())
				{
					ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr);
					ConveyorRunTimerOverShuttle = 0;
					conveyorState = ST_CONVEYOR_POS_SELECT;
				}
				else
				{
					conveyorState = ST_SHUTTLE_CONVEYOR_ERROR;
				}
			}
		}

		break;
	}

	case ST_CONVEYOR_POS_SELECT:
	{
		ArtIOClass::ShuttlePosition(0, false);
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr);
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);

		if (PalletOnConv->SensorState() == false) // если нет паллеты на шаттле
		{
			if (ArtIOClass::PLPReady(1, 1)) //готовность 1 PLP с 1 барды
			{
				ReqPos = ArtIOClass::ReqPosition(1);
				conveyorState = ST_CONVEYOR_RUN;
				break;
			}

			if (ArtIOClass::PLPReady(2, 1)) //готовность 2 PLP с 1 барды
			{
				ReqPos = ArtIOClass::ReqPosition(2);
				conveyorState = ST_CONVEYOR_RUN;
				break;
			}

			if (ArtIOClass::PLPReady(4, 1)) //готовность 3 PLP со 2 барды
			{
				ReqPos = ArtIOClass::ReqPosition(3);
				conveyorState = ST_CONVEYOR_RUN;
				break;
			}

			if (ArtIOClass::PLPReady(8, 1)) //готовность 4 PLP со 2 барды
			{
				ReqPos = ArtIOClass::ReqPosition(4);
				conveyorState = ST_CONVEYOR_RUN;
				break;
			}
		}
		else //есть паллета - на выгрузку 5 позиция выгрузка
		{
			ReqPos = ArtIOClass::ReqPosition(5);
			conveyorState = ST_CONVEYOR_RUN;
		}
		break;
	}

	case ST_SHUTTLE_CONVEYOR_ERROR:
	{
		signalfornext = false;
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr); // если ошибка то остановим конвейер и зажмем тормоз
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
		ArtIOClass::setOutputState(BrakeOUT, false);

		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита

		if (ArtIOClass::ResetDrv(OverShuttlePtr->ResetSignalOut)) //сброс ошибки
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}
		break;
	}

	case ST_OVERSHUTTLE_CONVEYOR_ERROR:
	{
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ShuttlePtr);
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, OverShuttlePtr);
		ArtIOClass::setOutputState(BrakeOUT, false);

		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита

		if (ArtIOClass::ResetDrv(OverShuttlePtr->ResetSignalOut)) //сброс ошибки
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false); // перестаем передавать ошибку
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

ArtConveyor1TypeNextExtDev::ArtConveyor1TypeNextExtDev(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, int PassTime, int RunTimer, int BitNum, int DevNum) : ArtConveyor1TypeNextExtDev(id, name)
{
	conveyorType = type;
	ArtConveyor1TypeNextExtDev::ActPoint = ActPoint;			 //указатель на драйвер
	ArtConveyor1TypeNextExtDev::EnterSensPoint = EnterSensPoint; //указатель на входной сенсор
	ArtConveyor1TypeNextExtDev::ExitSensPoint = ExitSensPoint;	 //указатель на выходной сенсор
	productPassTime = PassTime;
	conveyorRunTimer = 0;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtConveyor1TypeNextExtDev::BitNum = BitNum; // номер бита который передается в переменную Flags
	ArtConveyor1TypeNextExtDev::DevNum = DevNum;
	ArtConveyor1TypeNextExtDev::signalExt = false;
}

void ArtConveyor1TypeNextExtDev::doLogic()
{
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1)
	{
	}
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
		error = 1;
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
		if (!ArtIOClass::ExtDevReady(pow(2, BitNum)) && productExitSensConvey) // ждем сигнал с внешнего устройства
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}

		if (productEnterSensConvey || signalExt)
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
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
		}
		else
		{
			if (/*!ArtIOClass::ExtDevReady(pow(2, BitNum)) &&*/ productExitSensConvey) //если внешнее устройство не готово и сработал датчик на выходе
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
				conveyorRunTimer = 0;
				conveyorState = ST_CONVEYOR_BUSY;
			}
		}
		break;
	}

	case ST_CONVEYOR_BUSY:
	{
		if (!productExitSensConvey)
		{
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_FREE;
		}

		if (ArtIOClass::ExtDevReady(pow(2, BitNum))) // если внешнее устр-во гоово то отправляй паллету
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
			conveyorRunTimer = 0;
		}
		else
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			conveyorRunTimer = 0;
		}
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
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

int ArtConveyorShuttleType::expRunningAverage(int newVal) // фильтр для аналогового сигнала
{
	filVal += (newVal - filVal) * k;
	return filVal;
}

int ArtConveyorShuttleType::findMedianN_optim(int newVal) // фильтр для аналогового сигнала
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

ArtPalletConveyorWithStoppers::ArtPalletConveyorWithStoppers(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtDriver *DispDrvPtr, ArtSensor *Pos1Ptr,
															 ArtSensor *Pos2Ptr, ArtSensor *Pos3Ptr, ArtSensor *Pos4Ptr, ArtCylinder *StopperPos1, ArtCylinder *StopperPos2,
															 ArtCylinder *StopperPos3, ArtCylinder *StopperPos4, int PassTime, int RunTimer, ArtSensor *Lift1Down,
															 ArtSensor *Lift2Down, ArtSensor *Lift3Down, ArtSensor *Lift4Down, int DevNum) : ArtPalletConveyorWithStoppers(id, name)
{
	conveyorType = type;
	ArtPalletConveyorWithStoppers::Pos1Ptr = Pos1Ptr;		  //указатель на драйвер
	ArtPalletConveyorWithStoppers::Pos2Ptr = Pos2Ptr;		  //указатель на входной сенсор
	ArtPalletConveyorWithStoppers::Pos3Ptr = Pos3Ptr;		  //указатель на выходной сенсор
	ArtPalletConveyorWithStoppers::Pos4Ptr = Pos4Ptr;		  //указатель на следующее устройство
	ArtPalletConveyorWithStoppers::StopperPos1 = StopperPos1; // указатель на цилиндр 1 стоппера
	ArtPalletConveyorWithStoppers::StopperPos2 = StopperPos2; // указатель на цилиндр 2 стоппера
	ArtPalletConveyorWithStoppers::StopperPos3 = StopperPos3; // указатель на цилиндр 3 стоппера
	ArtPalletConveyorWithStoppers::StopperPos4 = StopperPos4; // указатель на цилиндр 4 стоппера
	ArtPalletConveyorWithStoppers::ActPoint = ActPoint;		  // указатель на драйвер цепного конвейера
	ArtPalletConveyorWithStoppers::DispDrvPtr = DispDrvPtr;	  //указатель на драйвер конвйера диспенсера
	ArtPalletConveyorWithStoppers::Lift1Down = Lift1Down;	  // указатель на датчик лифта
	ArtPalletConveyorWithStoppers::Lift2Down = Lift2Down;
	ArtPalletConveyorWithStoppers::Lift3Down = Lift3Down;
	ArtPalletConveyorWithStoppers::Lift4Down = Lift4Down;
	conveyorRunTimer = 0;
	Pos1Sens = false;
	Pos2Sens = false;
	Pos3Sens = false;
	Pos4Sens = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtPalletConveyorWithStoppers::DevNum = DevNum;
	ArtPalletConveyorWithStoppers::error = 0;
	Conv1Free = false;
	Conv2Free = false;
	Conv3Free = false;
	Conv4Free = false;
}

void ArtPalletConveyorWithStoppers::doLogic() //на переменную flags приходит состояние диспенсера надо распарсить
{
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1)
	{
	}
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
	}

	Pos1Sens = Pos1Ptr->SensorState();				   //сенсор первого стоппера
	Pos2Sens = Pos2Ptr->SensorState();				   //
	Pos3Sens = Pos3Ptr->SensorState();				   //
	Pos4Sens = Pos4Ptr->SensorState();				   //
	Conv1Free = ArtIOClass::readySignalFromNext(1, 1); //сигнал готовности с 1 PLP
	Conv2Free = ArtIOClass::readySignalFromNext(2, 1); //сигнал готовности со 2 PLP
	Conv3Free = ArtIOClass::readySignalFromNext(4, 2); //сигнал готовности с 3 PLP
	Conv4Free = ArtIOClass::readySignalFromNext(8, 2); //сигнал готовности с 4 PLP

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (Conv1Free)
		{
			conveyorState = ST_1_POS;
			return;
		}

		if (Conv2Free)
		{
			conveyorState = ST_2_POS;
			return;
		}

		if (Conv3Free) // ConveyorGetReadyReceive() == 1 &&
		{
			conveyorState = ST_3_POS;
			return;
		}

		if (Conv4Free)
		{
			conveyorState = ST_4_POS;
			return;
		}
		conveyorState = ST_1_POS;
		break;
	}

	case ST_1_POS:
	{
		ArtIOClass::DevReady(4, false);
		if (!Pos1Sens && Lift1Down->SensorState()) // Lift1Down->SensorState() - эта функция возвращает значение датчика на 1 входе на A38 барде(M24 внизу)
		{
			ArtIOClass::DevReady(1, false);
			StopperPos1->ARTCylinderOpen();
			if (ArtIOClass::ExtDevReady(1) && ((StopperPos1->getCylState())))
			{
				ActPoint->ConveySetDriverFWD(true); //запуск цепного конвейера
				DispDrvPtr->ConveySetDriverFWD(true);
			}
		}
		else
		{
			if (Pos1Sens)
			{
				ActPoint->ConveySetSTOP();
				DispDrvPtr->ConveySetSTOP();

				if (Conv1Free && !(ActPoint->ARTDriverGetFWD()))
				{
					StopperPos1->ARTCylinderClose();
					ArtIOClass::DevReady(1, true); // передаем сигнал лифту на 1 позиции подняться
				}

				if (!Conv1Free && !(ActPoint->ARTDriverGetFWD()))
				{
					conveyorState = ST_CONVEYOR_UNKNOWN;
					StopperPos1->ARTCylinderClose();
				}
			}
		}
		break;
	}

	case ST_2_POS:
	{
		ArtIOClass::DevReady(1, false);
		if (!Pos2Sens && Lift2Down->SensorState()) // Lift2Down->SensorState() - эта функция возвращает значение датчика на 3 входе на A38 барде(M25 внизу)
		{
			StopperPos2->ARTCylinderOpen();
			if (ArtIOClass::ExtDevReady(1) && (StopperPos2->getCylState()))
			{
				ActPoint->ConveySetDriverFWD(true); // запуск цепного конвейера
				DispDrvPtr->ConveySetDriverFWD(true);
			}
			if (Pos1Sens)
			{
				DispDrvPtr->ConveySetDriverFWD(false);
			}
		}
		else
		{
			if (Pos2Sens)
			{
				ActPoint->ConveySetSTOP();
				DispDrvPtr->ConveySetSTOP();

				if (Conv2Free && !(ActPoint->ARTDriverGetFWD()))
				{
					StopperPos2->ARTCylinderClose();
					ArtIOClass::DevReady(2, true);
					//поднимаем мини-конвейер и передаем паллету плейспоинту 2
				}

				if (!Conv2Free && !(ActPoint->ARTDriverGetFWD()))
				{
					conveyorState = ST_CONVEYOR_UNKNOWN;
				}
			}
		}
		break;
	}

	case ST_3_POS:
	{
		ArtIOClass::DevReady(2, false);
		ArtIOClass::DevReady(1, false);
		if (!Pos3Sens && Lift3Down->SensorState()) // Lift3Down->SensorState() - эта функция возвращает значение датчика на 5 входе на A38 барде (M26 внизу)
		{
			StopperPos3->ARTCylinderOpen();

			if (ArtIOClass::ExtDevReady(1) && (StopperPos3->getCylState()))
			{
				ActPoint->ConveySetDriverFWD(true); // запуск цепного конвейера
				DispDrvPtr->ConveySetDriverFWD(true);
			}

			DispDrvPtr->ConveySetDriverFWD(true);
		}
		else
		{
			if (Pos3Sens)
			{
				ActPoint->ConveySetSTOP();
				DispDrvPtr->ConveySetSTOP();

				if (Conv3Free && !(ActPoint->ARTDriverGetFWD()))
				{
					StopperPos3->ARTCylinderClose();
					ArtIOClass::DevReady(3, true);
					//поднимаем мини-конвейер и передаем паллету пикпоинту 2
				}

				if (!Conv3Free && !(ActPoint->ARTDriverGetFWD()))
				{
					conveyorState = ST_CONVEYOR_UNKNOWN;
				}
			}
		}
		break;
	}

	case ST_4_POS:
	{
		ArtIOClass::DevReady(3, false);
		ArtIOClass::DevReady(1, false);
		ArtIOClass::DevReady(2, false);
		if (!Pos4Sens && Lift4Down->SensorState()) // Lift4Down->SensorState() - эта функция возвращает значение датчика на 7 входе на A38 барде (M27 внизу)
		{
			StopperPos4->ARTCylinderOpen();

			if (ArtIOClass::ExtDevReady(1) && (StopperPos4->getCylState() == ArtCylinder::ARTCYL_ST_OPEN))
			{
				ActPoint->ConveySetDriverFWD(true);	  // запуск цепного конвейера
				DispDrvPtr->ConveySetDriverFWD(true); // запуск конвейера диспенсера
			}
		}
		else
		{
			if (Pos4Sens)
			{
				ActPoint->ConveySetSTOP();
				DispDrvPtr->ConveySetSTOP();

				if (Conv4Free && !(ActPoint->ARTDriverGetFWD()))
				{
					StopperPos4->ARTCylinderClose();
					ArtIOClass::DevReady(4, true);
					//поднимаем мини-конвейер и передаем паллету пикпоинту 2
				}

				if (!Conv4Free && !(ActPoint->ARTDriverGetFWD()))
				{
					conveyorState = ST_CONVEYOR_UNKNOWN;
				}
			}
		}
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}
		break;
	}

	default:
		break;
	}
}

//---------------------------------ArtConveyor1TypeNextExtDev--------------------------------------------------
//---------------------------------ArtConveyorPLPType--------------------------------------------------
ArtConveyorPLPType::ArtConveyorPLPType(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyorPLPType::ArtConveyorPLPType(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint,
									   ArtSensor *ExitSensPoint, ArtSensor *PallOnPosition, ArtSensor *LayerSensor, ArtCylinder *Stopper, ArtCylinder *Podzhim,
									   int PassTime, int RunTimer, int PLPNum, int DevNum) : ArtConveyorPLPType(id, name)
{
	ArtConveyorPLPType::conveyorType = type;
	ArtConveyorPLPType::ActPoint = ActPoint;			 //указатель на драйвер
	ArtConveyorPLPType::EnterSensPoint = EnterSensPoint; //указатель на входной сенсор
	ArtConveyorPLPType::ExitSensPoint = ExitSensPoint;	 //указатель на выходной сенсор датчик между шаттлом и PLP
	ArtConveyorPLPType::PallOnPosition = PallOnPosition; //указатель на паллетный сенсор
	ArtConveyorPLPType::LayerSensor = LayerSensor;		 //указатель на сенсор слоя
	ArtConveyorPLPType::productPassTime = PassTime;
	ArtConveyorPLPType::conveyorRunTimer = RunTimer;
	ArtConveyorPLPType::PLPNum = PLPNum; //номер PLP
	ArtConveyorPLPType::Stopper = Stopper;
	ArtConveyorPLPType::Podzhim = Podzhim;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	PalletOnPosition = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtConveyorPLPType::DevNum = DevNum;
	ArtConveyorPLPType::error = 0;
}

void ArtConveyorPLPType::doLogic()
{
	if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut)) //сброс ошибки частотника
	{
		conveyorState = ST_CONVEYOR_UNKNOWN;
	}

	ArtIOClass::ConvState(conveyorState, DevNum);

	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) != 1) //состояние драйвера
	{
		conveyorState = ST_CONVEYOR_ERROR;
		error = 1;
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

	PalletOnPosition = PallOnPosition->SensorState();	  //сенсор паллеты на конце PLP
	productExitSensConvey = ExitSensPoint->SensorState(); //сенсор между шаттлом и PLP

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (PalletOnPosition)
		{
			if (/*LayerSensor->SensorState() */ ArtIOClass::PallFull(pow(2, PLPNum - 1))) //ЗАКОМЕНЧЕНО ДЛЯ ТЕСТОВ
			{
				conveyorState = ST_CONVEYOR_BUSY;
			}
			else
			{
				conveyorState = ST_CONVEYOR_PALL_PLACED;
			}
		}
		else
		{
			conveyorState = ST_CONVEYOR_NEEDPALLET;
		}

		break;
	}

	case ST_CONVEYOR_NEEDPALLET:
	{
		if (PalletOnPosition)
		{
			if (1 /*!LayerSensor->SensorState()*/)
			{
				conveyorState = ST_CONVEYOR_PALL_PLACED;
			}
			else
			{
				conveyorState = ST_CONVEYOR_BUSY;
			}
			ArtIOClass::NeedPal(PLPNum, false); // сигнал "нужна паллета" с PLP
		}
		else
		{
			ArtIOClass::NeedPal(PLPNum, true); // сигнал "нужна паллета" с PLP
		}

		if (productEnterSensConvey && !PalletOnPosition)
		{
			conveyorState = ST_CONVEYOR_MOVE_IN;
			conveyorRunTimer = 0;
		}
		break;
	}

	case ST_CONVEYOR_MOVE_IN:
	{
		Stopper->ARTCylinderOpen(); // поднять стоппер

		if (conveyorRunTimer == 0 && Stopper->getCylState()) // ждем пока поднимется стоппер
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //!* !Change to actual function
		}

		if (PalletOnPosition) // если паллета на позиции то вырубай движок
		{
			ArtIOClass::NeedPal(PLPNum, false);
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_UNFIXED;
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
		}
		break;
	}

	case ST_CONVEYOR_UNFIXED:
	{
		Podzhim->ARTCylinderOpen();														   // поджимаем паллету
		if ((!Podzhim->cylOpenIn->SensorState()) && (!Podzhim->cylCloseIn->SensorState())) // ждем пока подожмет
		{																				   //
			conveyorState = ST_CONVEYOR_PALL_PLACED;									   // паллета зажата и готов для заполнения
		}
		else
		{
			if ((Podzhim->cylOpenIn->SensorState()) && (!Podzhim->cylCloseIn->SensorState()))
			{
				conveyorState = ST_CONVEYOR_ERROR;
			}
		}
		break;
	}

	case ST_CONVEYOR_PALL_PLACED:
	{
		ArtIOClass::DevReady(PLPNum, true); //отправляем сигнал о том что этот плейспоинт готов для заполнения

		if (ArtIOClass::PallFull(pow(2, PLPNum - 1)) /*LayerSensor->SensorState()*/) // ЗАКОМЕНЧЕНО ДЛЯ ТЕСТОВ как только датчик слоя увидел продукт переход в BUSY
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}
		break;
	}

	case ST_CONVEYOR_BUSY:
	{
		ArtIOClass::DevReady(PLPNum, false);		  //перестаем отправлять сигнал о том что этот плейспоинт готов для заполнения
		if (ArtIOClass::PallFull(pow(2, PLPNum - 1))) //если пришел сигнал с робота о том что паллета заполнена тогда отпускаем поджим
		{
			Podzhim->ARTCylinderClose();
			Stopper->ARTCylinderClose(); // отпускаем поджим

			if (Podzhim->cylCloseIn->SensorState() && Stopper->cylCloseIn->SensorState()) //если поджим и стоппер отпущен тогда начинаем выгонять паллету
			{
				if (ArtIOClass::PallFull(pow(2, PLPNum - 1)) && ArtIOClass::readySignalFromNext(pow(2, PLPNum - 1), 2))
				{
					if (1 /*LayerSensor->SensorState()*/) // есть продукт на паллете ЗАКОМЕНЧЕНО ДЛЯ ТЕСТОВ
					{
						conveyorRunTimer = 0;
						conveyorState = ST_CONVEYOR_MOVE_OUT; // переходим в состояние выгона паллеты
					}
					else
					{
						conveyorState = ST_CONVEYOR_ERROR; // ошибка потому что паллета полная , а датчик слоя ничего не видит
						error = 8;
					}
				}
			}
		}
		break;
	}

	case ST_CONVEYOR_MOVE_OUT:
	{
		ArtIOClass::PLPReady(pow(2, PLPNum - 1), true);
		if (ArtIOClass::readySignalFromNext(pow(2, PLPNum - 1), 2) && !m_ptrEasyCat->BufferOut.Cust.SensSignalFromPrevBarda & 3 && conveyorRunTimer == 0) // если шаттл на необходимой позиции и готов принимать тогда отправляем
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
		}

		if (!PalletOnPosition && !ArtIOClass::readySignalFromNext(pow(2, PLPNum - 1), 2)) // паллеты нет на плейспоинте
		{
			// if (!LayerSensor->SensorState()) ЗАКОМЕНТИРОВАНО ДЛЯ ТЕСТОВ
			//{

			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			conveyorState = ST_CONVEYOR_NEEDPALLET;
			//}
			/*else
			{
				conveyorState = ST_CONVEYOR_ERROR;
				error = 8;
			}*/
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			error = 1;
		}

		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
		ArtIOClass::NeedPal(PLPNum, false);
		conveyorRunTimer = 0;
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}
		break;
	}

	default:
		break;
	}
}

//---------------------------------ArtConveyorPLPType--------------------------------------------------

//---------------------------------ArtConveyorWithLift лифт с двигателем--------------------------------------------------

ArtConveyorWithLift::ArtConveyorWithLift(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyorWithLift::ArtConveyorWithLift(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtDriver *LiftDrv, ArtSensor *EnterSensPoint,
										 ArtSensor *ExitSensPoint, ArtSensor *LiftUP, ArtSensor *LiftDOWN, ArtSensor *NextConvEnd,
										 int PassTime, int LiftUpTime, int LiftDOWNTime, int RunTimer, int DevNum) : ArtConveyorWithLift(id, name)
{
	ArtConveyorWithLift::conveyorType = type;
	ArtConveyorWithLift::ActPoint = ActPoint;			//указатель на драйвер
	ArtConveyorWithLift::LiftDrv = LiftDrv;				//указатель на драйвер
	ArtConveyorWithLift::LiftUP = LiftUP;				//лифт вверху
	ArtConveyorWithLift::LiftUpTime = LiftUpTime;		//время подъема
	ArtConveyorWithLift::LiftDOWNTime = LiftDOWNTime;	// время опускания
	ArtConveyorWithLift::LiftDOWN = LiftDOWN;			//лифт внизу
	ArtConveyorWithLift::LiftDOWN = EnterSensPoint;		//указатель на входной сенсор
	ArtConveyorWithLift::ExitSensPoint = ExitSensPoint; //указатель на выходной сенсор
	ArtConveyorWithLift::NextConvEnd = NextConvEnd;		//указатель на сенсор следующего конвейера
	ArtConveyorWithLift::productPassTime = PassTime;
	ArtConveyorWithLift::conveyorRunTimer = RunTimer;
	ArtConveyorWithLift::DevNum = DevNum;

	productEnterSensConvey = false;
	productExitSensConvey = false;
	conveyorLiftRunTimer = 0;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtConveyorWithLift::error = 0;
}

void ArtConveyorWithLift::doLogic()
{
	ArtIOClass::ConvState(conveyorState, DevNum);
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) != 1)
	{
		conveyorState = ST_CONVEYOR_ERROR;
		error = 1;
	}

	productEnterSensConvey = EnterSensPoint->SensorState();
	productExitSensConvey = ExitSensPoint->SensorState();

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (productEnterSensConvey)
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
		}

		if (productExitSensConvey)
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}
		else
		{
			conveyorState = ST_CONVEYOR_FREE;
		}
		break;
	}

	case ST_CONVEYOR_FREE:
	{
		if (productEnterSensConvey && LiftUP->SensorState())
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
		}
		else
		{
			conveyorState = ST_CONVEYOR_LIFT_UP;
		}
		break;
	}

	case ST_CONVEYOR_PROD_FWD:
	{
		if (conveyorRunTimer == 0)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime(); //получаем текущее время
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
		}

		if (productExitSensConvey)
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_LIFT_DOWN;
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000)) // если крутится больше productPassTime то вырубай и в ошибку
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			error = 1;
		}

		break;
	}

	case ST_CONVEYOR_LIFT_UP:
	{
		if (conveyorLiftRunTimer == 0)
		{
			if (LiftDOWN->SensorState() == true)
			{
				conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
				ActuatorsSet(SET_CONV_ACTUATOR_FWD, LiftDrv);
			}
		}

		if (ARTTimerIsTimePassed(conveyorLiftRunTimer, LiftUpTime, 99000)) // если крутится больше LiftUpTime то вырубай и в ошибку
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, LiftDrv); //!* !Change to actual function
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			error = 5;
		}

		if (LiftUP->SensorState() == true)
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, LiftDrv);
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_FREE;
		}
		break;
	}

	case ST_CONVEYOR_LIFT_DOWN:
	{
		if (conveyorLiftRunTimer == 0)
		{
			if (LiftUP->SensorState() == true)
			{
				conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
				ActuatorsSet(SET_CONV_ACTUATOR_REV, LiftDrv);
			}
		}

		if (ARTTimerIsTimePassed(conveyorLiftRunTimer, LiftDOWNTime, 99000)) // если крутится больше LiftDOWNTime то вырубай и в ошибку
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, LiftDrv); //!* !Change to actual function
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			error = 5;
		}

		if (LiftDOWN->SensorState() == true)
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, LiftDrv);
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_BUSY;
		}
		break;
	}

	case ST_CONVEYOR_BUSY:
	{
		if (NextConvEnd->SensorState() == true)
		{
			conveyorState = ST_CONVEYOR_LIFT_UP; // надо переделать если следующий встанет по ошибке зависнет
		}
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}
		break;
	}

	default:
		break;
	}
}

//---------------------------------ArtConveyorWithLift лифт с двигателем--------------------------------------------------

//-----------------------------------------------ArtConveyorWithLiftType1 лифт с цилиндром----------------------------------------------
ArtConveyorWithLiftType1::ArtConveyorWithLiftType1(int id, const char name[]) //
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyorWithLiftType1::ArtConveyorWithLiftType1(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtCylinder *Lift,
												   ArtSensor *LiftUP, ArtSensor *LiftDOWN, ArtSensor *NextConvEnd,
												   int PassTime, int LiftUpTime, int LiftDOWNTime, int RunTimer, int LiftNum, int DevNum) : ArtConveyorWithLiftType1(id, name)
{
	ArtConveyorWithLiftType1::conveyorType = type;
	ArtConveyorWithLiftType1::ActPoint = ActPoint;		   //указатель на драйвер
	ArtConveyorWithLiftType1::LiftUP = LiftUP;			   //лифт вверху
	ArtConveyorWithLiftType1::LiftDOWN = LiftDOWN;		   //лифт внизу
	ArtConveyorWithLiftType1::Lift = Lift;				   // указатель на цилиндр лифта
	ArtConveyorWithLiftType1::LiftUpTime = LiftUpTime;	   //время подъема
	ArtConveyorWithLiftType1::LiftDOWNTime = LiftDOWNTime; //время опускания
	ArtConveyorWithLiftType1::NextConvEnd = NextConvEnd;   //указатель на сенсор следующего конвейера
	ArtConveyorWithLiftType1::productPassTime = PassTime;
	ArtConveyorWithLiftType1::conveyorRunTimer = RunTimer;
	ArtConveyorWithLiftType1::LiftNum = LiftNum;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	conveyorLiftRunTimer = 0;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtConveyorWithLiftType1::DevNum = DevNum;
	ArtConveyorWithLiftType1::error = 0;
}

void ArtConveyorWithLiftType1::doLogic()
{
	ArtIOClass::ConvState(conveyorState, DevNum);
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) != 1)
	{
		conveyorState = ST_CONVEYOR_ERROR;
		error = 1;
	}

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (Lift->getCylState()) // если лифт наверху, то пусть опустится
		{
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_LIFT_DOWN;
		}
		else
		{
			conveyorState = ST_CONVEYOR_FREE;
		}
		break;
	}

	case ST_CONVEYOR_FREE:
	{
		if (ArtIOClass::ExtDevReady(pow(2, LiftNum - 1)) && !NextConvEnd->SensorState()) // если пришел сигнал с цепного на этот лифт тогда поднимай
		{
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_LIFT_UP;
		}

		break;
	}

	case ST_CONVEYOR_PROD_FWD:
	{
		if (conveyorRunTimer == 0)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint); //
		}

		if (!ArtIOClass::ExtDevReady(pow(2, LiftNum - 1)))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_LIFT_DOWN;
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000)) // если крутится больше productPassTime то вырубай и в ошибку
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			error = 1;
		}

		break;
	}

	case ST_CONVEYOR_LIFT_UP:
	{
		if (conveyorLiftRunTimer == 0 && !NextConvEnd->SensorState() && (ArtIOClass::ExtDevReady(pow(2, LiftNum - 1))))
		{
			conveyorLiftRunTimer = ArtIOClass::ARTTimerGetTime();
			Lift->ARTCylinderOpen();
		}
		else
		{
			conveyorState = ST_CONVEYOR_FREE;
		}

		if (ARTTimerIsTimePassed(conveyorLiftRunTimer, productPassTime, 99000)) // не успел открыться
		{
			Lift->ARTCylinderOFF(); // вырубаем цилиндр
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			error = 5;
		}

		while (!Lift->cylOpenIn->SensorState())
		{
			return;
		}

		if (!NextConvEnd->SensorState())
		{
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_PROD_FWD;
		}
		break;
	}

	case ST_CONVEYOR_LIFT_DOWN:
	{
		if (conveyorLiftRunTimer == 0)
		{
			conveyorLiftRunTimer = ArtIOClass::ARTTimerGetTime();
			Lift->ARTCylinderClose(); // опускаем лифт
		}

		if (ARTTimerIsTimePassed(conveyorLiftRunTimer, productPassTime, 99000)) //
		{
			Lift->ARTCylinderOFF();
			conveyorLiftRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			error = 5;
		}

		while (!Lift->cylCloseIn->SensorState()) // пока не опустился вылетаем
		{
			return;
		}

		conveyorState = ST_CONVEYOR_FREE;
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}
		break;
	}

	default:
		break;
	}
}
//------------------------------------------------------ArtConveyorWithLiftType1 лифт с цилиндром----------------------------------------

//-------------------------------------------ArtConveyorPalletPickPoint паллетный PP--------------------------------------------------
ArtConveyorPalletPickPoint::ArtConveyorPalletPickPoint(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyorPalletPickPoint::ArtConveyorPalletPickPoint(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *ConvEnter, ArtSensor *ConvEnd,
													   int PassTime, int RunTimer, int ButtonIn, int DevNum) : ArtConveyorPalletPickPoint(id, name)
{
	ArtConveyorPalletPickPoint::conveyorType = type;
	ArtConveyorPalletPickPoint::ActPoint = ActPoint;		//указатель на драйвер
	ArtConveyorPalletPickPoint::ButtonIn = ButtonIn;		// вход на котором сидит кнопка
	ArtConveyorPalletPickPoint::productPassTime = PassTime; // максимум сколько должен крутится конвейер до конечного датчика
	ArtConveyorPalletPickPoint::ConvEnd = ConvEnd;
	ArtConveyorPalletPickPoint::ConvEnter = ConvEnter;
	productEnterSensConvey = false;
	productExitSensConvey = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtConveyorPalletPickPoint::DevNum = DevNum;
	ArtConveyorPalletPickPoint::error = 0;
}

void ArtConveyorPalletPickPoint::doLogic()
{
	ArtIOClass::ConvState(conveyorState, DevNum);
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) != 1)
	{
		conveyorState = ST_CONVEYOR_ERROR;
		error = 1;
	}

	productEnterSensConvey = ConvEnter->SensorState();
	productExitSensConvey = ConvEnd->SensorState();

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		if (productExitSensConvey)
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}
		else
		{
			conveyorState = ST_CONVEYOR_FREE;
		}
		break;
	}

	case ST_CONVEYOR_FREE:
	{
		if (!productExitSensConvey)
		{
			if (productEnterSensConvey)
			{
				conveyorState = ST_CONVEYOR_PROD_FWD;
			}
		}
		else
		{
			conveyorState = ST_CONVEYOR_BUSY;
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

		if (productExitSensConvey)
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_BUSY;
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000)) // если крутится больше productPassTime то вырубай и в ошибку
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
		}

		break;
	}

	case ST_CONVEYOR_BUSY:
	{
		if (ArtIOClass::getInputState(ButtonIn))
		{
			if (!productExitSensConvey)
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}
		break;
	}

	default:
		break;
	}
}

ArtCrossConveyor::ArtCrossConveyor(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtCrossConveyor::ArtCrossConveyor(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtDriver *DispDrvPtr,
								   ArtBasicConveyor *NextConvPoint, ArtSensor *PosPtr, ArtCylinder *StopperPos,
								   int PassTime, int RunTimer,
								   ArtSensor *LiftDown, int DevNum, bool last) : ArtCrossConveyor(id, name)
{
	conveyorType = type;
	ArtCrossConveyor::PosPtr = PosPtr;		   //указатель на драйвер
	ArtCrossConveyor::StopperPos = StopperPos; // указатель на цилиндр 1 стоппера
	ArtCrossConveyor::ActPoint = ActPoint;	   // указатель на драйвер цепного конвейера
	ArtCrossConveyor::DispDrvPtr = DispDrvPtr; //указатель на драйвер конвйера диспенсера
	ArtCrossConveyor::NextConvPoint = NextConvPoint;
	ArtCrossConveyor::LiftDown = LiftDown;

	conveyorRunTimer = 0;
	Pos1Sens = false;
	Pos2Sens = false;
	Pos3Sens = false;
	Pos4Sens = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtCrossConveyor::DevNum = DevNum;
	ArtCrossConveyor::error = 0;
	Conv1Free = false;
	Conv2Free = false;
	Conv3Free = false;
	Conv4Free = false;
	ArtCrossConveyor::last = last;
}

ArtCrossConveyor::ArtCrossConveyor(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtDriver *DispDrvPtr,
								   ArtSensor *PosPtr, ArtCylinder *StopperPos, int PassTime, int RunTimer,
								   ArtSensor *LiftDown, int DevNum, bool last) : ArtCrossConveyor(id, name)
{
	conveyorType = type;
	ArtCrossConveyor::PosPtr = PosPtr;		   //указатель на драйвер
	ArtCrossConveyor::StopperPos = StopperPos; // указатель на цилиндр 1 стоппера
	ArtCrossConveyor::ActPoint = ActPoint;	   // указатель на драйвер цепного конвейера
	ArtCrossConveyor::DispDrvPtr = DispDrvPtr; //указатель на драйвер конвйера диспенсера
	ArtCrossConveyor::LiftDown = LiftDown;
	conveyorRunTimer = 0;
	Pos1Sens = false;
	Pos2Sens = false;
	Pos3Sens = false;
	Pos4Sens = false;
	productExitSensConvey = false;
	productFctEnterConveyor = false;
	conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtCrossConveyor::DevNum = DevNum;
	ArtCrossConveyor::error = 0;
	Conv1Free = false;
	Conv2Free = false;
	Conv3Free = false;
	Conv4Free = false;
	ArtCrossConveyor::last = last;
}

void ArtCrossConveyor::doLogic() //на переменную flags приходит состояние диспенсера
{
	if (!ArtIOClass::ExtDevReady(1))
	{
		DispDrvPtr->ConveySetDriverFWD(false);
	}

	if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))
	{
		conveyorState = ST_CONVEYOR_UNKNOWN;
	}

	ArtIOClass::ConvState(conveyorState, DevNum);
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) == 1)
	{
	}
	else
	{
		conveyorState = ST_CONVEYOR_ERROR;
	}

	Pos1Sens = PosPtr->SensorState(); //датчик паллеты

	if (DevNum < 3)
	{
		Conv1Free = ArtIOClass::readySignalFromNext(pow(2, DevNum - 1), 1); //сигнал готовности с 1 PLP
	}
	else
	{
		Conv1Free = ArtIOClass::readySignalFromNext(pow(2, DevNum - 1), 2); //сигнал готовности с 1 PLP
	}

	switch (conveyorState)
	{
	case ST_CONVEYOR_UNKNOWN:
	{
		ArtIOClass::DevReady(DevNum, false);
		StopperPos->ARTCylinderOpen();

		if (conveyorRunTimer == 0)
		{
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
		}
		if (ARTTimerIsTimePassed(conveyorRunTimer, 1200, 99000)) // если крутится больше productPassTime то вырубай и в ошибку
		{
			conveyorRunTimer = 0;
			if (!Pos1Sens)
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
			else
			{
				conveyorState = ST_CONVEYOR_BUSY;
			}
		}

		break;
	}

	case ST_CONVEYOR_FREE:
	{
		if (last)
		{
			StopperPos->ARTCylinderOpen();
		}
		ArtIOClass::DevReady(DevNum, false);
		if (DevNum == 1 && !last)
		{
			if (LiftDown->SensorState() && ArtIOClass::ExtDevReady(1))
			{
				StopperPos->ARTCylinderOpen();
				ActPoint->ConveySetDriverFWD(true);
				DispDrvPtr->ConveySetDriverFWD(true);
			}

			if (Pos1Sens)
			{
				conveyorState = ST_CONVEYOR_BUSY;
				DispDrvPtr->ConveySetDriverFWD(false);
				ActPoint->ConveySetDriverFWD(false);
			}

			if (!Conv1Free)
			{
				ArtIOClass::DevReady(DevNum, false);
			}
		}

		if (DevNum == 2 && !last)
		{
			if (Pos1Sens)
			{
				conveyorState = ST_CONVEYOR_BUSY;
				ActPoint->ConveySetDriverFWD(false);
			}
			if (LiftDown->SensorState())
			{
				StopperPos->ARTCylinderOpen();
				// ActPoint->ConveySetDriverFWD(true);
			}
			if (!Conv1Free)
			{
				ArtIOClass::DevReady(DevNum, false);
			}
		}

		if (DevNum == 3 && !last)
		{
			if (Pos1Sens)
			{
				conveyorState = ST_CONVEYOR_BUSY;
				ActPoint->ConveySetDriverFWD(false);
			}
			if (LiftDown->SensorState())
			{
				StopperPos->ARTCylinderOpen();
				// ActPoint->ConveySetDriverFWD(true);
			}
			if (!Conv1Free)
			{
				ArtIOClass::DevReady(DevNum, false);
			}
		}
		if (DevNum == 4 && last)
		{
			StopperPos->ARTCylinderOpen();

			if (Pos1Sens)
			{
				conveyorState = ST_CONVEYOR_BUSY;
				ActPoint->ConveySetDriverFWD(false);
			}

			if (LiftDown->SensorState())
			{
				StopperPos->ARTCylinderOpen();
				// ActPoint->ConveySetDriverFWD(true);
			}

			if (!Conv1Free)
			{
				ArtIOClass::DevReady(DevNum, false);
			}
		}
		break;
	}

	case ST_CONVEYOR_BUSY:
	{
		if (last)
		{
			StopperPos->ARTCylinderOpen();
		}

		ArtIOClass::DevReady(DevNum, false);
		if (DevNum == 1 && !last)
		{
			if (!ArtIOClass::ExtDevReady(1))
			{
				DispDrvPtr->ConveySetDriverFWD(false);
			}

			if (NextConvPoint->ConveyorGetReadyReceive())
			{
				conveyorState = ST_CONVEYOR_MOVE_OUT;
			}
			else
			{
				if (Conv1Free && Pos1Sens)
				{
					conveyorState = ST_CONVEYOR_PROD_FWD;
				}
			}
		}

		if (DevNum == 2 && !last)
		{
			if (NextConvPoint->ConveyorGetReadyReceive())
			{
				conveyorState = ST_CONVEYOR_MOVE_OUT;
			}
			else
			{
				if (Conv1Free && Pos1Sens)
				{
					conveyorState = ST_CONVEYOR_PROD_FWD;
				}
			}
		}

		if (DevNum == 3 && !last)
		{
			if (NextConvPoint->ConveyorGetReadyReceive())
			{
				conveyorState = ST_CONVEYOR_MOVE_OUT;
			}
			else
			{
				if (Conv1Free && Pos1Sens)
				{
					conveyorState = ST_CONVEYOR_PROD_FWD;
				}
			}
		}

		if (DevNum == 4 && last)
		{
			if (Conv1Free && Pos1Sens)
			{
				conveyorState = ST_CONVEYOR_PROD_FWD;
			}

			if (!Conv1Free && !LiftDown->SensorState())
			{
				ArtIOClass::DevReady(DevNum, false);
			}
		}
		break;
	}

	case ST_CONVEYOR_PROD_FWD:
	{
		if (DevNum == 1 && !last)
		{
			StopperPos->ARTCylinderClose();
			if (Conv1Free)
			{
				ArtIOClass::DevReady(DevNum, true);
			}
			else
			{
				ArtIOClass::DevReady(DevNum, false);
			}

			if (!Pos1Sens && LiftDown->SensorState() || !Conv1Free)
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}

		if (DevNum == 2 && !last)
		{
			StopperPos->ARTCylinderClose();
			if (Conv1Free)
			{
				ArtIOClass::DevReady(DevNum, true);
			}
			else
			{
				ArtIOClass::DevReady(DevNum, false);
			}

			if (!Pos1Sens && LiftDown->SensorState() || !Conv1Free)
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}

		if (DevNum == 3 && !last)
		{
			StopperPos->ARTCylinderClose();
			if (Conv1Free)
			{
				ArtIOClass::DevReady(DevNum, true);
			}
			else
			{
				ArtIOClass::DevReady(DevNum, false);
			}

			if (!Pos1Sens && LiftDown->SensorState() || !Conv1Free)
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}

		if (DevNum == 4 && last)
		{
			StopperPos->ARTCylinderClose();
			if (Conv1Free)
			{
				ArtIOClass::DevReady(DevNum, true);
			}
			else
			{
				ArtIOClass::DevReady(DevNum, false);
			}

			if (!Pos1Sens && LiftDown->SensorState() || !Conv1Free)
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}
		break;
	}

	case ST_CONVEYOR_MOVE_OUT:
	{
		if (last)
		{
			StopperPos->ARTCylinderOpen();
		}

		ArtIOClass::DevReady(DevNum, false);
		if (DevNum == 1)
		{
			if (!ArtIOClass::ExtDevReady(1))
			{
				DispDrvPtr->ConveySetDriverFWD(false);
			}
			ActPoint->ConveySetDriverFWD(true);
			StopperPos->ARTCylinderClose();
			if (!NextConvPoint->ConveyorGetReadyReceive())
			{
				StopperPos->ARTCylinderOpen();
				ActPoint->ConveySetDriverFWD(false);
				conveyorState = ST_CONVEYOR_FREE;
			}
		}

		if (DevNum == 2)
		{
			ActPoint->ConveySetDriverFWD(true);
			StopperPos->ARTCylinderClose();
			if (!NextConvPoint->ConveyorGetReadyReceive())
			{
				StopperPos->ARTCylinderOpen();
				ActPoint->ConveySetDriverFWD(false);
				conveyorState = ST_CONVEYOR_FREE;
			}
		}

		if (DevNum == 3)
		{
			ActPoint->ConveySetDriverFWD(true);
			StopperPos->ARTCylinderClose();
			if (!NextConvPoint->ConveyorGetReadyReceive())
			{
				StopperPos->ARTCylinderOpen();
				ActPoint->ConveySetDriverFWD(false);
				conveyorState = ST_CONVEYOR_FREE;
			}
		}
		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		DispDrvPtr->ConveySetDriverFWD(false);
		ActPoint->ConveySetDriverFWD(false);
		ArtIOClass::DevReady(DevNum, false);
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}
		break;
	}

	default:
		break;
	}
}

//-----------------------------------------ArtConveyorPalletPickPoint паллетный PP-----------------------------------------------------
// ArtConveyor Next pusher---------------------------------------
ArtConveyor1NextPushType::ArtConveyor1NextPushType(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyor1NextPushType::ArtConveyor1NextPushType(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2,
												   ArtSensor *point3, ArtSensor *Pusher, ArtBasicConveyor *point4, int PassTime, int RunTimer, int DevNum) : ArtConveyor1NextPushType(id, name)
{
	ArtConveyor1NextPushType::conveyorType = type;
	ArtConveyor1NextPushType::ActPoint = point;		   //указатель на драйвер
	ArtConveyor1NextPushType::EnterSensPoint = point2; //указатель на входной сенсор
	ArtConveyor1NextPushType::ExitSensPoint = point3;  //указатель на выходной сенсор
	ArtConveyor1NextPushType::NextConvPoint = point4;  //указатель на следующий конвейер
	ArtConveyor1NextPushType::PusherHome = Pusher;
	ArtConveyor1NextPushType::productPassTime = PassTime;
	ArtConveyor1NextPushType::conveyorRunTimer = 0;
	ArtConveyor1NextPushType::productEnterSensConvey = false;
	ArtConveyor1NextPushType::productExitSensConvey = false;
	ArtConveyor1NextPushType::productFctEnterConveyor = false;
	ArtConveyor1NextPushType::conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtConveyor1NextPushType::SetedProdNumberCollect = 0;
	ArtConveyor1NextPushType::CalcedNumProdInConveyor = 0;
	ArtConveyor1NextPushType::DevNum = DevNum;
}

void ArtConveyor1NextPushType::doLogic()
{
	ArtIOClass::ConvState(conveyorState, DevNum);
	if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut)) //сброс ошибки частотника
	{
		conveyorState == ST_CONVEYOR_UNKNOWN;
		conveyorRunTimer = 0;
	}
	// int retVal;
	if ((conveyorState == ST_CONVEYOR_FREE) || (conveyorState == ST_CONVEYOR_PROD_FWD))
	{
		// ArtIOClass::ConvReady(1);//раскоментить если нужен сигнал на предыдущий конвейер
	}
	else
	{
		// ArtIOClass::ConvReady(0); //раскоментить если нужен сигнал на предыдущий конвейер
	}
	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) != 1)
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
		if (((NextConvPoint->ConveyorGetReadyReceive() != 1) || !PusherHome->SensorState()) && (productExitSensConvey))
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}

		if ((NextConvPoint->ConveyorGetReadyReceive() == 1) && (productExitSensConvey) && PusherHome->SensorState())
		{
			conveyorState = ST_CONVEYOR_PROD_FWD;
		}

		if (productEnterSensConvey) //
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

		// if (PusherHome->SensorState() )
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
			if (((NextConvPoint->ConveyorGetReadyReceive() != 1) || !PusherHome->SensorState()) && productExitSensConvey)
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
		if (NextConvPoint->ConveyorGetReadyReceive() == 1 && PusherHome)
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
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}

		break;
	}
	default:
		break;
	}
}
// ArtConveyor Next Pusher --------------------------------------------------------
//--------------------------------------------ArtConveyorWithPusher----------------------------------------------------------------
ArtConveyorWithPusher::ArtConveyorWithPusher(int id, const char name[])
{
	IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtConveyorWithPusher::ArtConveyorWithPusher(int id, const char name[], ConveyorType type, ArtDriver *point, ArtSensor *point2,
											 ArtSensor *point3, ArtSensor *OnPusher, ArtCylinder *PusherCylPtr, ArtBasicConveyor *point4, int PassTime, int RunTimer, int DevNum) : ArtConveyorWithPusher(id, name)
{
	ArtConveyorWithPusher::conveyorType = type;
	ArtConveyorWithPusher::ActPoint = point;		//указатель на драйвер
	ArtConveyorWithPusher::EnterSensPoint = point2; //указатель на входной сенсор
	ArtConveyorWithPusher::ExitSensPoint = point3;	//указатель на выходной сенсор
	ArtConveyorWithPusher::NextConvPoint = point4;	//указатель на следующий конвейер
	ArtConveyorWithPusher::OnPusher = OnPusher;
	ArtConveyorWithPusher::PusherCylPtr = PusherCylPtr;
	productPassTime = PassTime;
	ArtConveyorWithPusher::conveyorRunTimer = 0;
	ArtConveyorWithPusher::PushInTimer = 0;
	ArtConveyorWithPusher::PushOutTimer = 0;
	ArtConveyorWithPusher::productEnterSensConvey = false;
	ArtConveyorWithPusher::productExitSensConvey = false;
	ArtConveyorWithPusher::productFctEnterConveyor = false;
	ArtConveyorWithPusher::conveyorState = ST_CONVEYOR_UNKNOWN;
	ArtConveyorWithPusher::SetedProdNumberCollect = 0;
	ArtConveyorWithPusher::CalcedNumProdInConveyor = 0;
	ArtConveyorWithPusher::DevNum = DevNum;
}

void ArtConveyorWithPusher::doLogic()
{
	ArtIOClass::ConvState(conveyorState, DevNum);

	if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut)) //сброс ошибки частотника
	{
		conveyorState == ST_CONVEYOR_UNKNOWN;
		conveyorRunTimer = 0;
	}
	// int retVal;
	if ((conveyorState == ST_CONVEYOR_FREE) || (conveyorState == ST_CONVEYOR_PROD_FWD))
	{
		// ArtIOClass::ConvReady(1);//раскоментить если нужен сигнал на предыдущий конвейер
	}
	else
	{
		// ArtIOClass::ConvReady(0); //раскоментить если нужен сигнал на предыдущий конвейер
	}

	if (ActuatorsGet(GET_CONV_ACTUATOR_READY, ActPoint) != 1)
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
		conveyorRunTimer = 0;
		if ((NextConvPoint->ConveyorGetReadyReceive() != 1) || !PusherCylPtr->cylCloseIn->SensorState())
		{
			conveyorState = ST_CONVEYOR_BUSY;
		}

		if ((NextConvPoint->ConveyorGetReadyReceive() == 1) && (OnPusher->SensorState()) && PusherCylPtr->cylCloseIn->SensorState())
		{
			conveyorState = ST_CONVEYOR_MOVE_OUT;
		}

		if (productEnterSensConvey && (NextConvPoint->ConveyorGetReadyReceive() == 1) && PusherCylPtr->cylCloseIn->SensorState()) //
		{
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_MOVE_IN;
		}
		break;
	}

	case ST_CONVEYOR_MOVE_IN:
	{
		if (conveyorRunTimer == 0 && PusherCylPtr->cylCloseIn->SensorState())
		{
			ActuatorsSet(SET_CONV_ACTUATOR_FWD, ActPoint);
			conveyorRunTimer = ArtIOClass::ARTTimerGetTime();
		}

		if (OnPusher->SensorState())
		{
			conveyorRunTimer = 0;
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
			conveyorState = ST_CONVEYOR_MOVE_OUT;
		}

		if (ARTTimerIsTimePassed(conveyorRunTimer, productPassTime, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			conveyorRunTimer = 0;
			conveyorState = ST_CONVEYOR_FREE;
		}

		break;
	}

	case ST_CONVEYOR_MOVE_OUT:
	{
		if (NextConvPoint->ConveyorGetReadyReceive() == 1 && OnPusher->SensorState() && PusherCylPtr->cylCloseIn->SensorState())
		{
			PusherCylPtr->ARTCylinderOpen();
			// PushOutTimer = ArtIOClass::ARTTimerGetTime();
		}

		if (PusherCylPtr->cylOpenIn->SensorState())
		{
			PusherCylPtr->ARTCylinderClose();
			/*PushInTimer = ArtIOClass::ARTTimerGetTime();
			PushOutTimer = 0;*/
		}

		if (PusherCylPtr->cylCloseIn->SensorState() && !OnPusher->SensorState())
		{
			PushInTimer = 0;
			conveyorState = ST_CONVEYOR_FREE;
		}

		/*if (ARTTimerIsTimePassed(PushOutTimer, 10000, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			PushOutTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			PusherCylPtr->ARTCylinderClose();
		}

		if (ARTTimerIsTimePassed(PushInTimer, 10000, 99000))
		{
			ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
			PushInTimer = 0;
			conveyorState = ST_CONVEYOR_ERROR;
			PusherCylPtr->ARTCylinderOpen();
		}*/

		break;
	}

	case ST_CONVEYOR_BUSY:
	{
		if (PusherCylPtr->cylOpenIn->SensorState())
		{
			PusherCylPtr->ARTCylinderClose();
			PushInTimer = ArtIOClass::ARTTimerGetTime();
		}

		if (PusherCylPtr->cylCloseIn->SensorState())
		{
			PushInTimer = 0;
		}

		if (NextConvPoint->ConveyorGetReadyReceive() == 1 && PusherCylPtr->cylCloseIn->SensorState())
		{
			conveyorRunTimer = 0;
			if (!OnPusher->SensorState())
			{
				conveyorState = ST_CONVEYOR_FREE;
			}
		}

		/*	if (ARTTimerIsTimePassed(PushInTimer, 10000, 99000))
			{
				ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint); //!* !Change to actual function
				PushInTimer = 0;
				conveyorState = ST_CONVEYOR_ERROR;
				PusherCylPtr->ARTCylinderOpen();
			}*/

		break;
	}

	case ST_CONVEYOR_ERROR:
	{
		ActuatorsSet(SET_CONV_ACTUATOR_STOP, ActPoint);
		ArtIOClass::Error(error << ((DevNum - 1) * 4), true); // смещаем влево на 4 бита
		if (ArtIOClass::ResetDrv(ActPoint->ResetSignalOut))	  //сброс ошибки частотника
		{
			conveyorState = ST_CONVEYOR_UNKNOWN;
			ArtIOClass::Error(error << ((DevNum - 1) * 4), false);
		}

		break;
	}
	default:
		break;
	}
}
//-------------------------------------------ArtConveyorWithPusher-----------------------------------------------------------------------
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