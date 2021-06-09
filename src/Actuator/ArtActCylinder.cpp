#include "Actuator\ArtActCylinder.h"

ArtCylinder::ArtCylinder(int id, const char name[])
{
   IHasCycleLogicHelper::addDevice(this);
	m_id = id;
}

ArtCylinder::ArtCylinder(int id, const char name[], int CloseTime = 0, int OpenTime = 0,bool TimeoutControl = false, bool CylinderSet = false)
{
   cylCloseTimer = CloseTime;
   cylOpenTimer = OpenTime;
   bCylTimeoutControl = TimeoutControl;
   isCylinderSet = CylinderSet;
   cylState = ACGetInitialState();
}

void ArtCylinder:: doLogic()
{
   bool isCylinderActed;
   bool isCylinderOpened;
   bool isCylinderClosed;
   
   for (int i = 1;i<5;i++)
    {
      isCylinderActed  = ArtIOClass::getOutputState(/*нужный выход*/ 1);
      isCylinderOpened = ArtIOClass::getInputState(/*нужный вход*/1);
      isCylinderClosed = ArtIOClass::getInputState(/*нужный вход*/1);
      
      if (isCylinderOpened && isCylinderClosed) 
      {
         /*вывод ошибки*/
      }
      
      switch (cylState)
      {
         case ARTCYL_ST_OPEN:         
        {
                if (isCylinderClosed)
            {
              /* ArtDebugMsg("RUNTIME ERROR: Suddenly has closed CYL %1", "ArtActCylinder", handle)
               _NOP( _ON_ERR( _ERR( ERR_CYL_WRONG_POS ) ) )*/
            }
            
            if (isCylinderActed == cylCloseValue)
            {
               cylState= ARTCYL_ST_MOVING;
               cylCloseTimer = ArtBasicConveyor::ARTTimerGetTime(); 
            } 
            break;
        }  
         case ARTCYL_ST_CLOSED:
         {         
            if (isCylinderOpened)
            {  
               /*ArtDebugMsg("RUNTIME ERROR: Suddenly has opened CYL %1", "ArtActCylinder", handle)
               _NOP( _ON_ERR( _ERR( ERR_CYL_WRONG_POS ) ) )*/
            }
            
            if (isCylinderActed == ! cylCloseValue)
            {  
               cylState = ARTCYL_ST_MOVING;
               cylOpenTimer = ArtBasicConveyor::ARTTimerGetTime(); 
            }
            break;
         }

         case ARTCYL_ST_MOVING:
         {
            if (isCylinderActed == !cylCloseValue)
            {
                  if(isCylinderOpened)  
                  {
                     cylState = ARTCYL_ST_OPEN;
                     cylOpenTimer = 0;
                  }
               else
                  {
                     if (bCylTimeoutControl) 
                     {
                        if (CHK_ACTIVE_NTIME( isCylinderActed == ! cylCloseValue, cylOpenTimer, cylOpenDelay )) 
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
                  if(isCylinderClosed)
                  {    
                     cylState = ARTCYL_ST_CLOSED;
                     cylCloseTimer = 0;
                  }
                  else
                  {
                     if (bCylTimeoutControl)
                     {
                        if (! CHK_ACTIVE_NTIME( isCylinderActed == cylCloseValue, cylCloseTimer, cylCloseDelay ) )
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
}
bool ArtCylinder::CHK_ACTIVE_NTIME(bool sens_in, int timer_in, int delta_time)//sensor,timer for check, active time before return true
{
   int curTime, deltaTime;
 
   
   if (! sens_in)
   {
      timer_in = 0;
   }
   else
   {
      if (timer_in == 0 )
      {
         timer_in = millis();
      }
      
      curTime = millis();
      
      if (timer_in < curTime) 
      {
         deltaTime = curTime - timer_in;
         return (deltaTime > delta_time);
      }
      else //exceptions check
      {  
         if( timer_in == curTime)
         {
            return(false);
         }
         else
         {
            if (timer_in > curTime)
            {
               timer_in = timer_in - 2147483647;
               deltaTime = curTime - timer_in;
               return (deltaTime > delta_time);
            }        
         }
      }
   }      
   return(false);
}

int ArtCylinder:: ACGetInitialState( )
{
   bool isCylinderActed;
   bool isCylinderOpened;
   bool isCylinderClosed;
   
   isCylinderActed  = ArtIOClass::getOutputState(/*нужный выход*/ 1);
   isCylinderOpened = ArtIOClass::getInputState(/*нужный вход*/1);
   isCylinderClosed = ArtIOClass::getInputState(/*нужный вход*/1);
   
   if( isCylinderOpened && isCylinderClosed)
   {
      return (ARTCYL_ST_ERROR) ;
   }
   
   if (isCylinderActed != cylCloseValue)
   {
      if (isCylinderOpened)
      {         
         return (ARTCYL_ST_OPEN);
      } 
      
      
      cylOpenTimer = ArtBasicConveyor::ARTTimerGetTime();
      return (ARTCYL_ST_MOVING);
   }
   else
   {
      if (isCylinderClosed)
      {
         return (ARTCYL_ST_CLOSED); 
      }
      cylCloseTimer = ArtBasicConveyor::ARTTimerGetTime();
      return (ARTCYL_ST_MOVING); 
   } 

}
