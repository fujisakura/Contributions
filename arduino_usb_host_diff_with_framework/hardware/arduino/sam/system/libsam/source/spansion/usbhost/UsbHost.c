/*******************************************************************************
* Copyright (C) 2013 Spansion LLC. All Rights Reserved. 
*
* This software is owned and published by: 
* Spansion LLC, 915 DeGuigne Dr. Sunnyvale, CA  94088-3453 ("Spansion").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND 
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with Spansion 
* components. This software is licensed by Spansion to be adapted only 
* for use in systems utilizing Spansion components. Spansion shall not be 
* responsible for misuse or illegal use of this software for devices not 
* supported herein.  Spansion is providing this software "AS IS" and will 
* not be responsible for issues arising from incorrect user implementation 
* of the software.  
*
* SPANSION MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS), 
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING, 
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED 
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED 
* WARRANTY OF NONINFRINGEMENT.  
* SPANSION SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT, 
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT 
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, 
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR 
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA, 
* SAVINGS OR PROFITS, 
* EVEN IF SPANSION HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED 
* FROM, THE SOFTWARE.  
*
* This software may be replicated in part or whole for the licensed use, 
* with the restriction that this Disclaimer and Copyright notice must be 
* included with each copy of this software, whether used in part or whole, 
* at all times.  
*/

/************************************************************************/
/** \file UsbHost.c
 **
 ** Spansion LLC Usb Host 
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version  (works with 16FX, FR80, FM3)
 **   - 2011-07-20    1.1  MSc  Corrections for usage with IAR compiler
 **   - 2011-08-11    1.2  MSc  Support for transfer abort
 **   - 2011-08-24    1.3  MSc  Several Timing fixes
 **   - 2011-08-29    1.4  MSc  Low-Speed / Full-Speed switching bug fixes
 *****************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/


#include "UsbHost.h"

#if (USE_USB_HOST == 1)
#include "UsbRegisters.h"

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/
void EnumerationStateMachine();

/*****************************************************************************/
/* function */
/*****************************************************************************/


/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

HostEndpoint_t* Ep0IN;
HostEndpoint_t* Ep0OUT;
HostEndpoint_t* ActiveEndpoint;

uint8_t u8DeviceStatus = USBHOST_DEVICE_IDLE;
uint8_t u8DeviceAddress = 0;
uint8_t pu8GlobalBuffer[255];
uint8_t u8EnumerationCounter = 0;
volatile uint32_t u32Timeout = 10;
volatile uint32_t u32ConTimeout = 0;
volatile uint32_t u32BusResetTimeout = 0;
volatile uint32_t u32ConnectionDelay = 1000;
UsbRequest_t stcLastSetup;
UsbRequest_t stcLastSetupBeforeStall;
const UsbRequest_t stcDeviceDescriptorRequest = {0x80,0x06,0x0100,0x00,18};
const UsbRequest_t stcShortDeviceDescriptorRequest = {0x80,0x06,0x0100,0x00,8};
const UsbRequest_t stcClearStallEp0 = {0x02,0x01,0x0000,0x00,0x00};
uint8_t u8CurrentEndpointPosition = 2;
uint8_t u8EndpointListEnd = 0;
volatile boolean_t bSchedulerLock = FALSE;
volatile boolean_t bBusResetSend = FALSE;
HostEndpoint_t HostEndpoints[MAX_HOSTENDPOINTS];
uint8_t u8Setup[8];
uint8_t u8SyncToken;
uint8_t u8SetupDelay;
uint8_t u8LastToken;
uint8_t u8SchedulerState;
uint8_t* pu8SetupData;
volatile boolean_t bFullSpeed = FALSE;
volatile boolean_t bSofToken = FALSE;
void (* SetupCompletionHandler)(uint8_t* pu8Buffer, uint32_t u32DataSize);
void (* SOFHandler)(uint16_t u16FrameNumber) = 0;
void (* StallCompletionHandler)(void);

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/


/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/
void UsbClassSupervisor_Init();
void UsbClassSupervisor_Deinit();
/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

void UsbHost_Init()
{
    volatile uint32_t u32WaitTime = 500000;
    bSofToken = FALSE;
    u32ConTimeout = 0;
    u32BusResetTimeout = 0;
    u8CurrentEndpointPosition = 2;
    u8EndpointListEnd = 0;
    bSchedulerLock = FALSE;
    bBusResetSend = FALSE;
    SOFHandler = 0;
    SetupCompletionHandler = 0;
    u8DeviceStatus = USBHOST_DEVICE_IDLE;
    u8DeviceAddress = 0;
    u8EnumerationCounter = 0;
    
    CLEAR_MASK(HCNT, _HCNT_HOST); // Disable Host (HWM rev16 - Chapter 30.4.1)

    HOSTVBUS_INIT;
    //HOSTOTGPULLDOWN_INIT;

    //HOSTVBUS_SET;
    HOSTVBUS_CLEAR;
    //HOSTOTGPULLDOWN_SET;



    while(u32WaitTime--) __wait_nop();
    DBGOUT(1,dbg("UsbHost: Starting init... "));
    
    SET_MASK(UDCC, _UDCC_HCONX);   // disconnect HCON signal
    SET_MASK(UDCC, _UDCC_RST);     // reset the USB function / minihost

    CLEAR_MASK(EP1C,_EP1C_EPEN);   // disable endpoint 1
    CLEAR_MASK(EP2C,_EP2C_EPEN);   // disable endpoint 2
    
    EP1C = 0x4000 | 256;
    EP2C = 0x4000 | 64;
    EP3C = 0x4000;
    
    #ifndef __CPU_DEFINE_MB91665_INC__ 
        EP4C = 0x4000;
        EP5C = 0x4000;
    #endif
    
    CLEAR_MASK(EP1C,_EP1C_DIR);  // EP1 is a HOST IN endpoint
    SET_MASK(EP2C,_EP2C_DIR);    // EP2 is a HOST OUT endpoint

    SET_MASK(EP1C,_EP1C_EPEN);   // enable endpoint 1
    SET_MASK(EP2C,_EP2C_EPEN);   // enable endpoint 2
    
    UsbHost_ClearAllEndpoints();
    
    HFCOMP =0;
    
    #ifndef __CPU_DEFINE_MB91665_INC__ 
       HRTIMER0 = 0;
       HRTIMER1 = 0;
    #else
       HRTIMER10 = 0;
    #endif
    HRTIMER2 = 0;
    CLEAR_MASK(HSTATE,_HSTATE_CSTAT);
  
    HADR = 0;  // Device Address = 0
    
    SET_MASK(HCNT, _HCNT_HOST); // Enable Host (HWM rev16 - Chapter 30.4.1)
    SET_MASK(HCNT, _HCNT_HOST); // Enable Host (HWM rev16 - Chapter 30.4.1)
    
    HIRQ = 0;  /* IRQ clear */
    //HSTATE &= 0xef;  /* CLKSEL=0 : Default LowSpeed */
    
    //CLEAR_MASK(UDCC, _UDCC_RST);  // Enable USB   

    #ifndef __CPU_DEFINE_MB91665_INC__ 
       HRTIMER0 = 0;
       HRTIMER1 = 0;
    #else
       HRTIMER10 = 0;
    #endif
    HRTIMER2 = 0;
    
    CLEAR_MASK(HCNT, _HCNT_RETRY);
    
    HEOF = 0x2c9; // Set the time where token are allowed in a frame
    
    TOGGLE_MASK(EP1S,_EP1S_BFINI); // Reset endpoint 1 buffer (IN)
    TOGGLE_MASK(EP2S,_EP2S_BFINI); // Reset endpoint 2 buffer (OUT)
    
    
   
    
    HFCOMP = 0x00;  /* SOF interrupt frame No. for INTERRUPT-IN transfer */
    Usb_SetSOFTimeOutHandler(&SOFHandler);

    
    SET_MASK(HCNT, _HCNT_HOST); // Enable Host (HWM rev16 - Chapter 30.4.1)
    SET_MASK(HCNT, _HCNT_CNNIRE);  // connection interrupt (HWM rev16 - Chapter 30.4.1)
    SET_MASK(HCNT, _HCNT_DIRE);    // disconnect interrupt (HWM rev16 - Chapter 30.4.1)
    SET_MASK(HCNT, _HCNT_CMPIRE);  // token completion interrupt (HWM rev16 - Chapter 30.4.1)
    SET_MASK(HCNT, _HCNT_SOFIRE);  // SOF interrupt (HWM rev16 - Chapter 30.4.1)
    SET_MASK(HCNT, _HCNT_HOST); // Enable Host (HWM rev16 - Chapter 30.4.1)
}

