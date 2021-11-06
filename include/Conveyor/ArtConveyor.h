#ifndef ArtConv
#define ArtConv
#include "Actuator\ArtActDriver.h"
#include "Actuator\ArtActCylinder.h"
#include "Sensor\ArtSensor.h"
#include "interface\IHasCycleLogic.h"

class IChainable
{
public:
	virtual bool isAvailable() = 0;
	virtual IChainable *getNext() = 0;
	virtual IChainable *getPrev() = 0;
};

class ArtConveyor : public IHasCycleLogic
{
protected:
	int m_id;
	char name[256];

public:
	int getName();
	int getID();
	void update();
	void doLogic();
};

class ArtBasicConveyor : public ArtConveyor // базовый конвейер от него все наследуются
{
protected:
	int prodTravelTime,
		conveyorType,
		parameter,
		conveyorState,
		nconveyorLastState,
		conveyorRunTimer,
		nconveyorType,
		SetedProdNumberCollect,
		CalcedNumProdInConveyor,
		DevNum,
		error;
	bool outSensor,
		inSensor,
		productFctEnterConveyor,
		productEnterSensConvey,
		productExitSensConvey,
		nconveyorExtDeviceBlock,
		nconveyorExtModeEnabled,
		nconveyorExtFWD,
		nconveyorExtSTOP,
		nconveyorExtREV,
		AccumConvOn;
	float productDeltaPassTime;

public:
	enum ConveyorType
	{
		CONVEYOR_TYPE_1,
		CONVEYOR_TYPE_1E,
		CONVEYOR_TYPE_2,
		CONVEYOR_TYPE_3,
		CONVEYOR_TYPE_4,
		CONVEYOR_TYPE_1A,
		CONVEYOR_TYPE_1_EXTERNAL_BARDA,
		CONVEYOR_TYPE_1_EXTE,
		CONVEYOR_TYPE_CROSS,
		CONVEYOR_TYPE_SHUTTLE
	};

	enum ConveyorState
	{
		ST_CONVEYOR_UNKNOWN,
		ST_CONVEYOR_FREE,
		ST_CONVEYOR_BUSY,
		ST_CONVEYOR_STACK_READY,
		ST_CONVEYOR_PROD_FWD,
		ST_CONVEYOR_PROD_REV,
		ST_CONVEYOR_CONT_MANUAL,
		ST_CONVEYOR_CONT_EXT,
		ST_CONVEYOR_ERROR,
		ST_CONVEYOR_RUN,
		ST_CONVEYOR_AT_THE_POINT,
		ST_1_POS,
		ST_2_POS,
		ST_3_POS,
		ST_4_POS,
		ST_SHUTTLE_CONVEYOR_ERROR,
		ST_OVERSHUTTLE_CONVEYOR_ERROR,
		ST_CONVEYOR_MOVE_OUT,
		ST_CONVEYOR_MOVE_IN,
		ST_CONVEYOR_PALL_FULL,
		ST_CONVEYOR_PALL_PLACED,
		ST_CONVEYOR_NEEDPALLET,
		ST_CONVEYOR_UNFIXED,
		ST_CONVEYOR_FIXED,
		ST_CONVEYOR_POS_SELECT,
		ST_CONVEYOR_LIFT_UP,
		ST_CONVEYOR_LIFT_DOWN,
		ST_CONVEYOR_PALLET_READY
	};

	enum GetDriverState
	{
		GET_CONV_READY_RECEIVE,
		GET_CONV_MANUAL,
		GET_CONV_ACTUATOR_FWD,
		GET_CONV_ACTUATOR_REV,
		GET_CONV_ACTUATOR_STOP,
		GET_CONV_ACTUATOR_READY,
		GET_CONV_STATE
	};

