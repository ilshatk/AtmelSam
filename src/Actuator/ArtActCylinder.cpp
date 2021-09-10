#include "Actuator\ArtActCylinder.h"

int ArtCylinder::getName()
{
   return (0);
}

int ArtCylinder::getID()
{
   return (m_id);
}

ArtCylinder::ArtCylinder(int id, const char name[])
{
   IHasCycleLogicHelper::addDevice(this);
   m_id = id;
}

ArtCylinder::ArtCylinder(int id, const char name[], int CloseTime, int OpenTime, bool TimeoutControl, bool CylinderSet, distType type, int cylOpenOut, int cylCloseOut, ArtSensor *cylOpenIn, ArtSensor *cylCloseIn) : ArtCylinder(id, name)
{
   cylCloseTimer = CloseTime;
   cylOpenTimer = OpenTime;
   bCylTimeoutControl = TimeoutControl;
   isCylinderSet = CylinderSet;
   ArtCylinder::cylOpenOut = cylOpenOut;
   ArtCylinder::cylCloseOut = cylCloseOut;
   ArtCylinder::distrType = type;
   cylCloseValue = true;
   ArtCylinder::cylOpenIn = cylOpenIn;
   ArtCylinder::cylCloseIn = cylCloseIn;
   cylState = ACGetInitialState();
}

ArtCylinder::ArtCylinder(int id, const char name[], int OpenTime, bool TimeoutControl, bool CylinderSet, distType type, int cylOpenOut, ArtSensor *cylOpenIn, ArtSensor *cylCloseIn, int cylCloseDelay, int cylOpenDelay) : ArtCylinder(id, name)
{
   cylOpenTimer = OpenTime;
   bCylTimeoutControl = TimeoutControl;
   isCylinderSet = CylinderSet;
   ArtCylinder::cylOpenOut = cylOpenOut;
   ArtCylinder::distrType = type;
   cylCloseValue = true;
   ArtCylinder::cylOpenIn = cylOpenIn;
   ArtCylinder::cylCloseIn = cylCloseIn;
   cylState = ACGetInitialState();
   ArtCylinder::cylCloseDelay = cylCloseDelay;
   ArtCylinder::cylOpenDelay = cylOpenDelay;
}

void ArtCylinder::doLogic()
{
   isCylinderActed = ArtIOClass::getOutputState(cylOpenOut);
   isCylinderOpened = cylOpenIn->SensorState();
   isCylinderClosed = cylCloseIn->SensorState();

   if (!isCylinderOpened && !isCylinderClosed)
   {
      /*вывод ошибки*/
   }

   switch (cylState)
   {
   case ARTCYL_ST_OPEN:
   {
      if (isCylinderClosed)
      {
         /* ArtDebugMsg("RUNTIME ERROR: Suddenly has closed CYL %1", 
            "ArtActCylinder", handle)
                  _NOP( _ON_ERR( _ERR( ERR_CYL_WRONG_POS ) ) )*/
      }

      if (isCylinderActed == cylCloseValue)
      {
         cylState = ARTCYL_ST_MOVING;
         cylCloseTimer = ArtIOClass::ARTTimerGetTime();
      }
      break;
   }

   case ARTCYL_ST_CLOSED:
   {
      if (isCylinderOpened)
      {
         /*ArtDebugMsg("RUNTIME ERROR: Suddenly has opened CYL %1",
            "ArtActCylinder", handle)
                  _NOP( _ON_ERR( _ERR( ERR_CYL_WRONG_POS ) ) )*/
      }

      if (isCylinderActed == !cylCloseValue)
      {
         cylState = ARTCYL_ST_MOVING;
         cylOpenTimer = ArtIOClass::ARTTimerGetTime();
      }

      break;
   }

   case ARTCYL_ST_MOVING:
   {
      if (isCylinderActed == !cylCloseValue)
      {
         if (isCylinderOpened)
         {
            cylState = ARTCYL_ST_OPEN;
            cylOpenTimer = 0;
         }
         else
         {
            if (bCylTimeoutControl)
            {
               if (ArtIOClass::CHK_ACTIVE_NTIME(isCylinderActed == !cylCloseValue, &cylOpenTimer, cylOpenDelay))
               {
                  // ArtDebugMsg("RUNTIME ERROR: Opening timeout from %1 millisecs", "ArtActCylinder", cylOpenDelay[handle])
                  cylOpenTimer = 0;
                  cylState = ARTCYL_ST_ERROR;
                  //_NOP( _ON_ERR( _ERR( ERR_CYL_OPEN_TIMEOUT ) ) )
               }
            }
         }
      }
      else //;DEF_CLOSE_CYLINDER
      {
         if (isCylinderClosed)
         {
            cylState = ARTCYL_ST_CLOSED;
            cylCloseTimer = 0;
         }
         else
         {
            if (bCylTimeoutControl)
            {
               if (!ArtIOClass::CHK_ACTIVE_NTIME(isCylinderActed == cylCloseValue, &cylCloseTimer, cylCloseDelay))
               {
                  cylCloseTimer = 0;
                  //ArtDebugMsg("RUNTIME ERROR: Closing timeout from %1 millisecs", "ArtActCylinder", cylCloseDelay[handle])
                  cylState = ARTCYL_ST_ERROR;
                  //NOP( _ON_ERR( _ERR( ERR_CYL_OPEN_TIMEOUT )))
               }
            }
         }
      }
      break;
   }

   case ARTCYL_ST_ERROR:
   {
      break;
   }
   default:
      break;
   }
}