void UsbHost_DeInit(void)
{
    UsbClassSupervisor_Deinit();
    CLEAR_MASK(HCNT, _HCNT_CNNIRE);  // connection interrupt (HWM rev16 - Chapter 30.4.1)
    CLEAR_MASK(HCNT, _HCNT_DIRE);    // disconnect interrupt (HWM rev16 - Chapter 30.4.1)
    CLEAR_MASK(HCNT, _HCNT_CMPIRE);  // token completion interrupt (HWM rev16 - Chapter 30.4.1)
    CLEAR_MASK(HCNT, _HCNT_SOFIRE);  // SOF interrupt (HWM rev16 - Chapter 30.4.1)
    
    SET_MASK(UDCC, _UDCC_RST);     // reset the USB function / minihost
    
    CLEAR_MASK(EP1C,_EP1C_EPEN);   // disable endpoint 1
    CLEAR_MASK(EP2C,_EP2C_EPEN);   // disable endpoint 2
    
    UsbHost_ClearAllEndpoints();
    
    CLEAR_MASK(HCNT, _HCNT_HOST); // Disable Host
    HOSTVBUS_CLEAR;
    HOSTOTGPULLDOWN_CLEAR;
}

uint8_t UsbHost_AddHostEndpoint(HostEndpoint_t** Handler, uint8_t u8EndpointAddress, uint16_t u16FifoSize, uint8_t u8Interval, void(* CompletionHandler)(void))
{
   uint8_t i;
   bSchedulerLock = TRUE;
   bSchedulerLock = TRUE;
   bSchedulerLock = TRUE;
   for(i=0;i<MAX_HOSTENDPOINTS;i++)
   {
      if ((HostEndpoints[i].u16FifoSize == 0) || ((&HostEndpoints[i] == *Handler) && (*Handler != 0)))
      {
          if ((&HostEndpoints[i] == *Handler) && (*Handler != 0))
          {
             DBGOUT(3,dbg("    |->Endpoint exists in list...\n"));
          }
          DBGOUT(3,dbgval("    |->New EP, Address: ",u8EndpointAddress));
          
          HostEndpoints[i].u16FifoSize = u16FifoSize;
          HostEndpoints[i].u8Address = u8EndpointAddress;
          HostEndpoints[i].u8Interval = u8Interval;
          HostEndpoints[i].u8IntervalCount = u8Interval;
          HostEndpoints[i].CompletionHandler = CompletionHandler;
          *Handler = &HostEndpoints[i];
          DBGOUT(3,dbgval("    |->New EP, Index: ",i));
          DBGOUT(3,dbgval("    |->New EP, Handler: ",*Handler));
          u8EndpointListEnd = i;
          bSchedulerLock = FALSE;
          return USBHOST_SUCCESS;
      }
   }
   bSchedulerLock = FALSE;
   bSchedulerLock = FALSE;
   bSchedulerLock = FALSE;
   return USBHOST_ERROR; 
}

void StallCompletionEp0(uint8_t* pu8Buffer, uint32_t u32Size)
{
    //StallCompletionHandler();
    //void(*SetupCompletion)(uint8_t* pu8Buffer, uint32_t u32DataSize)
    dbg("Resending last Setup\n");
    UsbHost_SetupRequest(&stcLastSetupBeforeStall, SetupCompletionHandler);
}

