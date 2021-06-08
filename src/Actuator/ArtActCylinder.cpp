#include "Actuator\ArtActCylinder.h"
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
            IF isCylinderOpened THEN 
               ArtDebugMsg("RUNTIME ERROR: Suddenly has opened CYL %1", "ArtActCylinder", handle)
               _NOP( _ON_ERR( _ERR( ERR_CYL_WRONG_POS ) ) )
            ENDIF
            
            IF isCylinderActed == NOT cylCloseValue[handle] THEN
               cylinderState[handle] = ARTCYL_ST_MOVING
               cylOpenTimer[handle] = ARTTimerGetTime() 
            ENDIF
            ;            
         CASE ARTCYL_ST_MOVING
            IF isCylinderActed == NOT cylCloseValue[handle] THEN
               IF isCylinderOpened THEN 
                  cylinderState[handle] = ARTCYL_ST_OPEN
                  cylOpenTimer[handle] = 0
               ELSE
                  IF (bCylTimeoutControl[handle]) THEN
                     IF CHK_ACTIVE_NTIME( isCylinderActed == NOT cylCloseValue[handle], cylOpenTimer[handle], cylOpenDelay[handle] ) THEN
                        ArtDebugMsg("RUNTIME ERROR: Opening timeout from %1 millisecs", "ArtActCylinder", cylOpenDelay[handle])
                        cylOpenTimer[handle] = 0
                        cylinderState[handle] = ARTCYL_ST_ERROR
                        _NOP( _ON_ERR( _ERR( ERR_CYL_OPEN_TIMEOUT ) ) )
                     ENDIF
                  ENDIF
               ENDIF              
            ELSE ;DEF_CLOSE_CYLINDER
               IF isCylinderClosed THEN 
                  cylinderState[handle] = ARTCYL_ST_CLOSED
                  cylCloseTimer[handle] = 0
               ELSE
                  IF (bCylTimeoutControl[handle]) THEN
                     IF NOT CHK_ACTIVE_NTIME( isCylinderActed == cylCloseValue[handle], cylCloseTimer[handle], cylCloseDelay[handle] ) THEN
                        cylCloseTimer[handle] = 0
                        ArtDebugMsg("RUNTIME ERROR: Closing timeout from %1 millisecs", "ArtActCylinder", cylCloseDelay[handle])
                        cylinderState[handle] = ARTCYL_ST_ERROR
                        _NOP( _ON_ERR( _ERR( ERR_CYL_OPEN_TIMEOUT )))                     
                     ENDIF
                  ENDIF
               ENDIF              
            ENDIF
            ;
         CASE ARTCYL_ST_ERROR
            ;Deside what to do in error case
            }
   }
}