int ArtCylinder::ACGetInitialState()
{
   bool isCylinderOpened;
   bool isCylinderClosed;

   isCylinderActed = false;
   isCylinderOpened = cylOpenIn->SensorState();
   isCylinderClosed = cylCloseIn->SensorState();

   if (isCylinderOpened && isCylinderClosed)
   {
      return (ARTCYL_ST_ERROR);
   }

   if (isCylinderActed != cylCloseValue)
   {
      if (isCylinderOpened)
      {
         return (ARTCYL_ST_OPEN);
      }

      cylOpenTimer = ArtIOClass::ARTTimerGetTime();
      return (ARTCYL_ST_MOVING);
   }
   else
   {
      if (isCylinderClosed)
      {
         return (ARTCYL_ST_CLOSED);
      }
      cylCloseTimer = ArtIOClass::ARTTimerGetTime();
      return (ARTCYL_ST_MOVING);
   }
}

void ArtCylinder::ARTCylinderOpen()
{
   if (distrType == BI_STABLE)
   {
      ArtIOClass::setOutputState(cylCloseOut, false);
      ArtIOClass::setOutputState(cylOpenOut, true);
   }
   else
   {
      ArtIOClass::setOutputState(cylOpenOut, true);
   }
   isCylinderActed = !(cylCloseIn->SensorState());
}

void ArtCylinder::ARTCylinderClose()
{
   if (distrType == BI_STABLE)
   {
      ArtIOClass::setOutputState(cylOpenOut, false);
      ArtIOClass::setOutputState(cylCloseOut, true);
   }
   else
   {
      ArtIOClass::setOutputState(cylOpenOut, false);
      //ArtIOClass::setOutputState(cylCloseOut, true);
   }
   isCylinderActed = cylCloseIn->SensorState();
}

void ArtCylinder::ARTCylinderOFF()
{
   ArtIOClass::setOutputState(cylOpenOut, false);
   ArtIOClass::setOutputState(cylCloseOut, false);
}

int ArtCylinder::getCylState()
{
   if (cylCloseIn->SensorState())
   {
      return (1);
   }
   else
   {
      if (cylOpenIn->SensorState())
      {
         return (0);
      }
   }
   //return (cylState);
}

void ArtCylinder::update()
{
}