void UsbHost_Scheduler(uint8_t u8Option)
{
   uint8_t i;
   uint8_t u8Priority = 255;
   uint8_t u8NextToken = 0;
   HostEndpoint_t* Handler = 0;
   uint32_t u32DataSize = 0;
   if (u8Option == USBHOST_SCHEDULER_COMPLETION)
   {

       u8SchedulerState = USBHOST_SCHEDULER_IDLE;
       Handler = ActiveEndpoint;
       if (Handler == 0)
       {
          dbg("WARNING: Handler == 0\n");
       }
      
       dbgval("HERR: ",HERR);
       dbgval("TKN: ",(Handler->u8LastToken));
       if ((HERR & 0x03) == HERR_ACK)
       {
           dbg("ACK\n");
           Handler->bToggle = ~Handler->bToggle;
           
           if (((Handler->u8Address) & 0x0F) == 0) // handler == endpoint 0?
           {
               if (((Handler->u8LastToken) & 0x70) == HTOKEN_SETUP) // type == SETUP token?
               {
                   if (((stcLastSetup.bmRequestType) & 0x80) || (stcLastSetup.wLength == 0))
                   {
                       Handler = Ep0IN;
                   }
                   Handler->bToggle = TRUE;
                   Handler->pu8BufferPos = (Handler->pu8Buffer);
                   Handler->u32DataSize = 0;
                   ActiveEndpoint = Handler;
                   if ((stcLastSetup.wLength == 0)) // || ((stcLastSetup.wLength <= (Ep0IN->u16FifoSize)) && (Handler == Ep0IN)))
                   {
                       Ep0OUT->u8Stage = USBHOST_STAGE_STATUS;
                       Ep0IN->u8Stage = USBHOST_STAGE_STATUS;
                   }
                   else
                   { 
                       Ep0OUT->u8Stage = USBHOST_STAGE_DATA;
                       Ep0IN->u8Stage = USBHOST_STAGE_DATA;
                   }
               
                   Handler->u32DataSize = stcLastSetup.wLength;
                   if (((stcLastSetup.bmRequestType) & 0x80) || (stcLastSetup.wLength == 0))
                   {
                       dbg("IT\n");
                       u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_IN,((Handler->u8Address) & 0x0F));
                   }
                   else
                   {
                       dbg("OT\n");
                       Handler->pu8BufferPosNextPackage = pu8SetupData;
                       Handler->pu8BufferPos = pu8SetupData;
                       Handler->pu8Buffer = pu8SetupData;
                       Handler->u32BufferSize = stcLastSetup.wLength;
                       Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
                       Handler->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
                       
                       u32DataSize = (Handler->u32DataSize);
                       u32DataSize -= (uint32_t)((Handler->pu8BufferPos) - (Handler->pu8Buffer));
                       if (u32DataSize > (Handler->u16FifoSize))
                       {
                           u32DataSize = Handler->u16FifoSize;
                       }
                       if (u32DataSize > 0)
                      {
                           dbgval("DPKG l=",u32DataSize);
                           Handler->pu8BufferPosNextPackage += u32DataSize;
                           UsbHost_TransferDataToFifo(Handler->pu8BufferPos,(uint16_t)u32DataSize,Handler->u16FifoSize);
                           SET_TOKEN(Handler,HTOKEN_OUT);
                           //u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_OUT,((Handler->u8Address) & 0x0F));
                      } 
                      else
                      {
                           UsbHost_TransferDataToFifo(0,0,Handler->u16FifoSize);
                           SET_TOKEN(Handler,HTOKEN_OUT);
                           //u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_OUT,((Handler->u8Address) & 0x0F));
                      }
                       
                   }
               } /* END: type == SETUP token? */
               else
               {
                   if (u8DeviceStatus == USBHOST_DEVICE_ADDRESSING)
                   {
                       u8EnumerationCounter = 0;
                       HADR = u8DeviceAddress;
                       dbg("Enumeration Process:\n");
                       dbgval("  ->Addressed:  ",u8DeviceAddress);
                       u8DeviceStatus = USBHOST_DEVICE_ADDRESSED;
                       UsbClassSupervisor_Init();
                   }
                   if (u8DeviceStatus == USBHOST_DEVICE_CONFIGURATING)
                   {
                       dbg("-->Configured\n");
                       u8DeviceStatus = USBHOST_DEVICE_CONFIGURED;
                   }
               }
           } /* END: handler == endpoint 0? */
           
           if ((u8NextToken == 0) && (u8SyncToken == 0))
           {
               Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
               if (((Handler->u8LastToken) & 0x70) == HTOKEN_IN)
               {
                   uint32_t tmp;
                   u32DataSize = (uint32_t)((uint16_t)UsbHost_TransferFifoToBuffer(Handler->pu8BufferPos));
                   (Handler->pu8BufferPos) = (Handler->pu8BufferPos) + u32DataSize;
                   tmp = ((uint32_t)((uint32_t)(Handler->pu8BufferPos) - (uint32_t)(Handler->pu8Buffer)));
                   dbgval("EP#",(uint32_t)(Handler->u8Address));
                   dbgval("#",(uint32_t)(Handler->pu8Buffer));
                   dbgval("DSz ", u32DataSize);
                   dbgval("Sz ", tmp);
                   dbgval("mx: ",(Handler->u32DataSize));
                   if (((u32DataSize == (Handler->u16FifoSize)) && ((Handler->u32DataSize) == 0)) || (((Handler->u32DataSize) > 0) && ((Handler->u32DataSize) > tmp)))
                   {
                       dbg("NXT\n");
                       u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_IN,((Handler->u8Address) & 0x0F));
                   }
                   else
                   {
                       Handler->u32DataSize = (uint32_t)((uint32_t)(Handler->pu8BufferPos) - (uint32_t)(Handler->pu8Buffer));
                       dbgval("RX-Size: ", (Handler->u32DataSize));
                       Handler->pu8BufferPos = (Handler->pu8Buffer);
                       Handler->pu8BufferPosNextPackage = (Handler->pu8Buffer);
                       Handler->u8Status |= USBHOST_ENDPOINTSTATUS_DATA;
                       Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
                       
                       if ((((Handler->u8Address) & 0x0F) == 0) && ((Handler->u8Stage) == USBHOST_STAGE_DATA))
                       {
                           Ep0IN->u8Stage = USBHOST_STAGE_STATUS;
                           Ep0OUT->u8Stage = USBHOST_STAGE_STATUS;
                           Ep0OUT->bToggle = TRUE;
                           Ep0OUT->u32DataSize = 0;
                           Ep0OUT->pu8BufferPos = (Ep0OUT->pu8Buffer);
                           Ep0OUT->pu8BufferPosNextPackage = (Ep0OUT->pu8BufferPos);
                           Ep0OUT->CompletionHandler = Ep0IN->CompletionHandler;
                           UsbHost_TransferDataToFifo(0,0,(Ep0OUT->u16FifoSize));
                           SET_TOKEN(Ep0OUT,HTOKEN_OUT); // synchronous token (sent after next SOF)
                       }
                       else
                       {
                           if ((Handler->CompletionHandler) != 0)
                           {
                               dbg("Exec(IN)\n");
                               Handler->CompletionHandler();
                           }
                           ActiveEndpoint = 0;
                       }
                   }
               }
               else // Sending Data
               {
                   Handler->pu8BufferPos = (Handler->pu8BufferPosNextPackage);
                   u32DataSize = (Handler->u32DataSize);
                   u32DataSize -= (uint32_t)((Handler->pu8BufferPos) - (Handler->pu8Buffer));
                   if (u32DataSize > (Handler->u16FifoSize))
                   {
                       u32DataSize = Handler->u16FifoSize;
                   }
                   if (u32DataSize > 0)
                   {
                       Handler->pu8BufferPosNextPackage += u32DataSize;
                       UsbHost_TransferDataToFifo(Handler->pu8BufferPos,(uint16_t)u32DataSize,Handler->u16FifoSize);
                       u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_OUT,((Handler->u8Address) & 0x0F));
                   }
                   else
                   {
                       u8NextToken = 0;
                       Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
                       if ((((Handler->u8Address) & 0x0F) == 0) && ((Handler->u8Stage) == USBHOST_STAGE_DATA))
                       {
                           Ep0IN->u8Stage = USBHOST_STAGE_STATUS;
                           Ep0OUT->u8Stage = USBHOST_STAGE_STATUS;
                           Ep0IN->CompletionHandler = Ep0OUT->CompletionHandler;
                           Ep0IN->bToggle = TRUE;
                           SET_TOKEN(Ep0IN,HTOKEN_IN); // synchronous token (sent after next SOF)
                           //u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_OUT,((Handler->u8Address) & 0x0F));
                       }
                       else
                       {
                           if (Handler->u8Interval != 0)
                           {
                               Handler->u32DataSize = 0;
                           }
                           if ((Handler->CompletionHandler) != 0)
                           {
                               Handler->CompletionHandler();
                           }
                       }
                       ActiveEndpoint = 0;
                   }
    
               }
           } /* END: ((u8NextToken == 0) && (u8SyncToken == 0)) */
       } /* END: ((HERR & 0x03) == HERR_ACK) */
       
       if ((HERR & 0x08) > 0)
       {
           dbg("Toggle Error\n");
           CLEAR_MASK(HERR,_HERR_TGERR);
           //Handler->bToggle = ~Handler->bToggle;
       }
       if ((HERR & 0x03) == HERR_NAK)
       {
           dbg("NAK\n");
           if (((Handler->u8Interval) > 0) && ((Handler->pu8Buffer) == (Handler->pu8BufferPos)))
           {
               dbg("Cancel\n");
               Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
               Handler = 0;
               ActiveEndpoint = 0;
           }
           else
           {   
               dbg("RTY\n");
               Handler = ActiveEndpoint;

               if (Handler->bAbortTransfer == TRUE)
               {
                   dbg("-> CANCELED\n");
                   Handler->u32DataSize = 0;
                   Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
                   Handler->bAbortTransfer = FALSE;
                   //Handler->bToggle = ~Handler->bToggle;
                   ActiveEndpoint = 0;
                   return;
                   /*if ((Handler->CompletionHandler) != 0)
                   {
                      Handler->CompletionHandler();
                   }
                   Handler = 0;*/
                   
               }
               else
               {
                   //u8NextToken = Handler->u8LastToken;
                   u8SyncToken = Handler->u8LastToken;
                   Handler = 0;
               }
           }
       }
       if ((HERR & 0x03) == HERR_NULL)
       {
           dbg("NULL\n");
           dbgval("HERR: ",HERR);
           /*if (IS_SET(HERR,_HERR_TOUT))
           {
               dbg("Resetting TOUT\n");
               CLEAR_MASK(HERR,_HERR_TOUT);
               Handler = ActiveEndpoint;
               if (((Handler->u8Address) & 0x80) == 0)
               {
               u32DataSize = (Handler->u32DataSize);
               u32DataSize -= (uint32_t)((Handler->pu8BufferPos) - (Handler->pu8Buffer));
               if (u32DataSize > (Handler->u16FifoSize))
               {
                   u32DataSize = Handler->u16FifoSize;
               }
               if (u32DataSize > 0)
               {
                   Handler->pu8BufferPosNextPackage += u32DataSize;
                   UsbHost_TransferDataToFifo(Handler->pu8BufferPos,(uint16_t)u32DataSize,Handler->u16FifoSize);
               }
               }
               u8NextToken = Handler->u8LastToken;
           }*/
       }
       if ((HERR & 0x03) == HERR_STALL)
       {
           dbg("STALL\n");
           Handler = ActiveEndpoint;
           Handler->u8Status |= USBHOST_ENDPOINTSTATUS_STALL;
           if (((Handler->u8Address) & 0x0F) == 0)
           {
               dbg("Clearing STALL EP0\n");
               StallCompletionHandler = Handler->CompletionHandler;
               xmemcpy((uint8_t*)&stcLastSetupBeforeStall,(uint8_t*)&stcLastSetup,sizeof(stcLastSetup));
               UsbHost_SetupRequest((UsbRequest_t *)&stcClearStallEp0,StallCompletionEp0);
           }
           else
           {
               if ((Handler->CompletionHandler) != 0)
               {
                   Handler->CompletionHandler();
               }
           }
       }
   } /* END: (u8Option == USBHOST_SCHEDULER_COMPLETION) */
   
   if ((u8Option == USBHOST_SCHEDULER_SOF)) // && (u8DeviceStatus >= 3)) 
   {
       if (u8SetupDelay > 0)
       {
           u8SetupDelay--;
           dbg("SETUP WAIT\n");
       }
       else
       {
           if (u8SyncToken != 0)
           {
               dbg("SOF_T\n");
               if ((u8SyncToken & 0x0F) != 0)
               {
                   Handler = FromEndpointNumber(u8SyncToken & 0x0F);
               }
               else
               {
                   if ((u8SyncToken & 0x70) == HTOKEN_IN)
                   {
                       Handler = Ep0IN;
                   }
                   else
                   {
                       Handler = Ep0OUT;
                   }
               }
               Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
               u8NextToken = u8SyncToken;
               u8SyncToken = 0;
               u8Priority = 0;
           }
       }
       bSofToken = TRUE;
       if ((u8NextToken == 0) && (Handler == 0))
       {
           for(i=0;i<=u8EndpointListEnd;i++)
           {
               if (HostEndpoints[i].u8Interval != 0)
               {
                   if (HostEndpoints[i].u8IntervalCount == 0)
                   {
                       if (HostEndpoints[i].u8Interval < u8Priority)
                       {
                           u8Priority = HostEndpoints[i].u8Interval;
                           if (ActiveEndpoint == 0)
                           {
                               Handler = &HostEndpoints[i];
                           }
                       }
                   }
                   else
                   {
                       HostEndpoints[i].u8IntervalCount--;
                   }
               }
           }
       }
       if (Handler == 0)
       {
           if ((HostEndpoints[u8CurrentEndpointPosition].u8Interval) == 0)
           {
               Handler = &HostEndpoints[u8CurrentEndpointPosition];
           }
       } 
       else
       {
           if (!((Handler->u8Status) & USBHOST_ENDPOINTSTATUS_INUSE))
           {
               if (((Handler->u8Address) & 0x80))
               {
                   Handler->u8Status |= USBHOST_ENDPOINTSTATUS_INITIATE;
               }
               else
               {
                   if (((Handler->u32DataSize) != 0) && ((Handler->pu8BufferPos) == (Handler->pu8Buffer)))
                   {
                       Handler->u8Status |= USBHOST_ENDPOINTSTATUS_INITIATE;
                   }
               }
           }
           /*if ((((Handler->u8Address) & 0x80) == 0) && ((Handler->u32DataSize) != 0) && ((Handler->pu8BufferPos) == (Handler->pu8Buffer)))
           {
               Handler->u8Status |= USBHOST_ENDPOINTSTATUS_INITIATE;
           }*/
       }
   }
   /*if (Handler != 0)
   {
      DBGOUT(15,dbgval("Endpoint ", Handler->u8Address));
   }*/
   
   if ((Handler != 0) && (Handler->bAbortTransfer == TRUE))
   {
       dbg("-> CANCELED\n");
       Handler->u32DataSize = 0;
       Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INUSE;
       Handler->bAbortTransfer = FALSE;
       //Handler->bToggle = ~Handler->bToggle;
       ActiveEndpoint = 0;
       return;
       /*if ((Handler->CompletionHandler) != 0)
       {
          Handler->CompletionHandler();
       }
       Handler = 0;*/
       
   }
   
   if ((Handler != 0) && ((Handler->u8Status) & USBHOST_ENDPOINTSTATUS_INITIATE) && (u8NextToken == 0) && (u8SchedulerState == USBHOST_SCHEDULER_IDLE))
   {
       DBGOUT(4,dbgval("Init Token, Endpoint: ", Handler->u8Address));
       dbgval("IT:", Handler->u8Address);
       Handler->pu8BufferPos = (Handler->pu8Buffer);
       Handler->pu8BufferPosNextPackage = (Handler->pu8Buffer);
       Handler->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
       Handler->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
       Handler->u8IntervalCount = (Handler->u8Interval);
       if ((Handler->u8Address) & 0x80)
       {
           //Handler->u32DataSize = 0;
           u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_IN,((Handler->u8Address) & 0x0F));
           //SET_TOKEN(Handler,HTOKEN_IN);
       }
       else
       {
           u32DataSize = (Handler->u32DataSize);
           if (u32DataSize > (Handler->u16FifoSize))
           {
               u32DataSize = Handler->u16FifoSize;
           }
           if (u32DataSize > 0)
           {
               Handler->pu8BufferPosNextPackage += u32DataSize;
               UsbHost_TransferDataToFifo(Handler->pu8BufferPos,(uint16_t)u32DataSize,Handler->u16FifoSize);
               u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_OUT,((Handler->u8Address) & 0x0F));
               //SET_TOKEN(Handler,HTOKEN_OUT);
           }
           else
           {
               dbg("Sending 0 byte\n");
               UsbHost_TransferDataToFifo(Handler->pu8BufferPos,(uint16_t)u32DataSize,Handler->u16FifoSize);
               u8NextToken = SETUP_TOKEN(Handler->bToggle,HTOKEN_OUT,((Handler->u8Address) & 0x0F));
               //SET_TOKEN(Handler,HTOKEN_OUT);
           }
       }
   }
   
   u8CurrentEndpointPosition++;
   if (u8CurrentEndpointPosition > u8EndpointListEnd) 
   {
       u8CurrentEndpointPosition = 0;
   }
   
   if ((u8NextToken != 0) && (Handler != 0))
   {
       dbg("<T>");
       bSofToken = FALSE;
       u8LastToken = u8NextToken;
       ActiveEndpoint = Handler;
       Handler->u8LastToken = u8NextToken;
       u8SchedulerState = USBHOST_SCHEDULER_BUSY;
       Handler->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
       HTOKEN = u8NextToken;
   }
}

