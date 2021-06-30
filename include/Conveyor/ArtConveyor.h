#ifndef ArtConv
#define ArtConv
#include "Actuator\ArtActDriver.h"
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
		CONVEYOR_TYPE_1A
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
		ST_CONVEYOR_ERROR
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
	int static ARTTimerGetTime();
	
};

class ArtConveyor2Type : public ArtBasicConveyor
{
public:
	bool productContSensConvey, fproductCounted, pp_stack_ready, ftook_product;
	int productInSensAllowTime,
		productPassTime;
	ArtSensor *ExitSensPoint2;	//указатель на входной сенсор
	ArtSensor *EnterSensPoint2; //указатель на выходной сенсор
	ArtConveyor2Type(int id, const char name[]);
	ArtConveyor2Type(int id, const char name[], ConveyorType type, ArtDriver *ActPoint2, ArtSensor *EnterSensPoint2, ArtSensor *ExitSensPoint2, int PassTime, int NumProdInConveyor, int InSensAllowTime);
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
	ArtConveyor2Type *NextConvPoint;
	ArtConveyor1Type(int id, const char name[]);
	ArtConveyor1Type(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, ArtConveyor2Type *NextConvPoint, int PassTime, int RunTimer);
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
	ArtConveyor1Type *NextConv1Point;
	ArtConveyor1EType(int id, const char name[]);
	ArtConveyor1EType(int id, const char name[], ConveyorType type, ArtDriver *Act3, ArtSensor *Sens3_Enter, ArtSensor *Sens3_Exit, ArtConveyor1Type *Next3_Conv, int PassTime, int RunTimer);
	void ConvReturnStateAndTime();
	void ConveyorSaveStateAndTime();
	void doLogic();
};

class ArtConveyor1AType : public ArtBasicConveyor
{
private:
	bool fproductCounted, productContSensConvey,pp_stack_ready;

public:
	int productPassTime;
	ArtSensor *EnterSensPoint; //указатель на входной сенсор
	ArtSensor *ExitSensPoint;  //указатель на выходной сенсор
	//указатель на следующий конвейер
	ArtConveyor2Type *NextConvPoint;
	ArtConveyor1AType(int id, const char name[]);
	ArtConveyor1AType(int id, const char name[], ConveyorType type, ArtDriver *ActPoint, ArtSensor *EnterSensPoint, ArtSensor *ExitSensPoint, ArtConveyor2Type *NextConvPoint, int PassTime, int RunTimer, int SetedProdNumberCollect);
	void doLogic();
	void AccumConv(bool flag);
};
#endif //ArtConv