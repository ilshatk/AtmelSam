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
#include "Pusher\ArtPusher.h"
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
*//*
  //----setup for A32---------------------------------------------------------------------------------------
  //Эта A32 барда берет сигнал с датчика B5 с предыдущей барды поэтому в ArtIOClass распарсиваются входы 
  //с предыдущей, также там добавлен сброс ошибки драйвера
 
  ArtSensor B5(1, ("B5"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,true); // Фотодатчик на конце М3 (B5) (тип R)
  ArtSensor B6(2, ("B6"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на пушере М4 (B6) (тип BGS)
  ArtSensor PushHome(3, ("B7"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 1 дома (B7) (тип геркон)
  ArtSensor PushNoHome(4, ("B8"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 1 НЕ дома (B8) (тип геркон)
  ArtSensor M5Perepoln(5, ("B9"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М5 (B9) (тип R)
  ArtSensor M5End(6, ("B10"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М5 (B10) (тип R)
  ArtSensor M6Start(7, ("B11"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале М6 пикпойнт (B11) (тип BGS)
  ArtSensor M6Start2(8, ("B12"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале M6 пикпойнт (B12) (тип BGS)
  ArtSensor M6End(9, ("B13"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М6 пикпойнт (B13) (тип BGS)

  ArtCylinder PusherSV1(16, ("SV1"), 10, false, true, ArtCylinder::MONO_STABLE, 7,  &PushNoHome, &PushHome); //Пушер SV1

  ArtDriver M4DRV(10, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 9, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M4
  ArtDriver M5DRV(11, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 10, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M5
  ArtDriver M6DRV(12, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 11, 3, 3, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M6

  ArtConveyor2Type ConvM6(13, ("Conveyor3"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M6DRV, &B6, &M5End, 4000, 4, 9000);
  ArtConveyor1Type ConvM5(14, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M5DRV, &B5, &B6, &ConvM6, 1000, 0);
  ArtConveyor1Type ConvM4(15, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M4DRV, &B5, &B6, &ConvM5, 1000,0);
  //----setup for A32---------------------------------------------------------------------------------------
  
  //----setup for A33---------------------------------------------------------------------------------------
  ArtSensor AfterSiatB14(1, ("B14"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик после Сиата 2 (B14) (тип R)
  ArtSensor M7Perepoln(2, ("B15"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М7 (B15) (тип R)
  ArtSensor M7End(3, ("B16"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М7 (B16) (тип R)
  ArtSensor M8End(4, ("B17"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М8 (B17) (тип R)
  ArtSensor M9Perepoln(5, ("B18"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М9 (B18) (тип R)
  ArtSensor M9End(6, ("B19"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М9 (B19) (тип R)

  ArtDriver M7DRV(10, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 7, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M4
  ArtDriver M8DRV(11, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 8, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M5
  ArtDriver M9DRV(12, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 9, 3, 3, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M6

  ArtConveyor1AType ConvM9(13, ("ConvM9"), ArtConveyor1AType::CONVEYOR_TYPE_1_EXTERNAL_BARDA, &M9DRV, &M8End, &M9End,false, 4000, 4, 4);
  ArtConveyor1Type ConvM8(14, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M8DRV, &M7End, &M8End, &ConvM9, 15000, 0);
  ArtConveyor1Type ConvM7(15, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M7DRV, &AfterSiatB14, &M7End, &ConvM8, 8000,0);
  //----setup for A33---------------------------------------------------------------------------------------

  //----setup for A34---------------------------------------------------------------------------------------
  ArtSensor M9End(1, ("B19"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,true); // Фотодатчик на конце М9 (B19) (тип R) с ПРЕДЫДУЩЕЙ БАРДЫ
  ArtSensor OnPusherM10(2, ("B20"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на пушере М10 (B20) (тип BGS)
  ArtSensor PusherHome(3, ("B21"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 2 дома (B21) (тип геркон)
  ArtSensor PusherNOHome(4, ("B22"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 2 НЕ дома (B22) (тип геркон)
  ArtSensor M11Perepoln(5, ("B23"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М11 (B23) (тип R)
  ArtSensor M11End(6, ("B24"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М11 (B24) (тип R)
  ArtSensor M12Start1(7, ("B25"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале М12 пикпойнт (B25) (тип BGS)
  ArtSensor M12Count(8, ("B26"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале М12 пикпойнт (B26) (тип BGS)
  ArtSensor M12End(9, ("B26"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М12 пикпойнт (B27) (тип BGS)

  ArtCylinder PusherSV2(16, ("SV2"), 10, false, true, ArtCylinder::MONO_STABLE, 7,  &PusherNOHome, &PusherHome); //Пушер SV2

  ArtDriver M10DRV(10, ("M10DRV"), ArtDriver::DRIVER_TYPE_1, 9, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M10
  ArtDriver M11DRV(11, ("M11DRV"), ArtDriver::DRIVER_TYPE_1, 10, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M11
  ArtDriver M12DRV(12, ("M12DRV"), ArtDriver::DRIVER_TYPE_1, 11, 3, 3, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M12

  ArtConveyor2Type ConvM12(13, ("ConvM12"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M12DRV, &M12Start, &M12Count, &M12End, 4000, 4, 4);
  ArtConveyor1Type ConvM11(14, ("ConvM11"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M11DRV, &OnPusherM10, &M11End, &ConvM12, 15000, 0);
  ArtConveyor1Type ConvM10(15, ("ConvM10"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M10DRV, &M9End, &OnPusherM10, &ConvM11, 8000,0);
  //----setup for A34---------------------------------------------------------------------------------------
  
  //----setup for A35---------------------------------------------------------------------------------------
  ArtSensor AfterSiatB28(1, ("B28"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик после Сиата 3 (B28) (тип R)
  ArtSensor OnPusherM13(2, ("B29"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на пушере М13 (B29) (тип BGS)
  ArtSensor PusherHomeB30(3, ("B30"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 3 дома (B30) (тип геркон)
  ArtSensor PusherNOHomeB31(4, ("B31"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 3 НЕ дома (B31) (тип геркон)
  ArtSensor M14Perepoln(5, ("B32"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М14 (B32) (тип R)
  ArtSensor M14End(6, ("B33"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М14 (B33) (тип R)
  ArtSensor M15Perepoln(7, ("B34"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М15 (B34) (тип R)
  ArtSensor M15End(8, ("B35"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М15 (B35) (тип R)
  ArtSensor M16Start(9, ("B36"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале М16 пикпойнт (B36) (тип BGS)
  ArtSensor M16Count(10, ("B37"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале М16 пикпойнт (B36) (тип BGS)
  ArtSensor M16End(11, ("B38"), 11, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце M16 пикпойнт (B37) (тип BGS)

  ArtCylinder PusherSV3(16, ("SV3"), 10, false, true, ArtCylinder::MONO_STABLE, 9,  &PusherNOHomeB31, &PusherHomeB30); //Пушер SV3

  ArtDriver M13DRV(12, ("M13DRV"), ArtDriver::DRIVER_TYPE_1, 12, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M13
  ArtDriver M14DRV(13, ("M14DRV"), ArtDriver::DRIVER_TYPE_1, 13, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M14
  ArtDriver M15DRV(14, ("M15DRV"), ArtDriver::DRIVER_TYPE_1, 14, 3, 3, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M15
  ArtDriver M16DRV(15, ("M16DRV"), ArtDriver::DRIVER_TYPE_1, 15, 4, 3, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M16

  ArtConveyor2Type ConvM16(13, ("ConvM16"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M16DRV, &M16Start, &M16Count, &M16End, 4000, 4, 4);
  ArtConveyor1Type ConvM15(14, ("ConvM15"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M15DRV, &M14End, &M15End, &ConvM16, 15000, 0);
  ArtConveyor1Type ConvM14(15, ("ConvM14"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M14DRV, &OnPusherM13, &M14End, &ConvM15, 8000,0);
  ArtConveyor1Type ConvM13(15, ("ConvM13"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M13DRV, &AfterSiatB28, &OnPusherM13, &ConvM14, 8000,0);
  //----setup for A35---------------------------------------------------------------------------------------
 */
  //----setup for A36---------------------------------------------------------------------------------------
  ArtSensor AfterSiatB39(1, ("B39"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик после Сиата 4 (B39) (тип R)
  ArtSensor M17End(5, ("B40"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М17 (B40) (тип R)
  ArtSensor M18End(6, ("B41"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М18 (B41) (тип R)
  ArtSensor M19Perepoln(7, ("B42"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик переполнения М19 (B42) (тип R)
  ArtSensor M19End(8, ("B43"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М19 (B43) (тип R)
  ArtSensor OnPusherM20(2, ("B44"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на пушере М20 (B44) (тип BGS)
  ArtSensor PusherHomeB45(3, ("B45"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 4 дома (B45) (тип геркон)
  ArtSensor PusherNOHomeB46(4, ("B46"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Датчик пушер 4 НЕ дома (B46) (тип геркон)
  ArtSensor M21Start(9, ("B47"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале М21 пикпойнт (B47) (тип BGS)
  ArtSensor M21Count(10, ("B48"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на начале M21 пикпойнт (B48) (тип BGS)
  ArtSensor M21End(11, ("B49"), 11, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false); // Фотодатчик на конце М21 пикпойнт (B49) (тип BGS)

  ArtDriver M17DRV(12, ("M17DRV"), ArtDriver::DRIVER_TYPE_1, 12, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M17
  ArtDriver M18DRV(13, ("M18DRV"), ArtDriver::DRIVER_TYPE_1, 13, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M18
  ArtDriver M19DRV(14, ("M19DRV"), ArtDriver::DRIVER_TYPE_1, 14, 3, 3, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M19
  ArtDriver M20DRV(15, ("M20DRV"), ArtDriver::DRIVER_TYPE_1, 15, 4, 4, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M20
  ArtDriver M21DRV(16, ("M21DRV"), ArtDriver::DRIVER_TYPE_1, 16, 5, 5, true, 0, 2, 0, 2, 0, 0, 0);//Драйвер на M21

  ArtConveyor2Type ConvM21(13, ("ConvM16"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M21DRV, &M21Start, &M21Count, &M21End, 4000, 4, 4);
  ArtConveyor1Type ConvM20(14, ("ConvM15"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M20DRV, &M19End, &OnPusherM20, &ConvM21, 15000, 0);
  ArtConveyor1AType ConvM19(15, ("ConvM14"), ArtConveyor1Type::CONVEYOR_TYPE_1A, &M19DRV, &M18End, &M19End, &ConvM20, 8000,0,3);
  ArtConveyor1Type ConvM18(15, ("ConvM13"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M18DRV, &M17End, &M18End, &ConvM19, 8000,0);
  ArtConveyor1Type ConvM17(15, ("ConvM13"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M17DRV, &AfterSiatB39, &M17End, &ConvM18, 8000,0);
  
  ArtCylinder PusherSV4(17, ("SV4"), 10, false, true, ArtCylinder::MONO_STABLE, 11,  &PusherNOHomeB46, &PusherHomeB45); //Пушер SV1
  ArtPusher Pusher(18,("SV4"),&OnPusherM20,&ConvM20,&ConvM21,&PusherSV4);
  //----setup for A36---------------------------------------------------------------------------------------
  
/*
  //----setup for Dispenser---------------------------------------------------------------------------------------
  ArtSensor PallONConvey(1, ("PallONConvey"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor PalletsInStack(2, ("PalletsInStack"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor Clamp1Open(3, ("Clamp1Open"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor Clamp1Close(4, ("Clamp1Close"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor Clamp2Open(5, ("Clamp2Open"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor Clamp2Close(6, ("Clamp2Close"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor TOPCylinderOpen(7, ("TOPCylinderOpen"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor TOPCylinderClose(8, ("TOPCylinderClose"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor BOTCylinderOpen(9, ("BOTCylinderOpen"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);
  ArtSensor BOTCylinderClose(10, ("BOTCylinderClose"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0,false);

  ArtCylinder Clamp1(11, ("Clamp1"), 10, 10, false, true, ArtCylinder::MONO_STABLE,
                     16, 5, &Clamp1Open, &Clamp1Close);

  ArtCylinder Clamp2(12, ("Clamp2"), 10, 10, false, true, ArtCylinder::MONO_STABLE,
                     16, 5, &Clamp2Open, &Clamp2Close);

  ArtCylinder TOPCylinder(13, ("TOPCylinder"), 10, 10, false, true,
                          ArtCylinder::BI_STABLE, 2, 1, &TOPCylinderOpen,
                          &TOPCylinderClose);

  ArtCylinder BOTCylinder(14, ("BOTCylinder"), 10, 10, false, true,
                          ArtCylinder::BI_STABLE, 4, 3, &BOTCylinderOpen,
                          &BOTCylinderClose);

  PalletMagazine Dispenser(15, ("Dispenser"), &Clamp1, &Clamp2, &TOPCylinder,
                           &BOTCylinder, 12, &PallONConvey, &PalletsInStack, 11, 16);
//----setup for Dispenser---------------------------------------------------------------------------------------
*/
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
   // EASYCAT.BufferIn.Cust.ConvReadySignal = ConvM4.ConveyorGetReadyReceive();
    //EASYCAT.BufferIn.Cust.SensSignalOnNextBarda = M3End.SensorState()<<(M3End.SensorInput-1);
  }
}


void loop()
{
}