	enum SetDriverState
	{
		SET_CONV_ACTUATOR_FWD,	//Parameter for set driver run forward
		SET_CONV_ACTUATOR_STOP, //Parameter for set driver stop
		SET_CONV_ACTUATOR_REV,	//Parameter for set driver run reverse
		SET_CONV_PROD_ENTERING, //Parameter for set product in enter conveyor
		SET_CONV_DIRECTION_FWD, //Parameter for set direction forward
		SET_CONV_DIRECTION_REV	//Parameter for set direction reverse
	};
	ArtDriver *pointer;
	ArtSensor *EnterSensPoint; //указатель на входной сенсор
	ArtSensor *ExitSensPoint;
	ArtDriver *ActPoint;  //указатель на драйвер
	ArtDriver *ActPoint2; //указатель на драйвер
	ArtDriver *ActPoint3; //указатель на драйвер
	int ConveyorGetReadyReceive();
	void ConveyorSetProdEntering();
	bool ARTTimerIsTimePassed(int timer, int timeSpan, int timeOut);
	int ActuatorsGet(int parameter, ArtDriver *pointer);
	int ActuatorsSet(int parameter, ArtDriver *pointer);
	int ConveyorGetState();
};

class ArtConveyor2Type : public ArtBasicConveyor // накопительный конвейер
{
public:
	bool productCountSensConvey, fproductCounted, pp_stack_ready, ftook_product;
	int productInSensAllowTime,
		productPassTime;
	ArtSensor *ExitSensPtr;	 //указатель на входной сенсор
	ArtSensor *EnterSensPtr; //указатель на выходной сенсор
	ArtSensor *CountSensPtr; //указатель на выходной сенсор
	ArtConveyor2Type(int id, const char name[]);
	ArtConveyor2Type(int id, const char name[], ConveyorType type, ArtDriver *ActPoint2, ArtSensor *EnterSensPtr,
					 ArtSensor *CountSensPtr, ArtSensor *ExitSensPtr, int PassTime, int NumProdInConveyor, int InSensAllowTime, int DevNum);
	void doLogic();
	void ArtConveyorTookProduct();
};
//-----------------------------------------------------------------------
class ArtConveyor1Type : public ArtBasicConveyor // проходной конвейер
{
public:
	int productPassTime;
	//указатель на следующий конвейер
	ArtBasicConveyor *NextConvPoint;
	ArtConveyor1Type(int id, const char name[]);
	ArtConveyor1Type(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint,
					 ArtSensor *ExitSensPoint, ArtBasicConveyor *NextConvPoint, int PassTime, int RunTimer, int DevNum);
	void doLogic();
};
//-------------------------------------------------------------------------
class ArtConveyor1EType : public ArtBasicConveyor //конвейер который управляется извне
{
public:
	int productPassTime3;
	ArtSensor *EnterSensPoint3; //указатель на входной сенсор
	ArtSensor *ExitSensPoint3;	//указатель на выходной сенсор
	//указатель на следующий конвейер
	ArtBasicConveyor *NextConv1Point;
	ArtConveyor1EType(int id, const char name[]);
	ArtConveyor1EType(int id, const char name[], ConveyorType type, ArtDriver *Act3, ArtSensor *Sens3_Enter, ArtSensor *Sens3_Exit,
					  ArtBasicConveyor *Next3_Conv, int PassTime, int RunTimer, int DevNum);
	void ConvReturnStateAndTime();
	void ConveyorSaveStateAndTime();
	void doLogic();
};

class ArtConveyor1AType : public ArtBasicConveyor // проходной конвейер который по сигналу может стать накопительным
{
private:
	bool fproductCounted, productCountSensConvey, pp_stack_ready, readySignalFromNextBarda;

public:
	int productPassTime;
	//указатель на следующий конвейер
	ArtBasicConveyor *NextConvPoint;
	ArtConveyor1AType(int id, const char name[]);
	ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, ArtBasicConveyor *NextConvPoint, int PassTime, int RunTimer, int SetedProdNumberCollect, int DevNum);
	ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, bool readySignalFromNextBarda, int PassTime, int RunTimer, int SetedProdNumberCollect, int DevNum);
	void doLogic();
	void AccumConv(bool flag);
};