void UsbHost_ClearAllEndpoints()
{
   uint8_t i;
   u8EndpointListEnd = 0;
   for(i=0;i<MAX_HOSTENDPOINTS;i++)
   {
       xmemset((uint8_t*)&HostEndpoints[i],0,sizeof(HostEndpoints[i]));
   }
}

void UsbHost_BusReset()
{
    volatile uint32_t u32WaitTime = 10000;
    if (IS_SET(HSTATE,_HSTATE_CSTAT))
    {
        DBGOUT(1,dbg(">> Exec Bus Reset...\n"));
        
        HADR = 0;  // Device Address = 0
        
        TOGGLE_MASK(EP1S,_EP1S_BFINI); // Reset endpoint 1 buffer (IN)
        TOGGLE_MASK(EP2S,_EP2S_BFINI); // Reset endpoint 2 buffer (OUT)
        
        SET_MASK(HCNT,_HCNT_URST); // initiate bus reset
        while(u32WaitTime--) __wait_nop();
    }
    else
    {
        UsbHost_Init();
    }
}

void UsbHost_StartSOFToken()
{
    DBGOUT(1,dbg(">> Starting SOF\n"));
    HFRAME = 0;
    HEOF = 0x2c9; // Set the time where token are allowed in a frame
    
    SET_MASK(HCNT,_HCNT_SOFSTEP); 
    
    //if(IS_SET(HSTATE, _HSTATE_TMODE))  /* TMODE : Full-Speed */
    //{
    //    SET_MASK(HSTATE,_HSTATE_SOFBUSY);
    //}
    //else
    //{
    //    CLEAR_MASK(HSTATE,_HSTATE_SOFBUSY);
    //}
    
    Ep0OUT->bToggle = FALSE;
    
    SET_TOKEN_NOW(Ep0OUT,HTOKEN_SOF);
    //SETUP_TOKEN(0,HTOKEN_SOF,0);
}

