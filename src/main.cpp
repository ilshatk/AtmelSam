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
int timer1 = 0, timer2 = 0, timer3 = 0, curtime = 0;
bool curOut1 = false;
bool curOut2 = false;
bool curOut3 = false;

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
  /*
//----setup for A31 2-nd Line---------------------------------------------------------------------------------------
ArtSensor AfterSiat(1, ("B1"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Фотодатчик после Сиата 1 (B1) (тип R)

ArtSensor M1End(2, ("B2"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Фотодатчик на конце М1 (B2) (тип R)
ArtSensor M2End(3, ("B3"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Фотодатчик на конце М2 (B3) (тип R)

ArtSensor M3Perepoln(4, ("B4"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Фотодатчик переполнения М3 (B4) (тип R)
ArtSensor M3End(5, ("B5"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);      // Фотодатчик на конце М3 (B5) (тип R)

ArtDriver M1DRV(5, ("M1DRV"), ArtDriver::DRIVER_TYPE_1, 6, 1, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);  //Драйвер на M1
ArtDriver M2DRV(10, ("M2DRV"), ArtDriver::DRIVER_TYPE_1, 7, 2, 2, 2, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M2
ArtDriver M3DRV(11, ("M3DRV"), ArtDriver::DRIVER_TYPE_1, 8, 4, 3, 3, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M3

ArtConveyor1AType ConvM3(12, ("ConvM3"), ArtConveyor1AType::CONVEYOR_TYPE_1_EXTERNAL_BARDA, &M3DRV, &M2End, &M3End, false, 30000, 0, 4, 3);
ArtConveyor1Type ConvM2(13, ("ConvM2"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M2DRV, &M1End, &M2End, &ConvM3, 20000, 0, 2);
ArtConveyor1Type ConvM1(14, ("ConvM1"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M1DRV, &AfterSiat, &M1End, &ConvM2, 20000, 0, 1);
//----setup for A31 2-nd Line---------------------------------------------------------------------------------------

//----setup for A32 2-nd Line---------------------------------------------------------------------------------------
//Эта A32 барда берет сигнал с датчика B5 с предыдущей барды поэтому в ArtIOClass распарсиваются входы
//с предыдущей, также там добавлен сброс ошибки драйвера
/*
  ArtSensor B5(1, ("B5"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true); // Фотодатчик на конце М3 (B5) (тип R)

  ArtSensor B6(2, ("B6"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 500, false);         // Фотодатчик на пушере М4 (B6) (тип BGS)
  ArtSensor PushHome(3, ("B7"), 2, ArtSensor::SENSOR_TYPE_BASIC, 100, 0, false); // Датчик пушер 1 дома (B7) (тип геркон)
  ArtSensor PushHome_2(3, ("B7"), 2, ArtSensor::SENSOR_TYPE_INVERSE, 0, 0, false);
  ArtSensor PushNoHome(4, ("B8"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Датчик пушер 1 НЕ дома (B8) (тип геркон)

  ArtSensor M5Perepoln(5, ("B9"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Фотодатчик переполнения М5 (B9) (тип R)
  ArtSensor M5End(6, ("B10"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);     // Фотодатчик на конце М5 (B10) (тип R)

  ArtSensor M6Count(7, ("B11"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 300, false); // Фотодатчик на начале М6 пикпойнт (B11) (тип BGS)
  ArtSensor M6Start2(8, ("B12"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 300, false);  // Фотодатчик на начале M6 пикпойнт (B12) (тип BGS)
  ArtSensor M6End(9, ("B13"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);     // Фотодатчик на конце М6 пикпойнт (B13) (тип BGS)

  ArtCylinder PusherSV1(16, ("SV1"), 10000, false, true, ArtCylinder::MONO_STABLE, 7, &PushNoHome, &PushHome, 0, 0); //Пушер SV1

  ArtDriver M4DRV(10, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 9, 1, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);  //Драйвер на M4
  ArtDriver M5DRV(11, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 10, 2, 2, 2, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M5
  ArtDriver M6DRV(12, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 11, 4, 3, 3, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M6

  ArtConveyor2Type ConvM6(13, ("Conveyor3"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M6DRV, &M5End, &M6Start2, &M6Count, &M6End, 12000, 0, 100, 3);
  ArtConveyor1AType ConvM5(14, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1A, &M5DRV, &PushHome_2, &M5End, &ConvM6, 30000, 0, 2, 2);
  ArtConveyorWithPusher ConvM13(15, ("ConvM14"), ArtConveyor1Type::CONVEYOR_TYPE_WITH_PUSHER, &M4DRV, &B5, &PushHome_2,
                                &B6, &PusherSV1, &ConvM5, 100000, 0, 1, &PushHome, &PushNoHome); //проверить как работает
  //----setup for A32 2-nd Line---------------------------------------------------------------------------------------
  /*
//----setup for A33 1-st Line---------------------------------------------------------------------------------------
ArtSensor AfterSiatB14(1, ("B14"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                // Фотодатчик после Сиата 2 (B14) (тип R)
ArtSensor M7Perepoln(2, ("B15"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                  // Фотодатчик переполнения М7 (B15) (тип R)
ArtSensor M7End(3, ("B16"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                       // Фотодатчик на конце М7 (B16) (тип R)
ArtSensor M8End(4, ("B17"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                       // Фотодатчик на конце М8 (B17) (тип R)
ArtSensor M9Perepoln(5, ("B18"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                  // Фотодатчик переполнения М9 (B18) (тип R)
ArtSensor M9End(6, ("B19"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                       // Фотодатчик на конце М9 (B19) (тип R)
ArtDriver M7DRV(10, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 7, 1, 1, 1, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M4
ArtDriver M8DRV(11, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 8, 2, 2, 2, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M5
ArtDriver M9DRV(12, ("M4DRV"), ArtDriver::DRIVER_TYPE_1, 9, 3, 3, 3, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M6
ArtConveyor1AType ConvM9(13, ("ConvM9"), ArtConveyor1AType::CONVEYOR_TYPE_1_EXTERNAL_BARDA, &M9DRV, &M8End, &M9End, false, 60000, 4, 4, 3);
ArtConveyor1Type ConvM8(14, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M8DRV, &M7End, &M8End, &ConvM9, 20000, 0, 2);
ArtConveyor1Type ConvM7(15, ("ConvM4"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M7DRV, &AfterSiatB14, &M7End, &ConvM8, 20000, 0, 1);
//----setup for A33 1-st Line---------------------------------------------------------------------------------------
/*
//----setup for A34 1-st Line---------------------------------------------------------------------------------------------------------------------------
ArtSensor M9End(1, ("B19"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true);        // Фотодатчик на конце М9 (B19) (тип R) с ПРЕДЫДУЩЕЙ БАРДЫ
ArtSensor OnPusherM10(2, ("B20"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 500, false); // Фотодатчик на пушере М10 (B20) (тип BGS)
ArtSensor PusherHome(3, ("B21"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);
ArtSensor PusherHome_Inverse(3, ("B21"), 2, ArtSensor::SENSOR_TYPE_INVERSE, 0, 0, false);                           // Датчик пушер 2 дома (B21) (тип геркон)
ArtSensor PusherNOHome(4, ("B22"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                                   // Датчик пушер 2 НЕ дома (B22) (тип геркон)
ArtSensor M11Perepoln(5, ("B23"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                                    // Фотодатчик переполнения М11 (B23) (тип R)
ArtSensor M11End(6, ("B24"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                                         // Фотодатчик на конце М11 (B24) (тип R)
ArtSensor M12Start1(7, ("B25"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 300, false);                                      // Фотодатчик на начале М12 пикпойнт (B25) (тип BGS)
ArtSensor M12Count(8, ("B26"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 300, false);                                       // Фотодатчик на начале М12 пикпойнт (B26) (тип BGS)
ArtSensor M12End(9, ("B26"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);                                         // Фотодатчик на конце М12 пикпойнт (B27) (тип BGS)
ArtCylinder PusherSV2(16, ("SV2"), 10, false, true, ArtCylinder::MONO_STABLE, 7, &PusherNOHome, &PusherHome, 0, 0); //Пушер SV2
ArtDriver M10DRV(10, ("M10DRV"), ArtDriver::DRIVER_TYPE_1, 9, 1, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);                  //Драйвер на M10
ArtDriver M11DRV(11, ("M11DRV"), ArtDriver::DRIVER_TYPE_1, 10, 2, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);                 //Драйвер на M11
ArtDriver M12DRV(12, ("M12DRV"), ArtDriver::DRIVER_TYPE_1, 11, 3, 3, 3, true, 0, 2, 0, 2, 0, 0, 0);                 //Драйвер на M12
ArtConveyor2Type ConvM12(13, ("ConvM12"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M12DRV, &M11End, &M12Start1, &M12Count, &M12End, 12000, 0, 4, 3);
ArtConveyor1Type ConvM11(14, ("ConvM11"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M11DRV, &PusherHome_Inverse, &M11End, &ConvM12, 15000, 0, 2);
//ArtConveyor1Type ConvM10(15, ("ConvM10"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M10DRV, &M9End, &OnPusherM10, &ConvM11, 8000, 0, 1);
//ArtPusher PusherOnM10(16, ("Podzhim2"), &OnPusherM10, &M9End, &ConvM11, &PusherSV2, &M10DRV);
 ArtConveyorWithPusher ConvM10(15, ("ConvM14"), ArtConveyor1Type::CONVEYOR_TYPE_WITH_PUSHER, &M10DRV, &M9End, &PusherHome_Inverse,
                                &OnPusherM10, &PusherSV2, &ConvM11, 100000, 0, 1,&PusherHome,&PusherNOHome);
//----setup for A34 1-st Line---------------------------------------------------------------------------------------
/*
  //----setup for A35 3-rd Line-------------------------------------------------------------------------------------------------------------------------------------------------------
  ArtSensor AfterSiatB28(1, ("B28"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);    // Фотодатчик после Сиата 3 (B28) (тип R)
  ArtSensor OnPusherM13(2, ("B29"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 500, false);     // Фотодатчик на пушере М13 (B29) (тип BGS)
  ArtSensor PusherHomeB30(3, ("B30"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Датчик пушер 3 дома (B30) (тип геркон)
  ArtSensor PusherNOHomeB31(4, ("B31"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Датчик пушер 3 НЕ дома (B31) (тип геркон)
  ArtSensor M14Perepoln(5, ("B32"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);     // Фотодатчик переполнения М14 (B32) (тип R)
  ArtSensor M14End(6, ("B33"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);          // Фотодатчик на конце М14 (B33) (тип R)

  ArtSensor M15Perepoln(7, ("B34"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Фотодатчик переполнения М15 (B34) (тип R)
  ArtSensor M15End(8, ("B35"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);      // Фотодатчик на конце М15 (B35) (тип R)

  ArtSensor M16Start(9, ("B36"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 300, false);   // Фотодатчик на начале М16 пикпойнт (B36) (тип BGS)
  ArtSensor M16Count(10, ("B37"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 300, false); // Фотодатчик на начале М16 пикпойнт (B36) (тип BGS)
  ArtSensor M16End(11, ("B38"), 11, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Фотодатчик на конце M16 пикпойнт (B37) (тип BGS)
  ArtSensor PusherHomeB30_Inverse(3, ("B30"), 3, ArtSensor::SENSOR_TYPE_INVERSE, 0, 0, false);

  ArtCylinder PusherSV3(16, ("SV3"), 10, false, true, ArtCylinder::MONO_STABLE, 9, &PusherNOHomeB31, &PusherHomeB30, 0, 0); //Пушер SV3

  ArtDriver M13DRV(12, ("M13DRV"), ArtDriver::DRIVER_TYPE_1, 12, 1, 1, 1, true, 0, 2, 0, 2, 0, 0, 0);  //Драйвер на M13
  ArtDriver M14DRV(13, ("M14DRV"), ArtDriver::DRIVER_TYPE_1, 13, 2, 2, 2, true, 0, 2, 0, 2, 0, 0, 0);  //Драйвер на M14 переделать в накопительный
  ArtDriver M15DRV(14, ("M15DRV"), ArtDriver::DRIVER_TYPE_1, 14, 3, 10, 3, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M15
  ArtDriver M16DRV(15, ("M16DRV"), ArtDriver::DRIVER_TYPE_1, 15, 4, 4, 3, true, 0, 2, 0, 2, 0, 0, 0);  //Драйвер на M16

  ArtConveyor2Type ConvM16(13, ("ConvM16"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M16DRV, &M15End, &M16Start, &M16Count, &M16End, 12000, 0, 4, 4);
  ArtConveyor1Type ConvM15(14, ("ConvM15"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M15DRV, &M14End, &M15End, &ConvM16, 26000, 0, 3);
  ArtConveyor1AType ConvM14(15, ("ConvM14"), ArtConveyor1Type::CONVEYOR_TYPE_1A, &M14DRV, &PusherHomeB30_Inverse, &M14End, &ConvM15, 20000, 0, 10, 2);
  ArtConveyorWithPusher ConvM13(15, ("ConvM14"), ArtConveyor1Type::CONVEYOR_TYPE_WITH_PUSHER, &M13DRV, &AfterSiatB28, &PusherHomeB30_Inverse,
                                &OnPusherM13, &PusherSV3, &ConvM14, 100000, 0, 1, &PusherHomeB30, &PusherNOHomeB31);
  //----setup for A35 3-rd Line---------------------------------------------------------------------------------------
  /*
  //----setup for A36 4-th Line---------------------------------------------------------------------------------------------------------------------------------------------------
  ArtSensor AfterSiatB39(1, ("B39"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);  // Фотодатчик после Сиата 4 (B39) (тип R)

  ArtSensor M17End(5, ("B40"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);        // Фотодатчик на конце М17 (B40) (тип R)

  ArtSensor M18End(6, ("B41"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);        // Фотодатчик на конце М18 (B41) (тип R)

  ArtSensor M19Perepoln(7, ("B42"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Фотодатчик переполнения М19 (B42) (тип R)
  ArtSensor M19End(8, ("B43"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);        // Фотодатчик на конце М19 (B43) (тип R)

  ArtSensor OnPusherM20(2, ("B44"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 500, false);   // Фотодатчик на пушере М20 (B44) (тип BGS)
  ArtSensor PusherHomeB45(3, ("B45"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Датчик пушер 4 дома (B45) (тип геркон)
  ArtSensor PusherHomeB45_Inverse(3, ("B45"), 7, ArtSensor::SENSOR_TYPE_INVERSE, 0, 0, false);
  ArtSensor PusherNOHomeB46(4, ("B46"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Датчик пушер 4 НЕ дома (B46) (тип геркон)

  ArtSensor M21Start(9, ("B47"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 300, false);        // Фотодатчик на начале М21 пикпойнт (B47) (тип BGS)
  ArtSensor M21Count(10, ("B48"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 300, false);    // Фотодатчик на начале M21 пикпойнт (B48) (тип BGS)
  ArtSensor M21End(11, ("B49"), 11, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);        // Фотодатчик на конце М21 пикпойнт (B49) (тип BGS)

  ArtDriver M17DRV(12, ("M17DRV"), ArtDriver::DRIVER_TYPE_1, 12, 1, 1, 1, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M17
  ArtDriver M18DRV(13, ("M18DRV"), ArtDriver::DRIVER_TYPE_1, 13, 2, 2, 2, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M18
  ArtDriver M19DRV(14, ("M19DRV"), ArtDriver::DRIVER_TYPE_1, 14, 3, 3, 3, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M19
  ArtDriver M20DRV(15, ("M20DRV"), ArtDriver::DRIVER_TYPE_1, 15, 4, 4, 4, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M20
  ArtDriver M21DRV(16, ("M21DRV"), ArtDriver::DRIVER_TYPE_1, 16, 5, 5, 5, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M21

  ArtCylinder PusherSV4(17, ("SV4"), 10, false, true, ArtCylinder::MONO_STABLE, 11, &PusherNOHomeB46, &PusherHomeB45, 0, 0); //Пушер SV1

  ArtConveyor2Type ConvM21(13, ("ConvM16"), ArtConveyor2Type::CONVEYOR_TYPE_2, &M20DRV, &PusherHomeB45_Inverse, &M21Start, &M21Count,
                           &M21End, 12000, 0, 4, 4);
  ArtConveyorWithPusher ConvM20(15, ("ConvM14"), ArtConveyor1Type::CONVEYOR_TYPE_WITH_PUSHER, &M19DRV, &M19End, &PusherHomeB45_Inverse,
                                &OnPusherM20, &PusherSV4, &ConvM21, 10000, 0, 3, &PusherHomeB45, &PusherNOHomeB46);

  ArtConveyor1AType ConvM19(15, ("ConvM13"), ArtConveyor1Type::CONVEYOR_TYPE_1A, &M18DRV, &M17End, &M19End, &ConvM20, 30000, 0, 5, 2);
  ArtConveyor1Type ConvM17(15, ("ConvM13"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M17DRV, &AfterSiatB39, &M17End, &ConvM19, 10000, 0, 1);

  //----setup for A36 4-th Line---------------------------------------------------------------------------------------
  /*
  //----setup for A37 цепной конвейер после диспенсера-----------------------------------------------------
  //принимаем input с 38 барды
  ArtSensor M24DOWN(1, ("B54"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true); // М24 внизу (B54) (тип геркон)
  ArtSensor M25DOWN(3, ("B65"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true); //  М25 внизу (B66) (тип геркон)
  ArtSensor M26DOWN(5, ("B76"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true); // М26 внизу (B76) (тип геркон)
  ArtSensor M27DOWN(7, ("B87"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true); // М27 внизу (B87) (тип геркон)

  ArtSensor PalletOnM24(99, ("B51"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 1000, false); // Фотодатчик паллеты на М24 (B51) (тип BGS G10) подобрать delayFE
  ArtSensor PalletOnM25(98, ("B62"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 1000, false); // Фотодатчик паллеты на М25 (B62) (тип BGS G10) подобрать delayFE
  ArtSensor PalletOnM26(97, ("B73"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 1000, false); // Фотодатчик паллеты на М26 (B73) (тип BGS G10) подобрать delayFE
  ArtSensor PalletOnM27(96, ("B84"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 1000, false); // Фотодатчик паллеты на М27 (B84) (тип BGS G10) подобрать delayFE

  ArtSensor Stopper1DOWN(6, ("B52"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Стопер 1 внизу (B52) (тип геркон)
  ArtSensor Stopper1UP(7, ("B53"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);     // Стопер 1 наверху (B53) (тип геркон)
  ArtSensor Stopper3DOWN(8, ("B63"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Стопер 3 внизу (B63) (тип геркон)
  ArtSensor Stopper3UP(9, ("B64"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);     // Стопер 3 наверху (B64) (тип геркон)
  ArtSensor Stopper5DOWN(10, ("B74"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Стопер 5 внизу (B74) (тип геркон)
  ArtSensor Stopper5UP(11, ("B75"), 11, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Стопер 5 наверху (B75) (тип геркон)
  ArtSensor Stopper7DOWN(12, ("B85"), 12, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Стопер 7 внизу (B85) (тип геркон)
  ArtSensor Stopper7UP(13, ("B86"), 13, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Стопер 7 наверху (B86) (тип геркон)

  ArtCylinder Stopper1(14, ("SV4"), 10, false, true, ArtCylinder::MONO_STABLE, 1, &Stopper1UP, &Stopper1DOWN, 2000, 2000); //Пушер SV1
  ArtCylinder Stopper3(15, ("SV4"), 10, false, true, ArtCylinder::MONO_STABLE, 2, &Stopper3UP, &Stopper3DOWN, 2000, 2000); //Пушер SV1
  ArtCylinder Stopper5(16, ("SV4"), 10, false, true, ArtCylinder::MONO_STABLE, 3, &Stopper5UP, &Stopper5DOWN, 2000, 2000); //Пушер SV1
  ArtCylinder Stopper7(17, ("SV4"), 10, false, true, ArtCylinder::MONO_STABLE, 4, &Stopper7UP, &Stopper7DOWN, 2000, 2000); //Пушер SV1

  ArtDriver M22DRV(18, ("M22DRV"), ArtDriver::DRIVER_TYPE_1, 15, 1, 13, 6, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M22
  ArtDriver M23DRV(19, ("M23DRV"), ArtDriver::DRIVER_TYPE_1, 16, 2, 14, 6, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M23

  ArtCrossConveyor ConvM23_4(20, ("ConvM23"), ArtBasicConveyor::CONVEYOR_TYPE_CROSS, &M23DRV, &M22DRV,
                             &PalletOnM27, &Stopper7, 20000, 0, &M27DOWN, 4, true);
  ArtCrossConveyor ConvM23_3(20, ("ConvM23"), ArtBasicConveyor::CONVEYOR_TYPE_CROSS, &M23DRV, &M22DRV,
                             &ConvM23_4, &PalletOnM26, &Stopper5, 20000, 0, &M26DOWN, 3, false);
  ArtCrossConveyor ConvM23_2(20, ("ConvM23"), ArtBasicConveyor::CONVEYOR_TYPE_CROSS, &M23DRV, &M22DRV,
                             &ConvM23_3, &PalletOnM25, &Stopper3, 20000, 0, &M25DOWN, 2, false);
  ArtCrossConveyor ConvM23_1(20, ("ConvM23"), ArtBasicConveyor::CONVEYOR_TYPE_CROSS, &M23DRV, &M22DRV,
                             &ConvM23_2, &PalletOnM24, &Stopper1, 20000, 0, &M24DOWN, 1, false);

  //----setup for A37 цепной конвейер после диспенсера----------------------------------------------------------------------------------------
  /*
  //----setup for A38 лифты паллет на цепном конвейере----------------------------------------------------------------------------------------
  ArtSensor M28Pall(54, ("B56"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1); // М28 датчик на конце следующего конвейера на следующей барде
  ArtSensor M29Pall(55, ("B67"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1); // М29 датчик на конце следующего конвейера на следующей барде
  ArtSensor M30Pall(56, ("B78"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 2); // М30 датчик на конце следующего конвейера на следующей барде
  ArtSensor M31Pall(57, ("B89"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 2); // М31 датчик на конце следующего конвейера на следующей барде

  ArtSensor M24DOWN(1, ("B54"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // М24 внизу (B54) (тип геркон)
  ArtSensor M24UP(2, ("B55"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // М24 наверху (B55) (тип геркон)
  ArtSensor M25DOWN(3, ("B65"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); //  М25 внизу (B66) (тип геркон)
  ArtSensor M25UP(4, ("B66"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // М25 наверху (B67) (тип геркон)
  ArtSensor M26DOWN(5, ("B76"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // М26 внизу (B76) (тип геркон)
  ArtSensor M26UP(6, ("B77"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // М26 наверху (B77) (тип геркон)
  ArtSensor M27DOWN(7, ("B87"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // М27 внизу (B87) (тип геркон)
  ArtSensor M27UP(8, ("B88"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // М27 наверху (B88) (тип геркон)

  ArtDriver M24DRV(9, ("M24DRV"), ArtDriver::DRIVER_TYPE_1, 13, 1, 9, 6, true, 2, 2, 2, 2, 2, 2, 2);   //Драйвер на M24
  ArtDriver M25DRV(10, ("M25DRV"), ArtDriver::DRIVER_TYPE_1, 14, 2, 11, 6, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M25
  ArtDriver M26DRV(11, ("M26DRV"), ArtDriver::DRIVER_TYPE_1, 15, 4, 13, 6, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M26
  ArtDriver M27DRV(12, ("M27DRV"), ArtDriver::DRIVER_TYPE_1, 16, 8, 15, 6, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M27

  ArtCylinder Lift1(13, ("Lift1"), 10, false, true, ArtCylinder::MONO_STABLE, 1, &M24UP, &M24DOWN, 2000, 2000); //Пушер SV1
  ArtCylinder Lift2(14, ("Lift2"), 10, false, true, ArtCylinder::MONO_STABLE, 2, &M25UP, &M25DOWN, 2000, 2000); //Пушер SV1
  ArtCylinder Lift3(15, ("Lift3"), 10, false, true, ArtCylinder::MONO_STABLE, 3, &M26UP, &M26DOWN, 2000, 2000); //Пушер SV1
  ArtCylinder Lift4(16, ("Lift4"), 10, false, true, ArtCylinder::MONO_STABLE, 4, &M27UP, &M27DOWN, 2000, 2000); //Пушер SV1

  ArtConveyorWithLiftType1 ConvM24(21, ("ConvM24"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M24DRV, &Lift1, &M24UP, &M24DOWN, &M28Pall, 500000, 2000, 2000, 0, 1, 1);
  ArtConveyorWithLiftType1 ConvM25(21, ("ConvM24"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M25DRV, &Lift2, &M25UP, &M25DOWN, &M29Pall, 500000, 2000, 2000, 0, 2, 2);
  ArtConveyorWithLiftType1 ConvM26(21, ("ConvM24"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M26DRV, &Lift3, &M26UP, &M26DOWN, &M30Pall, 500000, 2000, 2000, 0, 3, 3);
  ArtConveyorWithLiftType1 ConvM27(21, ("ConvM24"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M27DRV, &Lift4, &M27UP, &M27DOWN, &M31Pall, 500000, 2000, 2000, 0, 4, 4);
  //входы с A39, A40 и A37 передать сюда
  //----setup for A38 лифты паллет на цепном конвейере----------------------------------------------------------------------------------------
  /*
//----setup for A39 PLP 1,2----------------------------------------------------------------------------------------
//
ArtSensor EnterShuttle(1, ("B95"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 2); //с шаттла

ArtSensor M24DOWN(1, ("B54"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1); // М24 внизу (B54) (тип геркон)
ArtSensor M24UP(2, ("B55"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1);   // М24 наверху (B55) (тип геркон)

ArtSensor M24DOWNInverse(1, ("B54"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1); // М24 внизу (B54) (тип геркон)

ArtSensor M25DOWN(3, ("B65"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1); //  М25 внизу (B66) (тип геркон)
ArtSensor M25UP(4, ("B66"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1);   // М25 наверху (B67) (тип геркон)

ArtSensor M28Pall(1, ("B56"), 1, ArtSensor::SENSOR_TYPE_BASIC, 800, 0, false); // Датчик паллеты на М28 (B56) (тип Инд)
ArtSensor M28Layer(2, ("B57"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);  // Фотодатчик слоя на М28 (B57) (тип R)

ArtSensor Stopper2DOWN(3, ("B58"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Стопер 2 внизу (B58) (тип геркон)
ArtSensor Stopper2UP(4, ("B59"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Стопер 2 наверху (B59) (тип геркон)

ArtSensor Podzhim1HOME(5, ("B60"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Поджим 1 ДОМА (B60) (тип геркон)
ArtSensor Podzhim1NOHOME(6, ("B61"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Поджим 1 ОТКРЫТ (B61) (тип геркон)

ArtSensor M29Pall(8, ("B67"), 8, ArtSensor::SENSOR_TYPE_BASIC, 800, 0, false); // Датчик паллеты на М29 (B67) (тип Инд)
ArtSensor M29Layer(9, ("B68"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);  // Фотодатчик слоя на М29 (B68) (тип R)

ArtSensor Stopper4DOWN(10, ("B69"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Стопер 4 внизу (B69) (тип геркон)
ArtSensor Stopper4UP(11, ("B70"), 11, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Стопер 4 наверху (B70) (тип геркон)

ArtSensor Podzhim2HOME(12, ("B71"), 12, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Поджим 2 ДОМА (B71) (тип геркон)
ArtSensor Podzhim2NOHOME(13, ("B72"), 13, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Поджим 2 ОТКРЫТ (B72) (тип геркон)

ArtCylinder Podzhim1(1, ("Podzhim1"), 10, false, true, ArtCylinder::MONO_STABLE, 2, &Podzhim1NOHOME, &Podzhim1HOME, 2000, 2000); //Пушер SV1
ArtCylinder Podzhim2(2, ("Podzhim2"), 10, false, true, ArtCylinder::MONO_STABLE, 4, &Podzhim2NOHOME, &Podzhim2HOME, 2000, 2000); //Пушер SV1

ArtCylinder Stopper1(5, ("Stopper1"), 10, false, true, ArtCylinder::MONO_STABLE, 1, &Stopper2UP, &Stopper2DOWN, 2000, 2000); //Пушер SV1 переделать если не будет корректно работать
ArtCylinder Stopper2(6, ("Stopper2"), 10, false, true, ArtCylinder::MONO_STABLE, 3, &Stopper4UP, &Stopper4DOWN, 2000, 2000); //Пушер SV1

ArtDriver M28DRV(7, ("M28DRV"), ArtDriver::DRIVER_TYPE_1, 15, 1, 13, 6, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M28
ArtDriver M29DRV(8, ("M29DRV"), ArtDriver::DRIVER_TYPE_1, 16, 2, 15, 6, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M29

ArtConveyorPLPType ConvM28(9, ("ConvM30"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M28DRV, &M24UP, &EnterShuttle, &M28Pall, &M28Layer,
&Stopper1, &Podzhim1, 50000, 0, 1, 1, &Stopper2DOWN, &Podzhim1HOME);
ArtConveyorPLPType ConvM29(10, ("ConvM31"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M29DRV, &M25UP, &EnterShuttle, &M29Pall, &M29Layer,
&Stopper2, &Podzhim2, 50000, 0, 2, 2, &Stopper4DOWN, &Podzhim2HOME);
//----setup for A39----------------------------------------------------------------------------------------
/*
//----setup for A40 PLP 3,4--------------------------------------------------------------------------------

ArtSensor EnterShuttle(26, ("B95"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 2); //с шаттла

ArtSensor M26DOWN(27, ("B76"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1);      // М26 внизу (B76) (тип геркон)
ArtSensor M26UP(28, ("B77"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1);        // М26 наверху (B77) (тип геркон)

ArtSensor M27DOWN(29, ("B87"), 7, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1);      // М27 внизу (B87) (тип геркон)
ArtSensor M27UP(30, ("B88"), 8, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true, 1);        // М27 наверху (B88) (тип геркон)

ArtSensor M30Pall(1, ("B78"), 1, ArtSensor::SENSOR_TYPE_BASIC, 800, 0, false);        // Датчик паллеты на М30 (B78) (тип Инд)
ArtSensor M30Layer(2, ("B79"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);         // Фотодатчик слоя на М30 (B79) (тип R)

ArtSensor Stopper6DOWN(3, ("B80"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);     // Стопер 6 внизу (B80) (тип геркон)
ArtSensor Stopper6UP(4, ("B81"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);       // Стопер 6 наверху (B81) (тип геркон)

ArtSensor Podzhim3HOME(5, ("B82"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);     // Поджим 3 ДОМА (B82) (тип геркон)
ArtSensor Podzhim3NOHOME(6, ("B83"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Поджим 3 ОТКРЫТ (B83) (тип геркон)

ArtSensor M31Pall(8, ("B89"), 8, ArtSensor::SENSOR_TYPE_BASIC, 800, 0, false);        // Датчик паллеты на М31 (B89) (тип Инд)
ArtSensor M31Layer(9, ("B90"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);         // Фотодатчик слоя на М31 (B90) (тип R)

ArtSensor Stopper8DOWN(10, ("B91"), 11, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Стопер 8 внизу (B91) (тип геркон)
ArtSensor Stopper8UP(11, ("B92"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);     // Стопер 8 наверху (B92) (тип геркон)

ArtSensor Podzhim4HOME(12, ("B93"), 12, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Поджим 4 ДОМА (B93) (тип геркон)
ArtSensor Podzhim4NOHOME(13, ("B94"), 13, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Поджим 4 ОТКРЫТ (B94) (тип геркон)

ArtCylinder Podzhim3(14, ("Podzhim3"), 10, false, true, ArtCylinder::MONO_STABLE, 2, &Podzhim3NOHOME, &Podzhim3HOME, 2000, 2000); //Пушер SV1
ArtCylinder Podzhim4(16, ("Podzhim4"), 10, false, true, ArtCylinder::MONO_STABLE, 4, &Podzhim4NOHOME, &Podzhim4HOME, 2000, 2000); //Пушер SV1

ArtCylinder Stopper3(18, ("Stopper3"), 10, false, true, ArtCylinder::MONO_STABLE, 1, &Stopper6UP, &Stopper6DOWN, 2000, 2000); //Пушер SV1
ArtCylinder Stopper4(20, ("Stopper4"), 10, false, true, ArtCylinder::MONO_STABLE, 3, &Stopper8UP, &Stopper8DOWN, 2000, 2000); //Пушер SV1

ArtDriver M30DRV(22, ("M30DRV"), ArtDriver::DRIVER_TYPE_1, 15, 1, 13, 6, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M30
ArtDriver M31DRV(23, ("M31DRV"), ArtDriver::DRIVER_TYPE_1, 16, 2, 15, 6, true, 0, 2, 0, 2, 0, 0, 0); //Драйвер на M31

ArtConveyorPLPType ConvM30(24, ("ConvM30"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M30DRV, &M26UP, &EnterShuttle, &M30Pall, &M30Layer,
&Stopper3, &Podzhim3, 50000, 0, 3, 1, &Stopper6DOWN, &Podzhim3HOME);
ArtConveyorPLPType ConvM31(25, ("ConvM31"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M31DRV, &M27UP, &EnterShuttle, &M31Pall, &M31Layer,
&Stopper4, &Podzhim4, 50000, 0, 4, 2, &Stopper8DOWN, &Podzhim4HOME); //
  //----setup for A40---------------------------------------------------------------------------------------------------
  /*
  
  //----setup for A41--Shuttle and CO-------------------------------------------------------------------------------------
  ArtSensor EnterShuttle(1, ("B95"), 1, ArtSensor::SENSOR_TYPE_BASIC, 100, 0, false);          // Фотодатчик наличия паллеты на входе шаттла (B95) (тип R)
  ArtSensor RangeFinderDiscrete(2, ("B96"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);   // Фотодатчик дальномер шаттла (B96) (тип лазер BGS) (дискр выход)
  ArtSensor PalletOnConvM32(3, ("B97"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 2000, false); // Фотодатчик паллеты на М32 (B97) (тип R)
  ArtSensor EdgeOnShuttle(4, ("B98"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);         // Датчик крайних положений шаттла (B98) (тип Мех)
  ArtSensor ExitShuttle(5, ("B99"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);           // Фотодатчик наличия паллеты на выходе шаттла (B99) (тип R)
  ArtSensor PalletOnConvM34(6, ("B100"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);      // Датчик паллеты на М34 (B100) (тип Инд)
  ArtAnalogSensor RangeFinderAnalog(7, ("ConvM16"), 1);                                      //Фотодатчик дальномер шаттла (B96) (тип лазер BGS) (аналог выход)

  ArtDriver ShuttleDrv(8, ("M33DRV"), ArtDriver::DRIVER_TYPE_1, 15, 1, 11, 12, true, 2, 2, 2, 2, 2, 2, 2);   //Драйвер на M33
  ArtDriver OverShuttleDrv(9, ("M32DRV"), ArtDriver::DRIVER_TYPE_1, 14, 2, 9, 5, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M32
  ArtDriver BeforWinding(10, ("M34DRV"), ArtDriver::DRIVER_TYPE_1, 16, 3, 15, 5, true, 2, 2, 2, 0, 2, 2, 2); //Драйвер на M34

  ArtConveyor1TypeNextExtDev BeforWin(11, ("ConvM34"), ArtBasicConveyor::CONVEYOR_TYPE_1_EXTE, &BeforWinding, &ExitShuttle, &PalletOnConvM34, 20000, 0, 1, 2);
  ArtConveyorShuttleType Shuttle(12, ("ConvM32&33"), ArtBasicConveyor::CONVEYOR_TYPE_SHUTTLE, &ShuttleDrv, &OverShuttleDrv, &RangeFinderAnalog, &PalletOnConvM32, &EnterShuttle, &ExitShuttle,
                                 &BeforWin, &EdgeOnShuttle, &PalletOnConvM34, false, 1, 400000, 100000, 0, 0, 40, 1); // переделать
  //----setup for A41--Shuttle-------------------------------------------------------------------------------------
  /**/
  //----setup for A42 Lift-----------------------------------------------------------------------------------------
  ArtSensor M38End(1, ("B107"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true);            // Фотодатчик паллеты на конце М38 (B107) (тип BGS)
  ArtSensor OMSEnd(1, ("B101"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);           // Фотодатчик наличия паллеты на выходе OMS (B101) (тип R)
  ArtSensor M35End(2, ("B102"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);           // Датчик паллеты на конце М35 (B102) (тип Инд)
  ArtSensor AtlantaOut(3, ("B103"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 2000, false); // Фотодатчик наличия паллеты на выходе Атланты (B103) (тип R)
  ArtSensor M36End(4, ("B104"), 4, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);           // Фотодатчик паллеты на конце М36 (B104) (тип BGS G10)
  ArtSensor M37DOWN(5, ("B105"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);          // Датчик положения М37 ВНИЗУ (B105) (тип Инд)
  ArtSensor M37UP(6, ("B106"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);            // Датчик положения М37 НАВЕРХУ (B106) (тип Инд)

  ArtDriver M35Drv(8, ("M33DRV"), ArtDriver::DRIVER_TYPE_1, 14, 10, 9, 7, true, 2, 2, 2, 0, 2, 2, 2);    //Драйвер на M35
  ArtDriver M36Drv(9, ("M32DRV"), ArtDriver::DRIVER_TYPE_1, 15, 12, 11, 8, true, 2, 2, 2, 2, 2, 2, 2);   //Драйвер на M36
  ArtDriver M37Drv(10, ("M34DRV"), ArtDriver::DRIVER_TYPE_1, 16, 15, 13, 14, true, 2, 2, 2, 2, 2, 2, 2); //Драйвер на M37

  ArtConveyor1TypeNextExtDev M35(11, ("ConvM35"), ArtBasicConveyor::CONVEYOR_TYPE_1_EXTE, &M35Drv, &OMSEnd, &M35End, 20000, 0, 1, 2);
  ArtConveyorWithLift LiftConv(12, ("ConvM36"), ArtBasicConveyor::CONVEYOR_TYPE_1, &M36Drv, &M37Drv, &AtlantaOut, &M36End, &M37UP, &M37DOWN,
                               &M38End, 20000, 20000, 20000, 0, 1);

  //----setup for A42-Lift----------------------------------------------------------------------------------------
/*
  //----setup for A43----------------------------------------------------------------------------------------
ArtSensor M37DOWN(5, ("B105"), 5, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true);
 // ArtSensor M37UP(6, ("B106"), 6, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, true);         // Датчик положения М37 НАВЕРХУ (B106) (тип Инд)
  ArtSensor M38End(1, ("B107"), 1, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);       // Фотодатчик паллеты на конце М38 (B107) (тип BGS)
  ArtSensor M39End(2, ("B108"), 2, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);       // Фотодатчик паллеты на конце М39 (B108) (тип BGS)
  ArtSensor M40End(3, ("B109"), 3, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false); // Фотодатчик паллеты на конце М40 (B109) (тип BGS)

  ArtDriver M38Drv(8, ("M38DRV"), ArtDriver::DRIVER_TYPE_1, 14, 1, 11, 8, true, 2, 2, 2, 2, 2, 2, 2);  //Драйвер на M38
  ArtDriver M39Drv(9, ("M39DRV"), ArtDriver::DRIVER_TYPE_1, 15, 2, 13, 8, true, 2, 2, 2, 2, 2, 2, 2);  //Драйвер на M39
  ArtDriver M40Drv(10, ("M40DRV"), ArtDriver::DRIVER_TYPE_1, 16, 3, 15, 8, true, 2, 2, 2, 0, 2, 2, 2); //Драйвер на M40

  ArtConveyorPalletPickPoint ConvM40(13, ("ConvM40"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M40Drv, &M39End, &M40End, 10000, 0, 4, 3);
  ArtConveyor1Type ConvM39(12, ("ConvM39"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M39Drv, &M38End, &M39End, &ConvM40, 10000, 0, 2);
  ArtConveyor1Type ConvM38(11, ("ConvM38"), ArtConveyor1Type::CONVEYOR_TYPE_1, &M38Drv, &M37DOWN, &M38End, &ConvM39, 10000, 0, 1);

  //----setup for A43----------------------------------------------------------------------------------------
  /**/
  /*
  //----setup for Dispenser---------------------------------------------------------------------------------------
  ArtSensor PallONConvey(1, ("PallONConvey"), 10, ArtSensor::SENSOR_TYPE_BASIC, 0, 4000, false);
  ArtSensor PalletsInStack(2, ("PalletsInStack"), 9, ArtSensor::SENSOR_TYPE_BASIC, 0, 0, false);
  ArtSensor Clamp1Open(5, ("Clamp1Open"), 5, ArtSensor::SENSOR_TYPE_BASIC, 500, 0, false);
  ArtSensor Clamp1Close(7, ("Clamp1Close"), 7, ArtSensor::SENSOR_TYPE_BASIC, 500, 0, false);
  ArtSensor Clamp2Open(6, ("Clamp2Open"), 6, ArtSensor::SENSOR_TYPE_BASIC, 500, 0, false);
  ArtSensor Clamp2Close(8, ("Clamp2Close"), 8, ArtSensor::SENSOR_TYPE_BASIC, 500, 0, false);
  ArtSensor TOPCylinderOpen(7, ("TOPCylinderOpen"), 2, ArtSensor::SENSOR_TYPE_BASIC, 500, 0, false);
  ArtSensor TOPCylinderClose(8, ("TOPCylinderClose"), 1, ArtSensor::SENSOR_TYPE_BASIC, 500, 0, false);
  ArtSensor BOTCylinderOpen(9, ("BOTCylinderOpen"), 4, ArtSensor::SENSOR_TYPE_BASIC, 500, 0, false);
  ArtSensor BOTCylinderClose(10, ("BOTCylinderClose"), 3, ArtSensor::SENSOR_TYPE_BASIC, 500, 0, false);
  ArtCylinder Clamp1(11, ("Clamp1"), 10, 10, false, true, ArtCylinder::MONO_STABLE,
                     5, 5, &Clamp1Open, &Clamp1Close);

  ArtCylinder Clamp2(12, ("Clamp2"), 10, 10, false, true, ArtCylinder::MONO_STABLE,
                     5, 5, &Clamp2Open, &Clamp2Close);

  ArtCylinder TOPCylinder(13, ("TOPCylinder"), 10, 10, false, true,
                          ArtCylinder::BI_STABLE, 2, 1, &TOPCylinderOpen,
                          &TOPCylinderClose);

  ArtCylinder BOTCylinder(14, ("BOTCylinder"), 10, 10, false, true,
                          ArtCylinder::BI_STABLE, 4, 3, &BOTCylinderOpen,
                          &BOTCylinderClose);

  PalletMagazine Dispenser(15, ("Dispenser"), &Clamp1, &Clamp2, &TOPCylinder,
                           &BOTCylinder, 12, &PallONConvey, &PalletsInStack, 11, 16, 12,13);
  //----setup for Dispenser---------------------------------------------------------------------------------------
  /* */

  while (1)
  {
    if (EASYCAT.MainTask() == ESM_OP) // execute the EasyCAT task   if (EASYCAT.MainTask() == ESM_OP)
    {
      PORT->Group[0].OUTSET.reg = WDOG;
      //убрал драйверы из цикловика
      ArtIOClass::FlagConvState = false;
      int16_t AdcResult;
      Helper.doLogic();
      //AdcResult = ReadAdc(AnaInChannel);
     // WriteDacValues(1000, 0);
      ArtIOClass::doIOLogic();
      /*
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
      }*/

      /* EASYCAT.BufferIn.Cust.AnaIn_2 = Shuttle.PositionSens->SensorState();
       EASYCAT.BufferIn.Cust.AnaIn_3 = Shuttle.CurPos;*/

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

void loop()
{
}