class ArtConveyorShuttleType : public ArtBasicConveyor // Конвейер - шаттл
{
private:
	bool fproductCounted, productCountSensConvey, pp_stack_ready, readySignalFromNextBarda, On_Position;
	int ReqPos, ConveyorRunTimerShuttle, ConveyorRunTimerOverShuttle, PLPNum;
	ArtDriver *ShuttlePtr;
	ArtDriver *OverShuttlePtr;
public:
	bool signalfornext;
	int productPassOverShuttle, PassTimeShuttle, CurPos, BrakeOUT, Range;
	ArtAnalogSensor *PositionSens; //указатель на аналоговый датчик
	ArtSensor *PalletOnConv;
	ArtSensor *SensOnIN;
	ArtSensor *SensOnOUT;
	ArtSensor *ShuttleEdge;
	ArtBasicConveyor *NextConvPtr; //указатель на приниммающий конвейер

	ArtConveyorShuttleType(int id, const char name[]);
	ArtConveyorShuttleType(int id, const char name[],ConveyorType type, ArtDriver *ShuttlePtr, ArtDriver *OverShuttlePtr, ArtAnalogSensor *PositionSens, ArtSensor *PalletOnConv,
						   ArtSensor *SensOnIN, ArtSensor *SensOnOUT, ArtBasicConveyor *NextConvPtr, ArtSensor *ShuttleEdge, bool readySignalFromNextBarda, int BrakeOUT, int PassTimeOverShuttle,
						   int PassTimeShuttle, int RunTimer, int ConveyorRunTimerOverShuttle, int Range, int DevNum);
	void doLogic();
	int expRunningAverage(int newVal);
	int findMedianN_optim(int newVal);
	int simpleKalman(int newVal);
	int ABfilter(int newVal);
};

class ArtConveyor1TypeNextExtDev : public ArtBasicConveyor //конвейер проходного типа который получает сигнал от
{														   //следующего устройства по EtherCAT (например от обмотчика)
private:
	int productPassTime, ExtDevReady, BitNum;

public:
	bool signalExt;
	ArtConveyor1TypeNextExtDev(int id, const char name[]);
	ArtConveyor1TypeNextExtDev(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, int PassTime, int RunTimer, int BitNum, int DevNum);
	void doLogic();
};

class ArtPalletConveyorWithStoppers : public ArtBasicConveyor // паллетный конвейер после диспенсера со стоперами и лифтами
{
private:
	ArtSensor *Pos1Ptr;	  //указатель на сенсор 1 позиция
	ArtSensor *Pos2Ptr;	  //указатель на сенсор 2 позиция
	ArtSensor *Pos3Ptr;	  //указатель на сенсор 3 позиция
	ArtSensor *Pos4Ptr;	  //указатель на сенсор 4 позиция
	ArtSensor *Lift1Down; //лифт 1 вверху
	ArtSensor *Lift2Down; //лифт 2 вверху
	ArtSensor *Lift3Down; //лифт 3 вверху
	ArtSensor *Lift4Down; //лифт 4 вверху
	ArtCylinder *StopperPos1;
	ArtCylinder *StopperPos2;
	ArtCylinder *StopperPos3;
	ArtCylinder *StopperPos4;
	ArtDriver *DispDrvPtr;
	bool Pos1Sens, Pos2Sens, Pos3Sens, Pos4Sens, Conv1Free, Conv2Free, Conv3Free, Conv4Free, flag;
	int productPassTime, ExtDevReady;

public:
	ArtPalletConveyorWithStoppers(int id, const char name[]);
	ArtPalletConveyorWithStoppers(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtDriver *DispDrvPtr,
								  ArtSensor *Pos1Ptr, ArtSensor *Pos2Ptr, ArtSensor *Pos3Ptr, ArtSensor *Pos4Ptr, ArtCylinder *StopperPos1,
								  ArtCylinder *StopperPos2, ArtCylinder *StopperPos3, ArtCylinder *StopperPos4, int PassTime,
								  int RunTimer, ArtSensor *Lift1Down, ArtSensor *Lift2Down, ArtSensor *Lift3Down, ArtSensor *Lift4Down,
								  int DevNum);
	void doLogic();
};