void UsbHost_SetupRequestWithData(UsbRequest_t* pstcSetup, uint8_t* pu8Data, void(*SetupCompletion)(uint8_t* pu8Buffer, uint32_t u32DataSize))
{
    pu8SetupData = pu8Data;
    UsbHost_SetupRequest(pstcSetup,SetupCompletion);
}
void UsbHost_SetupRequest(UsbRequest_t* pstcSetup, void(*SetupCompletion)(uint8_t* pu8Buffer, uint32_t u32DataSize))
{
    bSchedulerLock = TRUE;
    SetupCompletionHandler = SetupCompletion;
    if ((((Ep0IN->u8Status) & USBHOST_ENDPOINTSTATUS_STALL) == 0) && (((Ep0OUT->u8Status) & USBHOST_ENDPOINTSTATUS_STALL) == 0))
    {
        xmemcpy((uint8_t*)&stcLastSetup,(uint8_t*)pstcSetup,sizeof(stcLastSetup));
    }
    ConvertRequestToByteArray(u8Setup,pstcSetup);
    dbgval("Setup [0]",u8Setup[0]);
    dbgval("[1]",u8Setup[1]);
    dbgval("[2]",u8Setup[2]);
    dbgval("[3]",u8Setup[3]);
    dbgval("[4]",u8Setup[4]);
    dbgval("[5]",u8Setup[5]);
    dbgval("[6]",u8Setup[6]);
    dbgval("[7]",u8Setup[7]);

    UsbHost_TransferDataToFifo(u8Setup,8,Ep0OUT->u16FifoSize);
    Ep0OUT->u8Stage = USBHOST_STAGE_SETUP;
    Ep0IN->u8Stage = USBHOST_STAGE_SETUP;
    
    Ep0IN->pu8BufferPos = Ep0IN->pu8Buffer;
    Ep0IN->pu8BufferPosNextPackage = Ep0IN->pu8Buffer;
    Ep0IN->u8Status &= ~USBHOST_ENDPOINTSTATUS_DATA;
    Ep0OUT->bToggle = FALSE;
    Ep0OUT->u32DataSize = 8;
    Ep0OUT->pu8Buffer = u8Setup;
    Ep0OUT->pu8BufferPos = u8Setup;
    Ep0OUT->u32BufferSize = 8;
    Ep0IN->CompletionHandler = SetupComplete;
    Ep0OUT->CompletionHandler = SetupComplete;
    

    Ep0OUT->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
    Ep0OUT->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
    Ep0IN->u8Status &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
    Ep0IN->u8Status |= USBHOST_ENDPOINTSTATUS_INUSE;
    u8SetupDelay = 3;
    ActiveEndpoint=Ep0OUT;
    //UsbHost_TransferData(Ep0OUT,u8Setup,8,SetupComplete);
    SET_TOKEN(Ep0OUT,HTOKEN_SETUP); // synchronous token (sent after next SOF)
    bSchedulerLock = FALSE;
}

void SetupComplete(void)
{
    if (SetupCompletionHandler != 0)
    {
        SetupCompletionHandler((Ep0IN->pu8Buffer),(Ep0IN->u32DataSize));
    }
}

/*

uint8_t* UsbHost_RequestDeviceDescriptor()
{
    unsigned int u16RemainingLength; 
    SETUP_DATA setup;
    ZERO_STRUCT(setup);
    setup.Bytes.Type = USB_IN_DIRECTION; //get interface des
    setup.Bytes.Request = USB_REQ_GET_DESCRIPTOR;
    setup.Bytes.ValueH = USB_DEVICE_DESCRIPTOR_TYPE;
    setup.Words.Length = DESCRIPTOR_BUFFER_SIZE;
    
    if(UMH_SetupRequest((uint8_t*)&setup,pu8Buffer,&u16RemainingLength) > 0) 
    {
        return 0;
    }
    return pu8Buffer;
}
*/

