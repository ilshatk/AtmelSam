#include "PalletMagazine\ArtPalletMagazine.h"

void PalletMagazine::doLogic()
{
    CLAMP_POS_STATE_SPS();
    DISP_POS_STATE_SPS();
}

void PalletMagazine::CLAMP_POS_STATE_SPS()
{
    if ((!isAutoMode) && (!isBotlleConvServiceMode))
    {
        if (Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)
        {
            if (ArtCylinder::CHK_ACTIVE_NTIME(Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED, isClamp1Open_timer, DEF_TIME_POS_SENS))
            {
                if (Clamp2->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED)
                {
                    ClampState = CL_OPEN;
                }
                else
                {
                    ClampState = ERROR;
                    //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "CLAMPS_OPEN_ERROR")
                    //artOpcMessage[201]=1
                }
            }
            else
            {
                if (ArtCylinder::CHK_ACTIVE_NTIME(Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED, isClamp1Open_timer, DEF_TIME_POS_SENS))
                {
                    //;reset timer
                }
            }
            isClamp1Close_timer = 0;
        }

        if (Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_OPEN)
        {
            if (ArtCylinder::CHK_ACTIVE_NTIME(Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_OPEN, isClamp1Close_timer, DEF_TIME_POS_SENS))
            {
                if (Clamp2->getCylState() == ArtCylinder::ARTCYL_ST_OPEN)
                {
                    ClampState = CL_CLOSE;
                }
                else
                {
                    ClampState = ERROR;
                    //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "CLAMPS_OPEN_ERROR")
                    //artOpcMessage[201]=1
                }
            }
            else
            {
                if (ArtCylinder::CHK_ACTIVE_NTIME(Clamp1->getCylState() == ArtCylinder::ARTCYL_ST_OPEN, isClamp1Close_timer, DEF_TIME_POS_SENS))
                {
                    //;reset timer
                }
            }
            isClamp1Open_timer = 0;
        }
    }
    else
    {
        doTOPCylinderUP = false;
        doTOPCylinderDOWN = false;
        doBOTCylinderUP = false;
        doBOTCylinderDOWN = false;
        doOpenClamps = false;

        if (DispCur == TOP)
        {
            if (isPalletsInStack)
            {
                DispPallState = HAS_PALL;
            }

            if (isPalletOnConvey && !isPalletsInStack)
            {
                DispPallState = LOW_PALL;
            }

            if (!isPalletsInStack && !isPalletOnConvey)
            {
                DispPallState = NO_PALL;
            }
        }
    }
}
void PalletMagazine::DISP_POS_STATE_SPS()
{
    isBOTCylinderUP = BOTCylinder->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED;
    isTOPCylinderUP = BOTCylinder->getCylState() == ArtCylinder::ARTCYL_ST_CLOSED;
    isBOTCylinderDOWN = BOTCylinder->getCylState() == ArtCylinder::ARTCYL_ST_OPEN;
    isTOPCylinderDOWN = BOTCylinder->getCylState() == ArtCylinder::ARTCYL_ST_OPEN;

    if (isTOPCylinderUP)
    {
        if (ArtCylinder::CHK_ACTIVE_NTIME(isTOPCylinderUP, isTOPCylinderUP_timer, DEF_TIME_POS_SENS))
        {
            if (isBOTCylinderUP)
            {
                DispCur = TOP;
            }

            if (isBOTCylinderDOWN)
            {
                DispCur = ERROR;
                //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION") ;
                //artOpcMessage[200]=1      ;
            }

            if (!isBOTCylinderUP & !isBOTCylinderDOWN)
            {
                DispCur = ERROR;
                //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION")
                //artOpcMessage[200]=1
            }
        }
    }
    else
    {
        if (ArtCylinder::CHK_ACTIVE_NTIME(isBOTCylinderUP, isTOPCylinderUP_timer, DEF_TIME_POS_SENS))
        {
            // ;reset timer;
        }
    }

    if (isTOPCylinderDOWN)
    {
        if (ArtCylinder::CHK_ACTIVE_NTIME(isTOPCylinderDOWN, isTOPCylinderDOWN_timer, DEF_TIME_POS_SENS))
        {
            if (isBOTCylinderUP)
            {
                DispCur = MID;
            }

            if (isBOTCylinderDOWN)
            {
                DispCur = BOT;
            }

            if (!isBOTCylinderUP && !isBOTCylinderDOWN)
            {
                DispCur = ERROR;
                //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION")
                //artOpcMessage[200]=1}
            }
        }
    }
    else
    {
        if (ArtCylinder::CHK_ACTIVE_NTIME(isTOPCylinderDOWN, isTOPCylinderDOWN_timer, DEF_TIME_POS_SENS))
        {
            //;reset timer
        }
    }

    isTOPcylinderError = (!isTOPCylinderUP && !isTOPCylinderDOWN);

    if (ArtCylinder::CHK_ACTIVE_NTIME(isTOPcylinderError, isTOPcylinderError_timer, DEF_TIME_POS_SENS))
    {
        //ARTHMISetMessage(m_disp_pos_error, TRUE, "ARTDISP", "DISPENSER_CYLINDER_NOT_IN_END_POSITION")
        //artOpcMessage[200]=1
    }
}