class ArtConveyorPLPType : public ArtBasicConveyor // конвейер - PLP
{
public:
	int productPassTime, SignalPalletFull, PLPNum;
	bool PalletOnPosition;
	ArtSensor *PallOnPosition; //указатель на сенсор наличия паллеты
	ArtSensor *LayerSensor;	   //указатель на сенсор слоя
	ArtCylinder *Stopper;
	ArtCylinder *Podzhim;
	ArtConveyorPLPType(int id, const char name[]);
	ArtConveyorPLPType(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint,
					   ArtSensor *ExitSensPoint, ArtSensor *PallOnPosition, ArtSensor *LayerSensor,
					   ArtCylinder *Stopper, ArtCylinder *Podzhim, int PassTime, int RunTimer, int PLPNum, int DevNum);
	void doLogic();
};

class ArtConveyorWithLift : public ArtBasicConveyor //конвейер лифтовый подъем осуществляется двигателем
{
public:
	int productPassTime, LiftUpTime, LiftDOWNTime, conveyorLiftRunTimer;
	//указатель на выходной сенсор
	ArtDriver *LiftDrv; //указатель на драйвер лифта
	ArtSensor *LiftUP;
	ArtSensor *LiftDOWN;
	ArtSensor *NextConvEnd;
	ArtConveyorWithLift(int id, const char name[]);
	ArtConveyorWithLift(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtDriver *LiftDrv, ArtSensor *EnterSensPoint,
						ArtSensor *ExitSensPoint, ArtSensor *LiftUP, ArtSensor *LiftDOWN, ArtSensor *NextConvEnd,
						int PassTime, int LiftUpTime, int LiftDOWNTime, int RunTimer, int DevNum);
	void doLogic();
};

class ArtConveyorWithLiftType1 : public ArtBasicConveyor //конвейер лифтовый подъем осуществляется цилиндром
{
public:
	int productPassTime, LiftUpTime, LiftDOWNTime, conveyorLiftRunTimer, LiftNum;
	//указатель на выходной сенсор
	ArtSensor *LiftUP;
	ArtSensor *LiftDOWN;
	ArtCylinder *Lift;
	ArtSensor *NextConvEnd;
	ArtConveyorWithLiftType1(int id, const char name[]);
	ArtConveyorWithLiftType1(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtCylinder *Lift,
							 ArtSensor *LiftUP, ArtSensor *LiftDOWN, ArtSensor *NextConvEnd,
							 int PassTime, int LiftUpTime, int LiftDOWNTime, int RunTimer, int LiftNum, int DevNum);
	void doLogic();
};

class ArtConveyorPalletPickPoint : public ArtBasicConveyor // конвейер-пикпоинт
{
public:
	int productPassTime, ButtonIn;
	//указатель на выходной сенсор
	ArtSensor *ConvEnd;
	ArtSensor *ConvEnter;
	ArtDriver *ActPoint;
	ArtConveyorPalletPickPoint(int id, const char name[]);
	ArtConveyorPalletPickPoint(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *ConvEnter, ArtSensor *ConvEnd,
							   int PassTime, int RunTimer, int ButtonIn, int DevNum);
	void doLogic();
};

class ArtCrossConveyor : public ArtBasicConveyor // паллетный конвейер после диспенсера со стоперами и лифтами
{
private:
	ArtSensor *PosPtr;	 //указатель на сенсор 1 позиция
	ArtSensor *LiftDown; //лифт 1 внизу
	ArtCylinder *StopperPos;
	ArtDriver *DispDrvPtr;
	bool Pos1Sens, Pos2Sens, Pos3Sens, Pos4Sens, Conv1Free, Conv2Free, Conv3Free, Conv4Free, flag, last;
	int productPassTime, ExtDevReady;
	ArtBasicConveyor *NextConvPoint;

public:
	ArtCrossConveyor(int id, const char name[]);
	ArtCrossConveyor(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtDriver *DispDrvPtr,
					 ArtBasicConveyor *NextConvPoint, ArtSensor *PosPtr, ArtCylinder *StopperPos, int PassTime, int RunTimer,
					 ArtSensor *LiftDown, int DevNum, bool last);
	ArtCrossConveyor(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtDriver *DispDrvPtr,
					 ArtSensor *PosPtr, ArtCylinder *StopperPos, int PassTime, int RunTimer,
					 ArtSensor *LiftDown, int DevNum, bool last);
	void doLogic();
};

#endif //ArtConv