void UsbHost_SetConfigurationDescriptor(uint8_t u8Configuration)
{
    UsbRequest_t stcSetup = {0x00,0x09,0x00,0x00,0x00};
    stcSetup.wValue |= u8Configuration;
    dbgval("Setting Configuration: ",u8Configuration);
    u8DeviceStatus = USBHOST_DEVICE_CONFIGURATING;
    UsbHost_SetupRequest(&stcSetup,0);
}

/*
void UsbHost_GetConfigurationDescriptor(uint8_t u8Configuration)
{
    uint8_t* pu8Buffer;
    uint32_t u32Size = 0;
    UsbRequest_t stcSetup = {0x80,0x06,0x0200,0x00,0x08};
    stcSetup.wValue |= u8Configuration;
    DBGOUT(1,dbg("Requesting first 8 bytes of Configuration\n"));
    UsbHost_SetupRequest(&stcSetup);
    u32Size=0;
    while(u32Size == 0)
    {
        u32Size = ReceiveSetupData(&pu8Buffer);
    }
    printBuffer(pu8Buffer,u32Size);
    stcSetup.wLength = ((uint8_t)pu8Buffer[2]) | (((uint8_t)pu8Buffer[3]) << 8);
    DBGOUT(1,dbgval("Requesting full Configuration Length of ", stcSetup.wLength));
    UsbHost_SetupRequest(&stcSetup);
}
*/

uint8_t* UsbHost_GetUsbEndpointDescriptor(uint8_t* pu8Buffer, uint16_t u16Size, uint8_t u8InterfaceNumber, uint8_t u8EndpointNumber)
{
    uint16_t u16Position = 0;
    uint8_t u8DescriptorLength = 0;
    uint8_t u8DescriptorType = 0;
    uint8_t u8CurrentInterface = 0xFF;
    uint8_t u8CurrentEndpoint = 0;
    uint16_t u16wTotalLength;
    

    if (pu8Buffer[1] != USB_CONFIGURATION_DESCRIPTOR_TYPE) 
    {
        return 0;
    }
    
    u16wTotalLength = (uint16_t)pu8Buffer[2] + (uint16_t)(pu8Buffer[3] << 8);
    if (u16Size < u16wTotalLength)
    {
       u16wTotalLength = u16Size;
    }
    
    while(u16Position < u16wTotalLength)
    {
       u8DescriptorLength = pu8Buffer[u16Position];
       u8DescriptorType = pu8Buffer[u16Position + 1];
       if (u8DescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE)
       {
           u8CurrentEndpoint = 0;
           u8CurrentInterface = pu8Buffer[u16Position + 2];
       }
       if (u8DescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE)
       {
           u8CurrentEndpoint = u8CurrentEndpoint + 1;
       }
       if (((u8CurrentEndpoint == u8EndpointNumber) || (u8EndpointNumber == 0)) && (u8InterfaceNumber == u8CurrentInterface))
       {
           return (uint8_t*)(pu8Buffer + u16Position);
       }
       u16Position += u8DescriptorLength;
    }
    return 0;
}

void UsbHost_AddressDevice(uint8_t NewAddress)
{
    UsbRequest_t stcSetup = {0x00,0x05,0x02,0x00,0x00};
    DBGOUT(1,dbgval("Start Addressing:  ",NewAddress));
    stcSetup.wValue = NewAddress;
    u8DeviceAddress = NewAddress;  // Device Address = NewAddress
    u8DeviceStatus = USBHOST_DEVICE_ADDRESSING;
    UsbHost_SetupRequest(&stcSetup,EnumerationStateMachine);
}

void ConvertRequestToByteArray(uint8_t* pu8Buffer, UsbRequest_t* pstcSetup)
{
    pu8Buffer[0] = pstcSetup->bmRequestType;
    pu8Buffer[1] = pstcSetup->bRequest;
    pu8Buffer[2] = (uint8_t)(pstcSetup->wValue & 0xFF);
    pu8Buffer[3] = (uint8_t)((pstcSetup->wValue >> 8) & 0xFF);
    pu8Buffer[4] = (uint8_t)(pstcSetup->wIndex & 0xFF);
    pu8Buffer[5] = (uint8_t)((pstcSetup->wIndex >> 8) & 0xFF);
    pu8Buffer[6] = (uint8_t)(pstcSetup->wLength & 0xFF);
    pu8Buffer[7] = (uint8_t)((pstcSetup->wLength >> 8) & 0xFF);
}

void UsbHost_TransferDataToFifo(uint8_t* pu8Buffer, uint16_t u16Size, uint16_t u16FifoSize)
{
    boolean_t Odd = u16Size & 1;
    dbgval("TX: ",u16Size);
    dbgval("#",pu8Buffer);
    EP2S = (EP2S & (~0x1F)) | u16FifoSize;

    u16Size = u16Size / 2; // transfer 16 Bit words;
    while(u16Size--)	
    {
	    //EP2DT = *(uint16_t*)pu8Buffer;
	    EP2DTL = *pu8Buffer++;
	    EP2DTH = *pu8Buffer++;
	    //pu8Buffer += 2;
    }
    if(Odd == TRUE)  {
        EP2DTL= *(uint8_t*)pu8Buffer; // transfer the first or last byte
    }
    CLEAR_MASK(EP2S,_EP2S_DRQ);	 // now the OUT FIFO is valid for the next transfer
}

uint16_t UsbHost_TransferFifoToBuffer(uint8_t* pu8Buffer)
{
    uint16_t u16Size = (uint16_t)(EP1S & 0x00FF);
    uint16_t u16ReceivedSize = u16Size;
    dbgval("RX: ",u16Size);
    dbgval("#",pu8Buffer);
    u16Size = u16Size / 2; // transfer 16 Bit words;
    while(u16Size--)	
    {
	    //*(uint16_t*)pu8Buffer = EP1DT;
	    *pu8Buffer++ = EP1DTL;
	    *pu8Buffer++ = EP1DTH;
	    //pu8Buffer += 2;
    }
    if(u16ReceivedSize & 1 == TRUE)  {
        *(uint8_t*)pu8Buffer = EP1DTL; // transfer the first or last byte
    }	
    CLEAR_MASK(EP1S,_EP1S_DRQ);	 // now the IN FIFO is valid for the next transfer
    return u16ReceivedSize;
}

HostEndpoint_t* FromEndpointNumber(uint8_t EndpointNumber)
{
   uint8_t i;
   for(i=0;i<=u8EndpointListEnd;i++)
   {
       if ((HostEndpoints[i].u8Address & 0x0F) == EndpointNumber)
       {
           return &HostEndpoints[i];
       }
   }
   return 0;
}