void PalletMagazine:: DISP_MAIN_CYCLE_SPS()
{
    bool isBOT,isTOP,isMID;
    if (DispCur == BOT)
    {
        isBOT = true;
    }
    else
    {
        isBOT = false;
        if (DispCur == MID)
        {
            isMID = true;
        }
      ELSE
         isMID = FALSE
         IF DISP_CUR_POS==#TOP THEN
            isTOP = TRUE
         ELSE
            isTOP = FALSE
         ENDIF
      ENDIF
   }
   ;   
   ;
   IF ((NOT isPallONConvey) AND (doRunConv4)) THEN
      isPallONConvey_flag = TRUE
   ELSE
      isPallONConvey_flag = FALSE
   ENDIF
   ;
   
   ;
   IF isPallONConvey THEN
      PallONConvey = TRUE    
   ENDIF      
   ;
   IF CHK_ACTIVE_NTIME(isPallONConvey_flag, isPallONConvey_timer,isPallONConvey_TIME) THEN ;WAIT SEC 2 ;CHK_ACTIVE_NTIME(SENSOR, Timer, CHECKING_TIME)
      PallONConvey = FALSE 
   ENDIF
   ;
   
   IF (NOT isAutoMode) AND (NOT isBotlleConvServiceMode) THEN
      
      IF  DISP_STATE==#READY THEN
         
         SWITCH DISP_CUR_POS  
            ;
            CASE #BOT 
               
               IF CHK_ACTIVE_NTIME(isBOT, isBOT_timer,DEF_TIME_POS_SENS) THEN ;WAIT SEC 2 ;CHK_ACTIVE_NTIME(SENSOR, Timer, CHECKING_TIME)               
                  
                  ;IF isPalletOnConvey THEN
                  ;
                  IF isPalletsInStack THEN 
                     DISP_PAL_STATE = #HAS_PALL 
                  ENDIF
                  IF isPalletOnConvey AND NOT isPalletsInStack THEN
                     DISP_PAL_STATE = #LOW_PALL 
                  ENDIF      
                  ;
                  IF  NOT  isPalletsInStack THEN  ;NOT isPalletOnConvey AND             
                     DISP_PAL_STATE = #NO_PALL 
                  ENDIF
                  
                  IF CLAMPS_STATE == #CL_OPEN THEN
                     DISPGOMID()
                  ELSE
                     doOPENCLAMPS()
                  ENDIF               
               ENDIF
               
               ;              
            CASE #MID
               ;
               IF CHK_ACTIVE_NTIME(isMID, isMID_timer,DEF_TIME_POS_SENS) THEN ;WAIT SEC 2 ;CHK_ACTIVE_NTIME(SENSOR, Timer, CHECKING_TIME)
                  IF (CLAMPS_STATE == #CL_OPEN) THEN
                     
                     IF  ((DISP_PAL_STATE == #HAS_PALL) OR (DISP_PAL_STATE == #LOW_PALL)) THEN
                        doCLOSECLAMPS()
                     ENDIF
                     ;
                     ;IF (DISP_PAL_STATE == #HAS_PALL) THEN
                     ;
                     ;IF (CLAMPS_STATE == #CL_OPEN) THEN
                     ;doCLOSECLAMPS()
                     ;ELSE
                     ;DISPGOTOP()
                     ;ENDIF                     
                     ;ENDIF  
                     ;
                     IF (DISP_PAL_STATE == #NO_PALL) THEN
                        
                        IF (CLAMPS_STATE == #CL_OPEN) THEN
                           doCLOSECLAMPS()
                        ELSE
                           DISPGOTOP()
                        ENDIF                     
                     ENDIF  
                     
                  ELSE
                     IF ((DISP_PAL_STATE == #HAS_PALL) OR (DISP_PAL_STATE == #LOW_PALL)) THEN
                        
                        If PallONConvey OR (PALL_conveyors[4].state == #CONV_RUN)THEN 
                           DISPGOTOP()
                        ELSE
                           IF NOT isPallONConvey THEN
                              DISPGOBOT()
                           ENDIF
                           
                        ENDIF 
                     ENDIF                  
                  ENDIF  
               ENDIF
               
               ;
            CASE #TOP
               If PallONConvey OR (PALL_conveyors[4].state == #CONV_RUN) THEN 
               ELSE
                  IF NOT isPallONConvey THEN
                     IF ((DISP_PAL_STATE == #HAS_PALL) OR (DISP_PAL_STATE == #LOW_PALL)) THEN
                        
                        IF NOT isPallONConvey THEN
                           DISPGOBOT()
                        ENDIF
                        
                     ENDIF   
                  ENDIF
               ENDIF 
               ;
               
            DEFAULT
               ; continue
         ENDSWITCH   
         
      ENDIF
   ENDIF
}