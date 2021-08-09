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

class ArtBasicConveyor : public ArtConveyor
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
		CalcedNumProdInConveyor;
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
		CONVEYOR_TYPE_1_EXTERNAL_BARDA
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
		ST_CONVEYOR_FIXED
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

class ArtConveyor2Type : public ArtBasicConveyor
{
public:
	bool productCountSensConvey, fproductCounted, pp_stack_ready, ftook_product;
	int productInSensAllowTime,
		productPassTime;
	ArtSensor *ExitSensPtr;	 //указатель на входной сенсор
	ArtSensor *EnterSensPtr; //указатель на выходной сенсор
	ArtSensor *CountSensPtr; //указатель на выходной сенсор
	ArtConveyor2Type(int id, const char name[]);
	ArtConveyor2Type(int id, const char name[], ConveyorType type, ArtDriver *ActPoint2, ArtSensor *EnterSensPtr, ArtSensor *CountSensPtr, ArtSensor *ExitSensPtr, int PassTime, int NumProdInConveyor, int InSensAllowTime);
	void doLogic();
	void ArtConveyorTookProduct();
};
//-----------------------------------------------------------------------
class ArtConveyor1Type : public ArtBasicConveyor
{
public:
	int productPassTime;
	ArtSensor *EnterSensPoint; //указатель на входной сенсор
	ArtSensor *ExitSensPoint;  //указатель на выходной сенсор
	//указатель на следующий конвейер
	ArtBasicConveyor *NextConvPoint;
	ArtConveyor1Type(int id, const char name[]);
	ArtConveyor1Type(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, ArtBasicConveyor *NextConvPoint, int PassTime, int RunTimer);
	void doLogic();
};
//-------------------------------------------------------------------------
class ArtConveyor1EType : public ArtBasicConveyor
{
public:
	int productPassTime3;
	ArtSensor *EnterSensPoint3; //указатель на входной сенсор
	ArtSensor *ExitSensPoint3;	//указатель на выходной сенсор
	//указатель на следующий конвейер
	ArtBasicConveyor *NextConv1Point;
	ArtConveyor1EType(int id, const char name[]);
	ArtConveyor1EType(int id, const char name[], ConveyorType type, ArtDriver *Act3, ArtSensor *Sens3_Enter, ArtSensor *Sens3_Exit, ArtBasicConveyor *Next3_Conv, int PassTime, int RunTimer);
	void ConvReturnStateAndTime();
	void ConveyorSaveStateAndTime();
	void doLogic();
};

class ArtConveyor1AType : public ArtBasicConveyor
{
private:
	bool fproductCounted, productCountSensConvey, pp_stack_ready, readySignalFromNextBarda;

public:
	int productPassTime;
	ArtSensor *EnterSensPoint; //указатель на входной сенсор
	ArtSensor *ExitSensPoint;  //указатель на выходной сенсор
	//указатель на следующий конвейер
	ArtBasicConveyor *NextConvPoint;
	ArtConveyor1AType(int id, const char name[]);
	ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, ArtBasicConveyor *NextConvPoint, int PassTime, int RunTimer, int SetedProdNumberCollect);
	ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, bool readySignalFromNextBarda, int PassTime, int RunTimer, int SetedProdNumberCollect);
	void doLogic();
	void AccumConv(bool flag);
};

class ArtConveyorShuttleType : public ArtBasicConveyor
{
private:
	bool fproductCounted, productCountSensConvey, pp_stack_ready, readySignalFromNextBarda, On_Position;
	int ReqPos, ConveyorRunTimerShuttle, ConveyorRunTimerOverShuttle, PLPNum;
	ArtDriver *ShuttlePtr;
	ArtDriver *OverShuttlePtr;

public:
	int productPassOverShuttle, PassTimeShuttle, CurPos, BrakeOUT, Position1, Position2, Position3, Position4, Position5;
	ArtAnalogSensor *PositionSens; //указатель на аналоговый датчик
	ArtSensor *PalletOnConv;
	ArtSensor *SensOnIN;
	ArtSensor *SensOnOUT;
	ArtSensor *ShuttleEdge;

	ArtBasicConveyor *NextConvPtr; //указатель на приниммающий конвейер

	ArtConveyorShuttleType(int id, const char name[]);
	ArtConveyorShuttleType(int id, const char name[], ArtDriver *ShuttlePtr, ArtDriver *OverShuttlePtr, ArtAnalogSensor *PositionSens, ArtSensor *PalletOnConv,
						   ArtSensor *SensOnIN, ArtSensor *SensOnOUT, ArtBasicConveyor *NextConvPtr, ArtSensor *ShuttleEdge, bool readySignalFromNextBarda, int BrakeOUT, int PassTimeOverShuttle,
						   int PassTimeShuttle, int RunTimer, int ConveyorRunTimerOverShuttle);
	void doLogic();
	int expRunningAverage(int newVal);
	int findMedianN_optim(int newVal);
	int simpleKalman(int newVal);
	int ABfilter(int newVal);
};

class ArtConveyor1TypeNextExtDev : public ArtBasicConveyor
{
private:
	ArtSensor *EnterSensPoint; //указатель на входной сенсор
	ArtSensor *ExitSensPoint;  //указатель на выходной сенсор
	int productPassTime, ExtDevReady;

public:
	ArtConveyor1TypeNextExtDev(int id, const char name[]);
	ArtConveyor1TypeNextExtDev(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, int PassTime, int RunTimer, int ExtDevReady);
	void doLogic();
};

class ArtPalletConveyorWithStoppers : public ArtBasicConveyor
{
private:
	ArtSensor *Pos1Ptr; //указатель на сенсор 1 позиция
	ArtSensor *Pos2Ptr; //указатель на сенсор 2 позиция
	ArtSensor *Pos3Ptr; //указатель на сенсор 3 позиция
	ArtSensor *Pos4Ptr; //указатель на сенсор 4 позиция
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
								  ArtCylinder *StopperPos2, ArtCylinder *StopperPos3, ArtCylinder *StopperPos4, int PassTime, int RunTimer);
	void doLogic();
};

class ArtConveyorPLPType : public ArtBasicConveyor
{
public:
	int productPassTime, SignalPalletFull, PLPNum;
	bool PalletOnPosition;
	ArtSensor *EnterSensPoint; //указатель на входной сенсор
	ArtSensor *ExitSensPoint;  //указатель на выходной сенсор
	ArtSensor *PallOnPosition; //указатель на сенсор наличия паллеты
	ArtSensor *LayerSensor;	   //указатель на сенсор слоя
	ArtCylinder *Stopper;
	ArtCylinder *Podzhim;
	ArtConveyorPLPType(int id, const char name[]);
	ArtConveyorPLPType(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint,
					   ArtSensor *ExitSensPoint, ArtSensor *PallOnPosition, ArtSensor *LayerSensor,
					   ArtCylinder *Stopper, ArtCylinder *Podzhim, int PassTime, int RunTimer, int PLPNum);
	void doLogic();
};

#endif //ArtConv