void UsbHost_TransferData(HostEndpoint_t* Handler,uint8_t* pu8Buffer, uint32_t u32BufferSize, void (* CompletionHandler)(void))
{
    bSchedulerLock = TRUE;
    (Handler->u8Status) &= ~USBHOST_ENDPOINTSTATUS_INITIATE;
    if ((Handler->u8Interval) == 0)
    {
        (Handler->u8Status) |= USBHOST_ENDPOINTSTATUS_INUSE;
    }
    Handler->bAbortTransfer = FALSE;
    (Handler->u32DataSize) = u32BufferSize;
    (Handler->pu8Buffer) = pu8Buffer;
    (Handler->pu8BufferPos) = pu8Buffer;
    (Handler->u32BufferSize) = u32BufferSize;
    (Handler->CompletionHandler) = CompletionHandler;
    (Handler->u8Status) &= ~USBHOST_ENDPOINTSTATUS_INUSE;
    dbgval("RX(Init)->EP:", (Handler->u8Address));
    dbgval("RX(Init)->Status:", (Handler->u8Status));
    dbgval("RX(Init)->Buffer:", (Handler->pu8Buffer));
    dbgval("RX(Init)->BufferSize:", (Handler->u32BufferSize));
    /*
    dbgval("RX(Init)->Buffer:", (Handler->pu8Buffer));
    dbgval("RX(Init)->BufferPos:", (Handler->pu8BufferPos));
    dbgval("RX(Init)->DataSize:", (Handler->u32DataSize));
    
    */
    if ((Handler->u8Interval) == 0)
    {
        (Handler->u8Status) |= USBHOST_ENDPOINTSTATUS_INITIATE;
    }
    bSchedulerLock = FALSE;
    //Handler->u8Status |= USBHOST_ENDPOINTSTATUS_DATA;
}

uint8_t UsbHost_GetDeviceStatus()
{
    return u8DeviceStatus;
}


uint32_t ReceiveSetupData(uint8_t** pu8Buffer)
{
    uint32_t u32Size;
    if (Ep0IN->u8Status & USBHOST_ENDPOINTSTATUS_DATA)
    {
        Ep0IN->u8Status &= ~USBHOST_ENDPOINTSTATUS_DATA;
        *pu8Buffer = Ep0IN->pu8Buffer;
        u32Size = (Ep0IN->u32DataSize);
        return u32Size;
    }
    *pu8Buffer = 0;
    return 0;
}


void EnumerationStateMachine()
{
    uint8_t* pu8SetupBuffer;
    uint32_t u32Size = ReceiveSetupData(&pu8SetupBuffer);
    Ep0IN->CompletionHandler = 0;
    if (IS_SET(HSTATE,_HSTATE_SUSP))
    {

    }
    if (pu8SetupBuffer != 0)
    {
        if (u32Size > 0)
        {
            printBuffer(pu8SetupBuffer,u32Size);
            if (pu8SetupBuffer[1] == 0x02)
            {
                dbg("Received Configuration\n");
            }
            if (pu8SetupBuffer[1] == 0x01)
            {
                dbg("Enumeration Process:\n");
                dbg("  ->Received Device Descriptor\n");
                dbgval("  ->New EP0 buffersize: ",(uint16_t)((uint8_t)pu8SetupBuffer[7]));
                Ep0IN->u16FifoSize = (uint16_t)((uint8_t)pu8SetupBuffer[7]);
                Ep0OUT->u16FifoSize = (uint16_t)((uint8_t)pu8SetupBuffer[7]);
            }
        }
        else
        {
            if (u8DeviceStatus == USBHOST_DEVICE_ADDRESSING)
            {
                 u8EnumerationCounter = 0;
                 HADR = u8DeviceAddress;
                 dbg("Enumeration Process:\n");
                 dbgval("  ->Addressed:  ",u8DeviceAddress);
                 u8DeviceStatus = USBHOST_DEVICE_ADDRESSED;
                 UsbClassSupervisor_Init();
            }
        }
    }
    else
    {
        if (Ep0IN->u16FifoSize == 20)
        {
            dbg("Enumeration Process:\n");
            dbg("  -> Endpoint 0 has no Fifo size specified.\n"); 
            dbg("     Requesting Device Descriptor.\n");
            UsbHost_SetupRequest((UsbRequest_t *)&stcShortDeviceDescriptorRequest,0);
        }
        else
        {            
            if (u8DeviceStatus == USBHOST_DEVICE_IDLE)
            {
                dbg("Enumeration Process:\n");
                Ep0IN->CompletionHandler = EnumerationStateMachine;
                u8DeviceStatus = USBHOST_DEVICE_ADDRESSING;
                UsbHost_AddressDevice(2);
                u8EnumerationCounter = 0;
            }
        }
    }
    if (u8DeviceStatus != USBHOST_DEVICE_ADDRESSED)
    {
        u8EnumerationCounter++;
        if (u8EnumerationCounter == 40)
        {
            u32ConnectionDelay += 100;
            UsbHost_DeInit();
            UsbHost_Init();
        }
        
        if (u8EnumerationCounter > 50)
        {
            UsbRequest_t stcSetup;
            u8DeviceStatus = USBHOST_DEVICE_IDLE; 
            dbg("Enumeration timed out...\n");
            u8EnumerationCounter = 0;
            dbg("ENUMERATION FAILED - SYSTEM HALT\n");
            stcSetup.bmRequestType=0x82;  
            stcSetup.bRequest=0x00;
            stcSetup.wValue=0x00;  
            stcSetup.wIndex=0x00;
            stcSetup.wLength=0x02;
            UsbHost_SetupRequest((UsbRequest_t *)&stcShortDeviceDescriptorRequest,EndpointInStatusUpdate);
        }
        else
        {
            Usb_AddTimeOut(EnumerationStateMachine,100);
        }
    }
}

void EndpointInStatusUpdate(uint8_t* pu8Buffer, uint32_t u32Size)
{
    UsbRequest_t stcSetup;
    stcSetup.bmRequestType=0x82;  
    stcSetup.bRequest=0x00;
    stcSetup.wValue=0x00;  
    stcSetup.wIndex=0x80;
    stcSetup.wLength=0x02;
    if (u32Size > 0)
    {
       dbgval("Endpoint IN Status: ", pu8Buffer[0]);
    }
    UsbHost_SetupRequest((UsbRequest_t *)&stcShortDeviceDescriptorRequest,EndpointOutStatusUpdate);
}

void EndpointOutStatusUpdate(uint8_t* pu8Buffer, uint32_t u32Size)
{
    if (u32Size > 0)
    {
       dbgval("Endpoint OUT Status: ", pu8Buffer[0]);
    }
}


void UsbDummyFunction(void)
{
  volatile uint8_t dummy;
  for(dummy = 0;dummy < 10;dummy++)
  {
  }
  //dummy = UCCR;
  /*dummy = UCCR;
  dummy = UCCR;
  dummy = UCCR;
  dummy = UCCR;*/
}

#if (USBMCUTYPE == USBMCU_MB91665)
    void UsbHost_ISR(void)
#else
    __interrupt void UsbHost_ISR(void)
