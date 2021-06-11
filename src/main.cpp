#include <Arduino.h>
#undef min
#undef max
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

  //---- setup ---------------------------------------------------------------------------------------
  /*ArtSensor Sens1(1, ("Sensor1"), 0, ArtSensor::SENSOR_TYPE_BASIC, 0, 0); // Пусть этот будет входным датчиком;
  ArtSensor Sens2(2, ("Sensor2"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0); // Пусть этот будет выходным датчиком
  ArtSensor Sens3(3, ("Sensor3"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0); // Пусть этот будет входным датчиком2;
  ArtSensor Sens4(4, ("Sensor4"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0); // Пусть этот будет выходным датчиком3;
  ArtDriver Act1(1, ("Actuator1"), ArtDriver::DRIVER_TYPE_1, 8, true, 0, 2, 0, 2, 0, 0, 0);
  ArtDriver Act2(2, ("Actuator2"), ArtDriver::DRIVER_TYPE_1, 9, true, 0, 2, 0, 2, 0, 0, 0);
  ArtDriver Act3(3, ("Actuator3"), ArtDriver::DRIVER_TYPE_1, 10, true, 0, 2, 0, 2, 0, 0, 0);
  ArtConveyor2Type Conv3(8, ("Conveyor3"), ArtConveyor2Type::CONVEYOR_TYPE_2, &Act3, &Sens3, &Sens4, 4000, 4, 9000);
  ArtConveyor1Type Conv2(9, ("Conveyor2"), ArtConveyor1Type::CONVEYOR_TYPE_1, &Act2, &Sens2, &Sens3, &Conv3, 6000, 0);
  ArtConveyor1EType Conv1(10, ("Conveyor1"), ArtConveyor1Type::CONVEYOR_TYPE_1E, &Act1, &Sens1, &Sens2, &Conv2, 6000, 0);*/
  // ArtCylinder::ArtCylinder(int id, const char name[], int CloseTime, int OpenTime , bool TimeoutControl , bool CylinderSet, distType type, int cylOpenOut, int cylCloseOut)
  //Sensors---ArtSensor(int id, const char name[], int SensorInput, SensorType type, int delayRe, int delayFe) ---------------------------------------------------------------------
  ArtSensor PallONConvey(1, ("PallONConvey"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0);
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

  ArtCylinder Clamp1(11, ("Clamp1"), 10, 10, false, true, ArtCylinder::BI_STABLE,
                     0, 1, &Clamp1Open, &Clamp1Close);

  ArtCylinder Clamp2(12, ("Clamp2"), 10, 10, false, true, ArtCylinder::BI_STABLE,
                     0, 1, &Clamp2Open, &Clamp2Close);

  ArtCylinder TOPCylinder(13, ("TOPCylinder"), 10, 10, false, true,
                          ArtCylinder::BI_STABLE, 2, 3, &TOPCylinderOpen,
                          &TOPCylinderClose);

  ArtCylinder BOTCylinder(14, ("BOTCylinder"), 10, 10, false, true,
                          ArtCylinder::BI_STABLE, 4, 5, &BOTCylinderOpen,
                          &BOTCylinderClose);

  PalletMagazine Dispenser(15, ("Dispenser"), &Clamp1, &Clamp2, &TOPCylinder,
                           &BOTCylinder, 8, &PallONConvey, &PalletsInStack, 9, 10);

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
  }
}

//---- main loop ----------------------------------------------------------------------------------------

void loop()
{
}

//---- user application ------------------------------------------------------------------------------
