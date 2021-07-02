#include <Arduino.h>
#undef min
#undef max
#undef B1
#include "Conveyor\ArtConveyor.h"
#include "Actuator\ArtActDriver.h"
#include "Actuator\ArtActCylinder.h"
#include "Sensor\ArtSensor.h"
#include "interface\IHasCycleLogic.h"
#include "ArtIOClass.h"
#include "PalletMagazine\ArtPalletMagazine.h"
#include "EasyCAT\EasyCAT.h" // EasyCAT library to interface the LAN9252
#include <SPI.h>
EasyCAT EASYCAT; // EasyCAT istantiation
#include "InOut.h"
IHasCycleLogicHelper Helper;

//---- global variables ---------------------------------------------------------------------------

uint8_t AnaInChannel;
uint32_t DogTimer;
uint32_t PreviousTime;

//---- setup ----------------------------------------------------------------------------------------

void setup()
{
  ArtIOClass::initializeBoard(&EASYCAT);
  //-- pins directions ----------------------------------------------
  ConfDac(); // initialize the DAC

  //---- initialize the EasyCAT interface -----

  if (EASYCAT.Init() == true) // initialization
  {                           // succesfully completed
  }                           //
  else
  {
    // the EasyCAT interface was not recognized
    while (1)                          // stay in loop for ever
    {                                  // with the led blinking
      PORT->Group[1].OUTCLR.reg = LED; //
      delay(300);                      //
      PORT->Group[1].OUTSET.reg = LED; //
      delay(300);                      //
    }                                  //
  }
  // DogTimer = millis();                      // load the watchdog timer
  PORT->Group[0].OUTSET.reg = WDOG; //этот выход включает пин OUT_EN на драйвере перифирии ISO8200B


  /*/----setup for A31---------------------------------------------------------------------------------------
  ArtSensor AfterSiat(1, ("B1"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик после Сиата 1 (B1) (тип R)
  ArtSensor M1End(2, ("B2"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М1 (B2) (тип R)
  ArtSensor M2End(3, ("B3"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М2 (B3) (тип R)
  ArtSensor M3Perepoln(4, ("B4"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М3 (B4) (тип R)
  
  ArtSensor M3End(5, ("B5"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М3 (B5) (тип R)
  
  ArtDriver M1DRV(5, ("M1DRV"), ArtDriver::DRIVER_TYPE_1, 9, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M1
  ArtDriver M2DRV(10, ("M2DRV"), ArtDriver::DRIVER_TYPE_1, 10, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M2
  ArtDriver M3DRV(11, ("M3DRV"), ArtDriver::DRIVER_TYPE_1, 11, 3, 3, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M3

  ArtConveyor1AType ConvM3(12, ("ConvM3"), ArtConveyor1AType::CONVEYOR_TYPE_1_EXTERNAL_BARDA, &M3DRV, &M2End, &M3End, false, 4000, 4,4);
  //ArtConveyor1Type ConvM2(13, ("ConvM2"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M4DRV, &B5, &B6, &ConvM6, 1000, 0);
  //ArtConveyor1Type ConvM1(14, ("ConvM1"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M4DRV, &B5, &B6, &ConvM5, 1000,0);
  //----setup for A31---------------------------------------------------------------------------------------
*/
  //----setup for A32---------------------------------------------------------------------------------------
  //Эта A32 барда берет сигнал с датчика B5 с предыдущей барды поэтому в ArtIOClass распарсиваются входы 
  //с предыдущей, также там добавлен сброс ошибки драйвера
 
  ArtSensor B5(1, ("B5"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,true); // Фотодатчик на пушере М4 (B6) (тип BGS)
  ArtSensor B6(2, ("B6"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на пушере М4 (B6) (тип BGS)
  ArtSensor PushHome(3, ("B7"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 1 дома (B7) (тип геркон)
  ArtSensor PushNoHome(4, ("B8"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 1 НЕ дома (B8) (тип геркон)
  ArtSensor M5Perepoln(5, ("B9"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М5 (B9) (тип R)
  ArtSensor M5End(6, ("B10"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М5 (B10) (тип R)
  ArtSensor M6Start(7, ("B11"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале М6 пикпойнт (B11) (тип BGS)
  ArtSensor M6Start2(8, ("B12"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале M6 пикпойнт (B12) (тип BGS)
  ArtSensor M6End(9, ("B13"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М6 пикпойнт (B13) (тип BGS)

  ArtCylinder Clamp1(12, ("SV1"), 10, 10, false, true, ArtCylinder::MONO_STABLE,
                     7, 7, &PushNoHome, &PushHome); //Пушер SV1

  ArtDriver M4DRV(10, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 9, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M4
  ArtDriver M5DRV(11, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 10, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M5
  ArtDriver M6DRV(12, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 11, 3, 3, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M6

  ArtConveyor2Type ConvM6(13, ("Conveyor3"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M6DRV, &B6, &M5End, 4000, 4, 9000);
  ArtConveyor1Type ConvM5(14, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M5DRV, &B5, &B6, &ConvM6, 1000, 0);
  ArtConveyor1Type ConvM4(15, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M4DRV, &B5, &B6, &ConvM5, 1000,0);

  //----setup for A32---------------------------------------------------------------------------------------
  
  
  
  
  
  //ArtConveyor1AType Conv2(9, ("Conveyor2"), ArtConveyor1Type::CONVEYOR_TYPE_1, &Act2, &Sens1, &Sens2, &Conv3, 1000, 0,3);



  /*
  ArtSensor Sens1(1, ("Sensor1"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0); // Пусть этот будет входным датчиком;
  ArtSensor Sens2(2, ("Sensor2"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0); // Пусть этот будет выходным датчиком
  ArtSensor Sens3(3, ("Sensor3"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0); // Пусть этот будет входным датчиком2;
  ArtSensor Sens4(4, ("Sensor4"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0); // Пусть этот будет выходным датчиком3;
  ArtDriver Act1(1, ("Actuator1"), ArtDriver::DRIVER_TYPE_1, 9, true, 0, 2, 0, 2, 0, 0, 0);
  ArtDriver Act2(2, ("Actuator2"), ArtDriver::DRIVER_TYPE_1, 10, true, 0, 2, 0, 2, 0, 0, 0);
  ArtDriver Act3(3, ("Actuator3"), ArtDriver::DRIVER_TYPE_1, 11, true, 0, 2, 0, 2, 0, 0, 0);
  ArtConveyor2Type Conv3(8, ("Conveyor3"), ArtConveyor2Type::CONVEYOR_TYPE_2, &Act3, &Sens3, &Sens4, 4000, 4, 9000);
  ArtConveyor1AType Conv2(9, ("Conveyor2"), ArtConveyor1Type::CONVEYOR_TYPE_1, &Act2, &Sens1, &Sens2, &Conv3, 1000, 0,3);
*/

  //ArtConveyor1EType Conv1(10, ("Conveyor1"), ArtConveyor1Type::CONVEYOR_TYPE_1E, &Act1, &Sens1, &Sens2, &Conv2, 6000, 0);
  // ArtCylinder::ArtCylinder(int id, const char name[], int CloseTime, int OpenTime , bool TimeoutControl , bool CylinderSet, distType type, int cylOpenOut, int cylCloseOut)
  //Sensors---ArtSensor(int id, const char name[], int SensorInput, SensorType type, int delayRe, int delayFe) ---------------------------------------------------------------------
  /*ArtSensor PallONConvey(1, ("PallONConvey"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor PalletsInStack(2, ("PalletsInStack"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor Clamp1Open(3, ("Clamp1Open"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor Clamp1Close(4, ("Clamp1Close"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor Clamp2Open(5, ("Clamp2Open"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor Clamp2Close(6, ("Clamp2Close"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor TOPCylinderOpen(7, ("TOPCylinderOpen"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor TOPCylinderClose(8, ("TOPCylinderClose"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor BOTCylinderOpen(9, ("BOTCylinderOpen"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  ArtSensor BOTCylinderClose(10, ("BOTCylinderClose"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
  //Cylinders  ArtCylinder(int id, const char name[], int CloseTime, int OpenTime, bool TimeoutControl, bool CylinderSet,
  //distType type, int cylOpenOut, int cylCloseOut,ArtSensor *cylOpenIn,ArtSensor *cylCloseIn);

  ArtCylinder Clamp1(11, ("Clamp1"), 10, 10, false, true, ArtCylinder::MONO_STABLE,
                     1, 2, &Clamp1Open, &Clamp1Close);

  ArtCylinder Clamp2(12, ("Clamp2"), 10, 10, false, true, ArtCylinder::MONO_STABLE,
                     1, 2, &Clamp2Open, &Clamp2Close);

  ArtCylinder TOPCylinder(13, ("TOPCylinder"), 10, 10, false, true,
                          ArtCylinder::MONO_STABLE, 3, 4, &TOPCylinderOpen,
                          &TOPCylinderClose);

  ArtCylinder BOTCylinder(14, ("BOTCylinder"), 10, 10, false, true,
                          ArtCylinder::MONO_STABLE, 5, 6, &BOTCylinderOpen,
                          &BOTCylinderClose);

  PalletMagazine Dispenser(15, ("Dispenser"), &Clamp1, &Clamp2, &TOPCylinder,
                           &BOTCylinder, 9, &PallONConvey, &PalletsInStack, 10, 11);*/

  while (1)
  {
    if (EASYCAT.MainTask() == ESM_OP) // execute the EasyCAT task
    {
      int16_t AdcResult;
      Helper.doLogic();
      AdcResult = ReadAdc(AnaInChannel);
      ArtIOClass::doIOLogic();
      switch (AnaInChannel) // we read one channel each round
      {
      case 0:
        EASYCAT.BufferIn.Cust.AnaIn_3 = AdcResult;
        break;

      case 1:
        EASYCAT.BufferIn.Cust.AnaIn_2 = AdcResult;
        break;

      case 2:
        EASYCAT.BufferIn.Cust.AnaIn_1 = AdcResult;
        break;

      case 3:
        EASYCAT.BufferIn.Cust.AnaIn_0 = AdcResult;
        break;
      }

      if (EASYCAT.BufferOut.Cust.Output1 == 1)
      {
        //Conv2.AccumConv(true);
      }

      AnaInChannel++;
      AnaInChannel &= 0x03;

      PORT->Group[0].OUTSET.reg = WDOG;
      PORT->Group[1].OUTSET.reg = LED;
    }
    else
    {
      PORT->Group[1].OUTCLR.reg = LED;
      PORT->Group[0].OUTCLR.reg = WDOG;
    }
    EASYCAT.BufferIn.Cust.ConvReadySignal = ConvM4.ConveyorGetReadyReceive();
    //EASYCAT.BufferIn.Cust.SensSignalOnNextBarda = M3End.SensorState()<<(M3End.SensorInput-1);
  }
}


void loop()
{
}

