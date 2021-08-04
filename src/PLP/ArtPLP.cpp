#include "PLP/ArtPLP.h"





void ArtPLP::doLogic()
{
    switch (PLPState)
    {
            case PLP_ST_UNKNOWN:
            {              
               IF(APLPItem[nItemCounter].nSensorLayerId > 0)THEN
                  IF (ArtSensorState(APLPItem[nItemCounter].nSensorLayerId)) THEN
                     APITimer[nItemCounter].nTimerPallMoveOut = 0
                     APIFlags[nItemCounter].bPalletCompleted = TRUE
                     SetState(#PLP_ST_PALLET_FULL, nItemCounter) 
                  ENDIF
               ENDIF
               IF ArtSensorState(APLPItem[nItemCounter].nSensorPalleteId) THEN
                  APITimer[nItemCounter].nTimerPallMoveOut = 0
                  VOID_I(ARTConveyorSetExt(APLPItem[nItemCounter].nGlobalId, APLPItem[nItemCounter].nPlpConveyorId, CONVEYOR_EXTSTOP))
                  SetState(#PLP_ST_PALLET_UNFIXED, nItemCounter)
               ELSE
                  IF(APLPItem[nItemCounter].nPusherId > 0) THEN
                     ARTCylinderOpen(APLPItem[nItemCounter].nPusherId) 
                  ENDIF
                  IF (ArtConveyorGetDrvState(APLPItem[nItemCounter].nPlpConveyorId, GET_CONV_ACTUATOR_STOP, 0)>0) THEN     ;nPlpConveyorId IS STOPPED) THEN COMMAND: CHECK CONVEYOR AND DROP TIMER IF STOPPED
                     VOID_I(ARTConveyorSetExt(APLPItem[nItemCounter].nGlobalId, APLPItem[nItemCounter].nPlpConveyorId, CONVEYOR_EXTREV))
                     APITimer[nItemCounter].nTimerPallMoveOut = 0
                  ENDIF
                  IF CHK_ACTIVE_NTIME(NOT ArtSensorState(APLPItem[nItemCounter].nSensorPalleteId), APITimer[nItemCounter].nTimerPallMoveOut, APITimeots[nItemCounter].nTimeoutPallMoveout) THEN 
                     APITimer[nItemCounter].nTimerPallMoveOut = 0
                     VOID_I(ARTConveyorSetExt(APLPItem[nItemCounter].nGlobalId, APLPItem[nItemCounter].nPlpConveyorId, CONVEYOR_EXTSTOP))
                     SetState(#PLP_ST_NEEDPALLET, nItemCounter)
                  ENDIF
               ENDIF               
            }   
            case PLP_ST_NEEDPALLET:
            {
               if ()
            }   
            case PLP_ST_PALLET_PLACED: //align pallet with pusher 
            {
               IF(APLPItem[nItemCounter].nPusherId > 0) THEN
                  ARTCylinderClose(APLPItem[nItemCounter].nPusherId)
                  IF CHK_ACTIVE_NTIME(TRUE, APITimer[nItemCounter].nTimerPusherAction, APITimeots[nItemCounter].nTimeoutPusherAct) THEN
                     bPalletWraped = FALSE ;check pallet wraped from plp by pusher (pusher full closed) 
                     IF (ArtActGet(APLPItem[nItemCounter].nPusherId, GET_CYLINDER_STATE) == ARTCYL_ST_CLOSED) THEN
                        bPalletWraped = TRUE
                     ENDIF
                     ARTCylinderOpen(APLPItem[nItemCounter].nPusherId)    
                     APITimer[nItemCounter].nTimerPusherAction = 0
                     APITimer[nItemCounter].nTimerPallMoveOut = 0
                     APITimer[nItemCounter].nTimerPallInPlace = 0
                     IF (bPalletWraped) THEN
                        VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_PALLET_WRAPPED, #STATE, FALSE))
                        ;ArtOperatorMessage("ERROR: Pallet wrapped on PLP %1","ArtPLP", APLPItem[nItemCounter].nGlobalId) ;OP_MESSAGE
                        SetState(#ST_ERROR, nItemCounter)
                     ENDIF
                  ENDIF
               ENDIF
               ;wait for allign by robot
               IF(APIFlags[nItemCounter].bPalletAlligned)THEN
                  APIFlags[nItemCounter].bPalletAlligned = FALSE
                  SetState(#PLP_ST_PALLET_MOVE_IN, nItemCounter)
               ENDIF
            }   
            case PLP_ST_PALLET_MOVE_IN:
            {
               ;COMMAND: REVERSE CONVEYOR
               VOID_I(ARTConveyorSetExt(APLPItem[nItemCounter].nGlobalId, APLPItem[nItemCounter].nPlpConveyorId, CONVEYOR_EXTREV))
               IF CHK_ACTIVE_NTIME(ArtSensorState(APLPItem[nItemCounter].nSensorPalleteId), APITimer[nItemCounter].nTimerPallInPlace, APITimeots[nItemCounter].nTimeoutPallInPlace) THEN
                  APITimer[nItemCounter].nTimerPallInPlace = 0
                  APITimer[nItemCounter].nTimerPallMoveOut = 0
                  APITimer[nItemCounter].nTimerPusherAction = 0
                  ;COMMAND: STOP CONVEYOR
                  IF(ARTConveyorSetExt(APLPItem[nItemCounter].nGlobalId, APLPItem[nItemCounter].nPlpConveyorId, CONVEYOR_EXTSTOP)>-1)THEN
                     SetState(#PLP_ST_PALLET_UNFIXED, nItemCounter)
                  ENDIF
               ENDIF
               ;check for timeout of waiting
               IF CHK_ACTIVE_NTIME(NOT ArtSensorState(APLPItem[nItemCounter].nSensorPalleteId), APITimer[nItemCounter].nTimerPallMoveOut, APITimeots[nItemCounter].nTimeoutPallMoveout) THEN ;10secs CD
                  APITimer[nItemCounter].nTimerPallInPlace = 0
                  APITimer[nItemCounter].nTimerPallMoveOut = 0
                  VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_PALLET_STUCKED, #STATE, FALSE))
                  ;ArtOperatorMessage("ERROR: Pallet stucked or sensor broken on PLP %1","ArtPLP", APLPItem[nItemCounter].nGlobalId) ;OP_MESSAGE
                  ;COMMAND: STOP CONVEYOR
                  IF(ARTConveyorSetExt(APLPItem[nItemCounter].nGlobalId, APLPItem[nItemCounter].nPlpConveyorId, CONVEYOR_EXTSTOP)>-1)THEN
                     SetState(#ST_ERROR, nItemCounter)
                  ENDIF
               ENDIF
            }   
            case PLP_ST_PALLET_UNFIXED:
            {
               IF(APLPItem[nItemCounter].nPusherId > 0) THEN
                  ARTCylinderClose(APLPItem[nItemCounter].nPusherId)
                  IF CHK_ACTIVE_NTIME(TRUE, APITimer[nItemCounter].nTimerPusherAction, APITimeots[nItemCounter].nTimeoutPusherAct) THEN
                     APITimer[nItemCounter].nTimerPusherAction = 0
                     ;check for no_pos - WARNING NO NO_POS implemetation in ActCyls => error in ActCyl by timeout
                     IF (ArtActGet(APLPItem[nItemCounter].nPusherId, GET_CYLINDER_STATE)==ARTCYL_ST_MOVING) THEN
                        SetState(#PLP_ST_PALLET_FIXED, nItemCounter) 
                     ELSE
                        ;set error pallet wraped or still unfixed
                        VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_PALLET_UNFIXED, #STATE, FALSE))
                        ;ArtOperatorMessage("ERROR: Pallet wrapped or unfixed on PLP %1","ArtPLP", APLPItem[nItemCounter].nGlobalId) ;OP_MESSAGE
                        SetState(#ST_ERROR, nItemCounter)
                     ENDIF
                  ENDIF
               ELSE
                  SetState(#PLP_ST_PALLET_READY, nItemCounter) 
               ENDIF
            }   
            case PLP_ST_PALLET_FIXED:
            {
               IF(ArtSensorState(APLPItem[nItemCounter].nSensorLayerId)) THEN ;check: pallet must be empty
                  VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_PALLET_NOTEMPTY, #STATE, FALSE))
                  ;ArtOperatorMessage("ERROR: On PLP %1 Pallet not empty", "ArtPLP", APLPItem[nItemCounter].nGlobalId);OP_MESSAGE
                  SetState(#ST_ERROR, nItemCounter) 
               ELSE
                  SetState(#PLP_ST_PALLET_READY, nItemCounter) ;or maybe straight to #PLP_ST_PALLET_BUSY ?
               ENDIF
            }  
            case PLP_ST_PALLET_READY:
            {
               IF (APLPItem[nItemCounter].nSensorLayerId > 0) THEN
                  IF(ArtSensorState(APLPItem[nItemCounter].nSensorLayerId)) THEN
                     SetState(#PLP_ST_PALLET_BUSY, nItemCounter)                 
                  ENDIF
               ELSE
                  IF(APIFlags[nItemCounter].bPalletizeStarted)THEN
                     APIFlags[nItemCounter].bPalletizeStarted = FALSE
                     SetState(#PLP_ST_PALLET_BUSY, nItemCounter)
                  ENDIF
               ENDIF
               
               IF (APIFlags[nItemCounter].bPalletCompleted) THEN
                  ;APIFlags[nItemCounter].bPalletCompleted = FALSE
                  APITimer[nItemCounter].nTimerPusherAction = 0
                  SetState(#PLP_ST_PALLET_FULL, nItemCounter)
               ENDIF               
            }   
            case PLP_ST_PALLET_BUSY:
            {
               IF (APIFlags[nItemCounter].bPalletCompleted) THEN
                  ;APIFlags[nItemCounter].bPalletCompleted = FALSE
                  APITimer[nItemCounter].nTimerPusherAction = 0
                  SetState(#PLP_ST_PALLET_FULL, nItemCounter)
               ENDIF
            }

            case PLP_ST_PALLET_FULL://;check exit zone
            {
               IF (APIFlags[nItemCounter].bPalletCompleted) THEN
                  APIFlags[nItemCounter].bPalletCompleted = FALSE
                  IF (APLPItem[nItemCounter].nSensorLayerId > 0) THEN
                     IF NOT(ArtSensorState(APLPItem[nItemCounter].nSensorLayerId)) THEN ;check sens layer for brokedown
                        VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_SENSORLAYER_FAIL, #STATE, FALSE))
                        SetState(#ST_ERROR, nItemCounter)
                     ENDIF
                  ENDIF
               ENDIF
               IF(APLPItem[nItemCounter].nPusherId > 0) THEN
                  ARTCylinderOpen(APLPItem[nItemCounter].nPusherId)
                  IF (ArtWorkareaIsAvailable(APLPItem[nItemCounter].nExitWorkareaId)) THEN
                     IF(ArtActGet(APLPItem[nItemCounter].nPusherId, GET_CYLINDER_STATE)==ARTCYL_ST_OPEN) THEN
                        APITimer[nItemCounter].nTimerPallMoveOut = 0
                        APITimer[nItemCounter].nTimerPusherAction = 0
                        SetState(#PLP_ST_PALLET_MOVE_OUT, nItemCounter)
                     ELSE
                        IF(CHK_ACTIVE_NTIME(TRUE, APITimer[nItemCounter].nTimerPusherAction, APITimeots[nItemCounter].nTimeoutPusherAct)) THEN
                           APITimer[nItemCounter].nTimerPusherAction = 0
                           VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_PUSHER_FAIL, #STATE, FALSE))
                           SetState(#ST_ERROR, nItemCounter)
                        ENDIF
                     ENDIF
                  ENDIF
               ELSE
                  IF (ArtWorkareaIsAvailable(APLPItem[nItemCounter].nExitWorkareaId)) THEN
                     APITimer[nItemCounter].nTimerPallMoveOut = 0
                     SetState(#PLP_ST_PALLET_MOVE_OUT, nItemCounter)
                  ENDIF
               ENDIF
            }   
            case PLP_ST_PALLET_MOVE_OUT:
            {   
               OnPalletMoveout(nItemCounter) ;ends when CONV is stopped
            }
            case PLP_ST_CLEAR_CHECK:// ;check sensors: pallet and first layer - both must be false
            {  
               IF(APLPItem[nItemCounter].nSensorLayerId>0)THEN
                  IF(ArtSensorState(APLPItem[nItemCounter].nSensorLayerId)) THEN
                     VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_SMTH_DROPPED, #STATE, FALSE))
                     SetState(#ST_ERROR, nItemCounter) 
                  ENDIF
               ENDIF
               IF(ArtSensorState(APLPItem[nItemCounter].nSensorPalleteId))THEN
                  VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_SMTH_DROPPED, #STATE, FALSE))
                  SetState(#ST_ERROR, nItemCounter) 
               ELSE
                  SetState(#PLP_ST_NEEDPALLET, nItemCounter) 
               ENDIF
            }   
            case ST_ERROR:
            {
               IF(APLPItem[nItemCounter].nButtonResetId == 0)THEN
                  IF ARTHMIIsResetPushed() THEN
                     VOID_I(ArtConveyorSet(APLPItem[nItemCounter].nPlpConveyorId, SET_CONV_PROD_ENTERING, 0))
                     ArtMsgDeviceClear(APLPItem[nItemCounter].nGlobalId)
                     VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_PLP_IS_RESETED, #QUIT, FALSE))
                     DropBlocking(nItemCounter) ;drop current blocking
                     DropFlags(nItemCounter)
                     VOID_I(ARTConveyorSetExt(APLPItem[nItemCounter].nGlobalId, APLPItem[nItemCounter].nPlpConveyorId, CONVEYOR_EXTENABLE))
                     IF(APLPItem[nItemCounter].nPusherId>0)THEN
                        ARTCylinderOpen(APLPItem[nItemCounter].nPusherId)
                     ENDIF
                     SetState(#PLP_ST_PALLET_MOVE_OUT, nItemCounter)
                  ENDIF
               ELSE
                  VOID_I(ArtButtonSet(APLPItem[nItemCounter].nButtonResetId, #SET_LIGHT_LONGBLINKING, 0)) ;NEED TO BE TESTED
                  IF ArtButtonIsPressed(APLPItem[nItemCounter].nButtonResetId) THEN
                     VOID_I(ArtButtonSet(APLPItem[nItemCounter].nButtonResetId, #SET_LIGHT_OFF, 0));NEED TO BE TESTED
                     VOID_I(ArtConveyorSet(APLPItem[nItemCounter].nPlpConveyorId, SET_CONV_PROD_ENTERING, 0))
                     ArtMsgDeviceClear(APLPItem[nItemCounter].nGlobalId)
                     VOID_I(ArtMsgCreate(APLPItem[nItemCounter].nGlobalId, PLP_MSG_PLP_IS_RESETED, #QUIT, FALSE))
                     DropBlocking(nItemCounter) ;drop current blocking
                     DropFlags(nItemCounter)
                     VOID_I(ARTConveyorSetExt(APLPItem[nItemCounter].nGlobalId, APLPItem[nItemCounter].nPlpConveyorId, CONVEYOR_EXTENABLE))
                     IF(APLPItem[nItemCounter].nPusherId>0)THEN
                        ARTCylinderOpen(APLPItem[nItemCounter].nPusherId)
                     ENDIF
                     SetState(#PLP_ST_PALLET_MOVE_OUT, nItemCounter)
                  ENDIF
               ENDIF
            }   
            default:
            break;
        }
}