#endif
{
    if (IS_SET(HIRQ,_HIRQ_DIRQ))
    {
        CLEAR_MASK(HIRQ,_HIRQ_DIRQ);
        dbg("\n### Device Disconnection...\n");
        u8DeviceStatus = USBHOST_DEVICE_IDLE; 
        u32BusResetTimeout = 0;
        u32ConnectionDelay = 1000;
        u32ConTimeout = 0;
        bBusResetSend = FALSE;
        UsbClassSupervisor_Deinit();
        UsbHost_ClearAllEndpoints();
    }
    if (IS_SET(HIRQ,_HIRQ_CNNIRQ))
    {
        if (u32ConTimeout == 0)
        {
            if (IS_CLEARED(HIRQ,_HIRQ_URIRQ) || (bBusResetSend == FALSE))
            {
                SET_MASK(UDCC,_UDCC_RST);      
                if(IS_SET(HSTATE, _HSTATE_TMODE))  /* TMODE : Full-Speed */
                { 
                    dbg("Full-Speed Device\n");
                    CLEAR_MASK(HSTATE,_HSTATE_ALIVE);
                    SET_MASK(HSTATE,_HSTATE_CLKSEL);
                    bFullSpeed = TRUE;
                    UsbDummyFunction();
                    UsbDummyFunction();
                    
                }
                else /* Low-Speed */
                {       
                    dbg("Low-Speed Device\n"); 
                    bFullSpeed = FALSE;
                    SET_MASK(HSTATE,_HSTATE_ALIVE);
                    CLEAR_MASK(HSTATE,_HSTATE_CLKSEL);
                    UsbDummyFunction();
                    UsbDummyFunction();
                }
                CLEAR_MASK(UDCC,_UDCC_RST);
            }
            dbg("\n### Device Connection - wait...\n");
            if(IS_SET(HSTATE, _HSTATE_TMODE))  /* TMODE : Full-Speed */
            {
                u32ConTimeout = u32ConnectionDelay;
            }
            else
            {
                u32ConTimeout = u32ConnectionDelay;
            }
            return;
        }
        u32ConTimeout--;
        if (u32ConTimeout > 0)
        {
            return;
        }
        dbg("\n### Device Connection...\n");
        CLEAR_MASK(HIRQ,_HIRQ_CNNIRQ);

	     
	    
        if (IS_CLEARED(HIRQ,_HIRQ_URIRQ) || (bBusResetSend == FALSE))
        {
            bBusResetSend = TRUE;          
            //CLEAR_MASK(HCNT,_HCNT_HOST);// Disable Host (HWM rev16 - Chapter 30.4.1)
            //SET_MASK(HCNT,_HCNT_HOST); // Enable Host (HWM rev16 - Chapter 30.4.1)
            UsbHost_BusReset();
        }
        else
        {
            dbg("    |->Bus Resetted\n");
            CLEAR_MASK(HIRQ,_HIRQ_URIRQ);

            //SET_MASK(UDCC,_UDCC_RST);  // Clear Bus Reset Flag    
            
            if(IS_SET(HSTATE, _HSTATE_TMODE))  /* TMODE : Full-Speed */
            { 
                /*dbg("Full-Speed Device\n");
                SET_MASK(HSTATE,_HSTATE_CLKSEL);
                UsbDummyFunction();
                CLEAR_MASK(HSTATE,_HSTATE_ALIVE);
                UsbDummyFunction();*/
            }
            else /* Low-Speed */
            {       
                /*dbg("Low-Speed Device\n");     
                CLEAR_MASK(HSTATE,_HSTATE_CLKSEL);
                UsbDummyFunction();
                SET_MASK(HSTATE,_HSTATE_ALIVE);
                UsbDummyFunction();*/
            }
            
            //CLEAR_MASK(UDCC,_UDCC_RST);
            
            UsbHost_AddHostEndpoint(&Ep0IN ,0x80,20,0,0);
            UsbHost_AddHostEndpoint(&Ep0OUT,0x00,20,0,0);
            Ep0IN->u32BufferSize = 255;
            Ep0OUT->u32BufferSize = 255;
            //HSTATE0_TMODE  = 1;
            Ep0IN->pu8Buffer  = pu8GlobalBuffer;
            Ep0OUT->pu8Buffer = pu8GlobalBuffer;
            /*u32Timeout = 1000000;
            while(u32Timeout > 1) u32Timeout--;*/
            UsbHost_StartSOFToken();
            u8EnumerationCounter = 0;
            Usb_AddTimeOut(EnumerationStateMachine,500);
        }
    }
    if (IS_SET(HIRQ,_HIRQ_URIRQ))
    {
        dbg("### Bus Resetted\n");
        CLEAR_MASK(HIRQ,_HIRQ_URIRQ);
    }
    
    if (IS_SET(HIRQ,_HIRQ_SOFIRQ))
    {
        CLEAR_MASK(HIRQ,_HIRQ_SOFIRQ);
        //dbg("'");
        if (SOFHandler != 0)
        {
           SOFHandler(TMSP & 0x7FF);
        }
        if (bSchedulerLock == FALSE)
        {
            UsbHost_Scheduler(USBHOST_SCHEDULER_SOF);
        }
        else
        {
            dbg("SOF LOCK!\n");
        }

    }
    
    if (IS_SET(HIRQ,_HIRQ_CMPIRQ))
    {
        volatile uint16_t i = 1000;
        while(((HTOKEN & 0x70) != 0) && (i--)) //wait Token Register is ready
        {
            __wait_nop();
        }
        dbg("\n!\n");
        if (bSchedulerLock == FALSE)
        { 
            CLEAR_MASK(HIRQ,_HIRQ_CMPIRQ); 
            UsbHost_Scheduler(USBHOST_SCHEDULER_COMPLETION);
        }
        else
        {
           dbg("COMPLETION LOCK!\n");
        }
        //dbg("\n!e\n");
    }
    
    if (IS_SET(HIRQ,_HIRQ_RWKIRQ))
    {
        dbg("### Remote Wakeup...\n");
        CLEAR_MASK(HIRQ,_HIRQ_RWKIRQ);
    }
    if (IS_SET(HIRQ,_HIRQ_TCAN))
    {
        dbg("### Token canceled...\n");
        CLEAR_MASK(HIRQ,_HIRQ_TCAN);
    }
}

void xmemset(uint8_t *dest, uint8_t fill, uint32_t length)
{
    dbgval("MEMSET Size: ", length);
    while(length--)
    {
        *dest++ = fill;
    }
}

void xmemcpy(uint8_t *dest,uint8_t *src, uint32_t length)
{
    dbgval("MEMCPY Size: ", length);
    while(length--)
    {
        *((uint8_t*)dest) = *((uint8_t*)src);
        dest++;
        src++;
    }
}
#if (debug_enabled == 1)
void dbg_HERR(void)
{
    if (HERR & 0xFC)
        {
            if (HERR & 0x04)
            {
                dbg("Stuffing Error\n");
            }
            if (HERR & 0x08)
            {
                dbg("Toggle Error\n");
            }
            if (HERR & 0x10)
            {
                dbg("CRC Error\n");
            }
            if (HERR & 0x20)
            {
                dbg("Timeout Error\n");
            }
            if (HERR & 0x40)
            {
                dbg("Receive Error\n");
            }
            if (HERR & 0x80)
            {
                dbg("Lost SOF Error\n");
            }
        }
}
void dbg_token(uint8_t u8Token)
{
   dbg("<T>T=");
   if (u8Token & 0x80)
   {
      dbg("1; ");
   }
   else
   {
      dbg("0; ");
   }
   switch(u8Token & 0x70)
   {
      case HTOKEN_SETUP:
         dbg("SET; ");
         break;
      case HTOKEN_IN:
         dbg("I; ");
         break;
      case HTOKEN_OUT:
         dbg("O; ");
         break;
      case HTOKEN_SOF:
         dbg("SOF; ");
         break;
   }
   dbgval("EP: ",u8Token & 0x0F); 
}
#endif
#endif /* (USE_USB_HOST == 1) */
