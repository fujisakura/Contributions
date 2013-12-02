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
/** \file UsbFunction.c
 **
 ** - See README.TXT for project description
 ** - first release for a simple universal usb function library
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX)
 **   - 2010-04-15    1.1  MSc  Fixed bug in some cases a zero packages was sent twice
 **   - 2010-04-15    1.2  MSc  DMA Module, Timeout feature, EP0 received data handler
 **   - 2010-09-24    1.3  MSc  MB91665 Series support added
 **   - 2011-03-05    1.4  MSc  Support for FM3
 **   - 2011-03-30    1.5  MSc  Support for new HW description files
 **   - 2011-04-28    1.6  MSc  Support for MB91F662 template
 **   - 2011-05-30    1.7  MSc  Fixed Data Length Patch
 **   - 2012-02-03    1.8  MSc  Support for MB9BF51x and MB9BF31xK series
 *****************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/


#include "UsbFunction.h"

#if (USE_USB_FUNCTION == 1)
#if (CPU_TYPE == CPU_TYPE_32BIT)
   //#include "_fr.h" only for FR80 (MB91F662) controller
   //#include "mon.h" only for FR80 (MB91F662) controller
   #define USB_FR80
#endif

#include "base_type.h"
#include "UsbDescriptors.h"
#include "dbg.h"
#if (USB_USE_DMA == 1)
    #include "UsbDma.h"
#endif

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/


/*****************************************************************************/
/* function */
/*****************************************************************************/



/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/

// event handlers
uint8_t (* pu8HandlerClassEvent)(usb_event_t * stcEvent) = 0;         /// Class Event Handler (Class Requests, Status Change: Connect, Disconnect)
uint8_t (* pu8HandlerStatusEvent)(usb_event_t * stcEvent) = 0;        /// Event Handler Status Change: Connect, Disconnect
uint8_t (* pu8HandlerBeforeStatusChange)(usb_event_t * stcEvent) = 0; /// Event Handler Before Status Change: Connect, Disconnect
uint8_t (* u8DataReceivedHandler_EP0)(usb_event_t * stcEvent) = 0;    // Data received event on endpoint 0

void (* SOFHandler)(uint16_t u16FrameNumber) = 0;

fnuint8_t au8ReceiveBuffer[USB_FUNC_EP0_SIZE];
usb_buffer_t stcEndpointBuffer0 = {au8ReceiveBuffer,USB_FUNC_EP0_SIZE,0,0};
usb_buffer_t stcEndpointBuffer1 = {au8ReceiveBuffer,USB_FUNC_EP0_SIZE,0,0};
usb_buffer_t stcEndpointBuffer2 = {au8ReceiveBuffer,USB_FUNC_EP0_SIZE,0,0};
usb_buffer_t stcEndpointBuffer3 = {au8ReceiveBuffer,USB_FUNC_EP0_SIZE,0,0};
usb_buffer_t stcEndpointBuffer4 = {au8ReceiveBuffer,USB_FUNC_EP0_SIZE,0,0};
usb_buffer_t stcEndpointBuffer5 = {au8ReceiveBuffer,USB_FUNC_EP0_SIZE,0,0};

uint8_t au8TransmittBuffer[MAX_RECV_BUFFER]; // MAX_RECV_BUFFER by default 512
//static usb_buffer_t stcTransmitBuffer = {au8TransmittBuffer,MAX_RECV_BUFFER,0}; // MAX_RECV_BUFFER by default 512

uint32_t u32TransmissionLength0;
//static uint32_t u32MaxBuffer;
uint8_t flag_sendEP0 = 0;
//static uint8_t usb_configured = 1;

//static uint32_t laststate = 0;
uint8_t u8UsbStatus = 0;

usb_EP_t aEndpoint[6] = 
{
    {   // EP0 registers
        EP_STATUS_IDLE, // Endpoint 0 Status
        &EP0DTH,  // Endpoint 0 data register higher byte
        &EP0DTL,  // Endpoint 0 data register lower byte
        &EP0IS,   // Endpoint 0 status register
        &EP0C,    // Endpoint 0 control register
        USB_FUNC_EP0_SIZE, // Endpoint 0 size
        &stcEndpointBuffer0, // 0 Receive buffer information
        DummyEventHandler,  // 0 Event handler
        0
    },    
    {   // EP1 registers
        EP_STATUS_IDLE, // Endpoint 1 Status
        &EP1DTH,  // Endpoint 1 data register higher byte
        &EP1DTL,  // Endpoint 1 data register lower byte
        &EP1S,    // Endpoint 1 status register
        &EP1C,    // Endpoint 1 control register
        0,         // Endpoint 1 size
        &stcEndpointBuffer1, // 1 Receive buffer information
        DummyEventHandler,  // 1 Event handler
        1
    },
    {   // EP2 registers
        EP_STATUS_IDLE, // Endpoint 2 Status
        &EP2DTH,  // Endpoint 2 data register higher byte
        &EP2DTL,  // Endpoint 2 data register lower byte
        &EP2S,    // Endpoint 2 status register
        &EP2C,    // Endpoint 2 control register
        0,         // Endpoint 2 size
        &stcEndpointBuffer2, // 2 Receive buffer information
        DummyEventHandler,  // 2 Event handler
        2
    },
    {   // EP3 registers
        EP_STATUS_IDLE, // Endpoint 3 Status
        &EP3DTH,  // Endpoint 3 data register higher byte
        &EP3DTL,  // Endpoint 3 data register lower byte
        &EP3S,    // Endpoint 3 status register
        &EP3C,    // Endpoint 3 control register
        0,         // Endpoint 3 size
        &stcEndpointBuffer3, // 3 Receive buffer information
        DummyEventHandler,  // 3 Event handler
        3
    },
    #ifdef __CPU_DEFINE_MB91665_INC__
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0}
    #else
    {   // EP4 registers
        EP_STATUS_IDLE, // Endpoint 4 Status
        &EP4DTH,  // Endpoint 4 data register higher byte
        &EP4DTL,  // Endpoint 4 data register lower byte
        &EP4S,    // Endpoint 4 status register
        &EP4C,    // Endpoint 4 control register
        0,         // Endpoint 4 size
        &stcEndpointBuffer4, // 4 Receive buffer information
        DummyEventHandler,  // 4 Event handler
        4
    },
    {   // EP5 registers
        EP_STATUS_IDLE, // Endpoint 5 Status
        &EP5DTH,  // Endpoint 5 data register higher byte
        &EP5DTL,  // Endpoint 5 data register lower byte
        &EP5S,    // Endpoint 5 status register
        &EP5C,    // Endpoint 5 control register
        0,         // Endpoint 5 size
        &stcEndpointBuffer5, // 5 Receive buffer information
        DummyEventHandler,  // 5 Event handler
        5
    }
    #endif
};

stc_timeout_handler_t stcTimeOutHandler[10] = 
               { 
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0}
               };
/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/


/**
 ******************************************************************************
 ** SOF controlled Timeout Scheduler
 ** 
 ** \param ticks SOF counter
 ** 
 *****************************************************************************/

void TimeoutHandler(uint16_t ticks)
{
    uint8_t i;
    boolean_t bNothingToBeDone = TRUE;
    ticks = ticks;
    for(i = 0;i<10;i++)
    {
        if (stcTimeOutHandler[i].Handler != 0)
        {
            bNothingToBeDone = FALSE;
            if (stcTimeOutHandler[i].u16TimeOut == 0)
            {
                stcTimeOutHandler[i].Handler();
                stcTimeOutHandler[i].Handler = 0;
            }
            else
            {
                stcTimeOutHandler[i].u16TimeOut--;
            }
        }
    }
    if (bNothingToBeDone == TRUE)
    {
        UsbFunction_DisableSOF();
    }
}

/**
 ******************************************************************************
 ** Add a timeout handler
 **
 ** 
 ** \param Handler handler executed after timeout ( void handler(void) )
 ** 
 ** \param u16TimeOut timeout in ms
 **
 ** \return TRUE if command was successful
 **
 *****************************************************************************/
 
boolean_t UsbFunction_AddTimeOut(void (* Handler)(void), uint16_t u16TimeOut)
{
    uint8_t i;
    boolean_t bAdded = FALSE;
    for(i = 0;i<10;i++)
    {
        if (stcTimeOutHandler[i].Handler == 0)
        {
            stcTimeOutHandler[i].Handler = Handler;
            stcTimeOutHandler[i].u16TimeOut = u16TimeOut;
            bAdded = TRUE;
            break;
        }
    }
    UsbFunction_SetStartOfFrameHandler(TimeoutHandler);
    UsbFunction_EnableSOF();
    return bAdded;
}

/**
 ******************************************************************************
 ** Remove a timeout handler
 **
 ** 
 ** \param Handler handler executed after timeout ( void handler(void) )
 ** 
 ** \return none
 **
 *****************************************************************************/
 
void UsbFunction_RemoveTimeOut(void (* Handler)(void))
{
    uint8_t i;
    for(i = 0;i<10;i++)
    {
        if (stcTimeOutHandler[i].Handler == Handler)
        {
            stcTimeOutHandler[i].Handler = 0;
            stcTimeOutHandler[i].u16TimeOut = 0;
            break;
        }
    }
}

/**
 ******************************************************************************
 ** Get the current endpoint status
 **
 ** \param u8Endpoint Endpoint Number (0..5)
 **
 ** \return returns the endpoint
 *****************************************************************************/
usb_EP_t* UsbFunction_GetEndpoint(uint8_t u8EndpointNumber)
{
    return &(aEndpoint[u8EndpointNumber]);
}

/**
 ******************************************************************************
 ** Get the current USB status
 **
 ** \return returns the current USB status
 *****************************************************************************/
uint8_t UsbFunction_GetStatus(void) {
    return u8UsbStatus;
}

/**
 ******************************************************************************
 ** Get the current endpoint status
 **
 ** \param u8Endpoint Endpoint Number (0..5)
 **
 ** \return returns the current endpoint status
 *****************************************************************************/
uint8_t UsbFunction_GetEndpointStatus(uint8_t u8EndpointNumber) {
    return aEndpoint[u8EndpointNumber].status;
}

/**
 ******************************************************************************
 ** If all endpoints are idle, returns true
 **
 **
 ** \return If all endpoints are idle, returns true
 *****************************************************************************/
boolean_t UsbFunction_AllEndpointsIdle(void)
{
    uint8_t u8EndpointNumber;
    for(u8EndpointNumber=1;u8EndpointNumber<6;u8EndpointNumber++)
    {
        if ((aEndpoint[u8EndpointNumber].status & 0xF8) > 0)
        {
            dbg("\n\nNOT IDLE ENDPOINT");
            dbgval("EP No.:", u8EndpointNumber);
            dbgval("status:", aEndpoint[u8EndpointNumber].status);
            
            return FALSE;
        }
    }
    return TRUE;
}



/**
 ******************************************************************************
 ** Set event handler.
 **
 ** 
 ** \param u8Target target, can be: 
 ** #USB_EVENT_TARGET_VENDOR, 
 ** #USB_EVENT_TARGET_MAINAPP, 
 ** #USB_EVENT_TARGET_GENERAL
 **
 ** \param u8Event event, can be: 
 ** #USB_EVENT_RECEIVE, 
 ** #USB_EVENT_SENT, 
 ** #USB_EVENT_CONNECT, 
 ** #USB_EVENT_DISCONNECT
 **
 ** \param u8Options options
 ** \parm handler event handler
 **
 ** \return none
 **
 ** \b Example:
 ** \include UsbCustomClass_Init.c
 **
 *****************************************************************************/
void UsbFunction_SetEventHandler(uint8_t u8Target, uint8_t u8Event, uint8_t u8Options, uint8_t (* handler)(usb_event_t * stcEvent)) 
{
    switch (u8Target)
    {
        case USB_EVENT_TARGET_VENDOR:
            if (((u8Event & USB_EVENT_RECEIVE) != 0) || ((u8Event & USB_EVENT_SENT) != 0))
            {
                aEndpoint[u8Options].handler = handler;    // used for data transfers in USB Class
            }
            if ((u8Event & USB_EVENT_CLASSREQUEST) != 0)
            {
                pu8HandlerClassEvent = handler;    // used for USB status change in USB Class
            }
            break;
        case USB_EVENT_TARGET_MAINAPP: 
            if (((u8Event & USB_EVENT_CONNECT) != 0) || ((u8Event & USB_EVENT_DISCONNECT) != 0))
            {
                pu8HandlerStatusEvent = handler;    // used for USB status change event in main loop
            }
            break;
        case USB_EVENT_TARGET_GENERAL:
            if (((u8Event & USB_EVENT_CONNECT) != 0) || ((u8Event & USB_EVENT_DISCONNECT) != 0))
            {
                pu8HandlerBeforeStatusChange = handler;    // called before init or deinit USB Function
            }
            break;
        default:
            break;
    }
}

/**
 ******************************************************************************
 ** Enables Start of Frame Interrupt
 **
 **
 ** \return none
 **
 *****************************************************************************/
 
void UsbFunction_EnableSOF(void)
{
    SET_MASK(UDCIE,_UDCIE_SOFIE);       // SOF Flag Interrupt Enable   (HWM 29.3.6 rev16)
}

/**
 ******************************************************************************
 ** Disables Start of Frame Interrupt
 **
 **
 ** \return none
 **
 *****************************************************************************/
void UsbFunction_DisableSOF(void)
{
    CLEAR_MASK(UDCIE,_UDCIE_SOFIE);       // SOF Flag Interrupt Disable  (HWM 29.3.6 rev16)
}

/**
 ******************************************************************************
 ** Set a Start of Frame handler
 **
 ** \param handler Start Of Framehandler, type: handler(uint16_t u16FrameNumber);
 **
 ** \return none
 **
 *****************************************************************************/
void UsbFunction_SetStartOfFrameHandler(void(* handler)(uint16_t u16FrameNumber))
{
    SOFHandler = handler;
}

/**
 ******************************************************************************
 ** Clear a Start of Frame handler
 **
 **
 ** \return none
 **
 *****************************************************************************/
void UsbFunction_ClearStartOfFrameHandler(void)
{
    SOFHandler = 0;
}

/**
 ******************************************************************************
 ** Set a custom endpoint buffer
 **
 ** \param u8Endpoint Endpoint number
 ** \param pstcEndpointBuffer pointer to buffer struct
 **
 ** \return none
 **
 ** \b Example:
 ** \include UsbCustomClass_Init.c
 *****************************************************************************/
void UsbFunction_SetCustomEndpointBuffer(uint8_t u8Endpoint, usb_buffer_t * pstcEndpointBuffer)
{
    aEndpoint[u8Endpoint].pstcEndpointBuffer = pstcEndpointBuffer;
}


/**
 ******************************************************************************
 ** Parsing #au8ConfigDescriptor and writing settings of used endpoints.
 **
 ** \return none
 *****************************************************************************/
 
static void SetupEndpoints(void) 
{
    uint32_t i;
    uint8_t u8EndpointNumber;
    EP0C = USB_FUNC_EP0_SIZE;    // setting up EP0 fifo size
    aEndpoint[0].u16Size = USB_FUNC_EP0_SIZE; // setting up EP0 fifo size
    DisableEndpoints();
    for(i=0;i<sizeof(au8ConfigDescriptor);i++) // search the whole config descriptor
    {             
        if (au8ConfigDescriptor[i+1] == USBDESCR_ENDPOINT) // look for config descriptor type
        {
            u8EndpointNumber = au8ConfigDescriptor[i+2] & 0x0F; // get the endpoint number (1..5)
            
            EP_CONTROL(u8EndpointNumber) = au8ConfigDescriptor[i+4] | (au8ConfigDescriptor[i+5] << 8); // read/set package size
            aEndpoint[u8EndpointNumber].u16Size = au8ConfigDescriptor[i+4] | (au8ConfigDescriptor[i+5] << 8); // read/set package size
            
            aEndpoint[u8EndpointNumber].status |= EP_STATUS_CONFIGURED;  // set the endpoint number as configured
            
            // DEBUG print Endpoint number
            dbg("EP "); 
            dbghex(u8EndpointNumber); 
            dbg("\n  Dir: "); 
      
      
            if (au8ConfigDescriptor[i+2] & 0x80) // read/set communication direction
            {
                dbg("IN(PC)\n  Type: ");
                SET_MASK(EP_CONTROL(u8EndpointNumber),MASK_EPC_DIR);  // Direction IN-MCU / OUT-PC   
            } 
            #if (debug_enabled == 1)
            else 
            {
                dbg("OUT(PC)\n  Type: ");
            }
            #endif
      
            EP_CONTROL(u8EndpointNumber) |= (au8ConfigDescriptor[i+3] & 0x03) << 13; // read/set bulk / interrupt mode

            #if (debug_enabled == 1)                     //DEBUG
                switch(au8ConfigDescriptor[i+3] & 0x03)  //DEBUG
                {                                        //DEBUG
                    case 0x00:                           //DEBUG
                        dbg("Control\n");                //DEBUG
                        break;                           //DEBUG
                    case 0x01:                           //DEBUG
                        dbg("Isochronous\n");            //DEBUG
                        break;                           //DEBUG
                    case 0x02:                           //DEBUG
                        dbg("Bulk\n");                   //DEBUG
                        break;                           //DEBUG
                    case 0x03:                           //DEBUG
                        dbg("Interrupt\n");              //DEBUG
                        break;                           //DEBUG
                    default:                             //DEBUG
                        break;                           //DEBUG
                }                                        //DEBUG
            #endif                                       //DEBUG
            dbg("  Config: ");                           //DEBUG
            dbghex(EP_CONTROL(u8EndpointNumber));        //DEBUG
            dbg("\n");                                   //DEBUG
        }
        
        i += au8ConfigDescriptor[i] - 1;    // go to next descriptor
    }
}

/**
 ******************************************************************************
 ** Enable all configured endpoints.
 **
 ** \return none
 *****************************************************************************/
static void EnableEndpoints(void) 
{
    uint8_t i;
    for(i=1;i<6;i++)
    {
        if (aEndpoint[i].status != 0) // if endpoint was configured
        {
            dbg("    |->Endpoint ");  //DEBUG
            dbghex(i);                //DEBUG
            dbg(" enabled ");         //DEBUG
            
            aEndpoint[i].status |= EP_STATUS_ENABLED;  // set status to enabled
            SET_MASK(EP_CONTROL(i),MASK_EPC_EPEN);    // enable endpoint
            if (!IS_SET(EP_CONTROL(i),MASK_EPC_DIR))
            {
                SET_MASK(EP_STATUS(i),MASK_EPS_DRQIE); // enable interrupt flag if EP is IN direction
                dbg("; input interrupt enabled ");     //DEBUG
            }
            dbg("\n"); //DEBUG
        }
    }
    dbg("\n"); //DEBUG
}

/**
 ******************************************************************************
 ** Disable all configured endpoints.
 **
 ** \return none
 *****************************************************************************/
static void DisableEndpoints(void) 
{
    uint8_t i;
    for(i=1;i<6;i++)
    {
        if (aEndpoint[i].status != 0) // if endpoint was configured
        {
            dbg("    |->Endpoint ");  //DEBUG
            dbghex(i);                //DEBUG
            dbg(" disabled\n");       //DEBUG
            aEndpoint[i].status = EP_STATUS_DISABLED; // set status to disabled
            CLEAR_MASK(EP_CONTROL(i),MASK_EPC_EPEN);  // disable endpoint
            CLEAR_MASK(EP_STATUS(i),MASK_EPS_DRQIE);  // disable interrupt flag
        }
    }
    dbg("\n"); //DEBUG
}

/**
 ******************************************************************************
 ** called from interrupt after USB was connected. Provides events to main loop
 ** and USB class.
 **
 ** \return none
 *****************************************************************************/
static void ConnectEvent(void) {
    usb_event_t stcEvent;
    uint8_t i;
    
    CLEAR_USB_STATUS_FLAGS();  // Clear USB status
    
    dbg("\n\n### UsbFunction: Connect Event Detected ###\n\n"); //DEBUG
    
    SET_USB_STATUS_FLAG(USB_STATUS_CONNECTED); // Set USB status connected
    
    stcEvent.u8UsbStatus = u8UsbStatus;        // setup new event: USB status
    stcEvent.u8Event = USB_EVENT_CONNECT;      // setup new event: event type connect
    
    if (pu8HandlerBeforeStatusChange != 0) 
    {
        pu8HandlerBeforeStatusChange(&stcEvent); // connect event handler, which called before initialisation
    }

    SET_MASK(UDCC,_UDCC_RST);     // USB Reset  
    HCNT=0;                       // Select Device Mode   
    SET_MASK(HCNT,_HCNT_URST);    // Reset USB Host          
    CLEAR_MASK(UDCC,_UDCC_USTP);  // Start USB Clock
    CLEAR_MASK(UDCC,_UDCC_RESUM); // Clear Resume Flag
    CLEAR_MASK(UDCC,_UDCC_RFBK);  // Clear Data Toggle Flag
    if (au8ConfigDescriptor[7] == USBATTR_SELFPOWER) 
    {
        SET_MASK(UDCC,_UDCC_PWC);  // Setting selfpowered bit                (HWM 29.3.1 rev16)
    }
    else
    {
        CLEAR_MASK(UDCC,_UDCC_PWC);  // Clearing selfpowered bit               (HWM 29.3.1 rev16)
    }
    for(i=0;i<6;i++)
    {
        aEndpoint[i].pstcEndpointBuffer->u32Position = 0;
        aEndpoint[i].pstcEndpointBuffer->u32DataLength = 0;
    }
    SetupEndpoints(); // reading EP 1..5 setup information from au8ConfigDescriptor
    
    if(IS_SET(UDCC,_UDCC_RST))  // (HWM 29.3.1 rev16)
    {
        CLEAR_MASK(UDCC,_UDCC_RST); // Release Bus Reset, starting USB function  (HWM 29.3.1 rev16)
        __wait_nop();
        __wait_nop();
        __wait_nop();
    }
    
    CLEAR_MASK(EP1S,_EP1S_BFINI);  // Buffer EP1 Clear      (HWM 29.3.9 rev16)
    CLEAR_MASK(EP2S,_EP2S_BFINI);  // Buffer EP2 Clear      (HWM 29.3.9 rev16)
    CLEAR_MASK(EP3S,_EP3S_BFINI);  // Buffer EP3 Clear      (HWM 29.3.9 rev16)
    
    #ifndef __CPU_DEFINE_MB91665_INC__    
        CLEAR_MASK(EP4S,_EP4S_BFINI);;  // Buffer EP4 Clear      (HWM 29.3.9 rev16)
        CLEAR_MASK(EP5S,_EP5S_BFINI);   // Buffer EP5 Clear      (HWM 29.3.9 rev16)
    #endif
    
    CLEAR_MASK(EP0OS,_EP0OS_BFINI); // Buffer EP0 Out Clear  (HWM 29.3.8 rev16)
    CLEAR_MASK(EP0IS,_EP0IS_BFINI); // Buffer EP0 In Clear   (HWM 29.3.7 rev16)
    
    CLEAR_MASK(UDCC,_UDCC_HCONX);   // Enable Pull-Up Resistor on USB  (HWM 29.3.1 rev16)
    
    DEVICEHCONX_CLEAR;

    
    UDCIE = 0x08;        // Status-INTR Enable              (HWM 29.3.6 rev16)  
    CLEAR_MASK(EP0IS, _EP0IS_DRQIIE);    // EP0IN-INTR Enable               (HWM 29.3.7 rev16)
    SET_MASK(EP0OS, _EP0OS_DRQOIE);      // EP0OUT-INTR Disable             (HWM 29.3.8 rev16)
    
    SET_MASK(UDCIE,_UDCIE_CONFIE);    // Configure Flag Interrupt Enable (HWM 29.3.6 rev16)
    SET_MASK(UDCIE,_UDCIE_SUSPIE);    // Suspend Flag Interrupt Enable   (HWM 29.3.6 rev16)
    SET_MASK(UDCIE,_UDCIE_WKUPIE);    // Wakeup Flag Interrupt Enable    (HWM 29.3.6 rev16)
    
    
    if (pu8HandlerClassEvent != 0) 
    {
        pu8HandlerClassEvent(&stcEvent);    // Event Handler Vendor Class
    }
    
    if (pu8HandlerStatusEvent != 0) 
    {
        pu8HandlerStatusEvent(&stcEvent);  // Event Handler Main Application
    }
}

/**
 ******************************************************************************
 ** called from interrupt after USB was disconnected. Provides events to 
 ** main loop and USB class.
 **
 ** \return none
 *****************************************************************************/
static void DisconnectEvent(void) {
    usb_event_t stcEvent;

    CLEAR_USB_STATUS_FLAGS();
    dbg("\n\n### UsbFunction: Disconnect Event Detected ###\n\n"); //DEBUG
    
    stcEvent.u8UsbStatus = u8UsbStatus;        // setup new event: USB status
    stcEvent.u8Event = USB_EVENT_DISCONNECT;   // setup new event: event type disconnect
    
    if (pu8HandlerBeforeStatusChange != 0) 
    {
        pu8HandlerBeforeStatusChange(&stcEvent);  // disconnect event handler, which called before disable USB function   
    }
    
    DisableEndpoints();  // Disable all active endpoints
    
    SET_MASK(UDCC,_UDCC_HCONX);  // Disable Pull-Up Resistor on USB  (HWM 29.3.1 rev16)
    
    DEVICEHCONX_SET;

    UDCIE = 0x00;        // Status Interrupts Disable (HWM 29.3.6 rev16)
    CLEAR_MASK(EP0IS, _EP0IS_DRQIIE);    // EP0IN-INTR Disable        (HWM 29.3.7 rev16) 
    CLEAR_MASK(EP0OS, _EP0OS_DRQOIE);    // EP0OUT-INTR Disable       (HWM 29.3.8 rev16)
    
    SET_MASK(UDCC,_UDCC_RST); // Bus Reset, halt USB function       (HWM 29.3.1 rev16)
    
    if (pu8HandlerClassEvent != 0) 
    {
        pu8HandlerClassEvent(&stcEvent);    // Event Handler Vendor Class
    }
    
    if (pu8HandlerStatusEvent != 0) 
    {
        pu8HandlerStatusEvent(&stcEvent);  // Event Handler Main Application
    }
}

/**
 ******************************************************************************
 ** initalizes USB function 
 **
 ** \return none
 *****************************************************************************/
void UsbFunction_Initialize(boolean_t bAutoConnect)
{

    uint16_t cnt;
    
    #ifdef USBEN_USB_EN
        USBEN_USB_EN = 1; // Enable USB (FR80 only)
    #endif
    #ifdef USBEN_USBEN
        USBEN_USBEN = 1;  // Enable USB (FR80 only)
    #endif
    DEVICEHCONX_SET;
    DEVICEHCONX_INIT;
    DEVICEVBUS_DISABLEISR;
    DEVICEVBUS_INIT;
    

    for(cnt=0; cnt<(USB_FUNC_EP0_SIZE); cnt++) // Init Global Receive Buffer
    {    
        au8ReceiveBuffer[cnt] = 0xFF;
    }

    if (bAutoConnect == TRUE)
    {
        DEVICEVBUS_DISABLEISR;
        DEVICEVBUS_CLEARISRFLAG;
        if (DEVICEVBUS_HIGHDETECT)   
        {        
            DEVICEVBUS_SETLOWDETECT;           // interrupt while low level  
            ConnectEvent(); 
        } 
        else
        {
            DEVICEVBUS_SETHIGHDETECT;           // interrupt while high level 
            DisconnectEvent(); 
        }   
        DEVICEVBUS_ENABLEISR;      // interrupt enable                                       
    }   
}

/**
 ******************************************************************************
 ** Connect USB function (only used if auto connect is disabled) 
 **
 ** \param bAutoConnect can be used to auto connect/disconnect (standard)
 **
 ** \return returns TRUE if connect was successful started
 *****************************************************************************/
boolean_t UsbFunction_Connect(boolean_t bAutoConnect)
{
    if (bAutoConnect == TRUE)
    {
        UsbFunction_Initialize(TRUE);
        return TRUE;
    }
    if (DEVICEVBUS_HIGHDETECT)
    {
        ConnectEvent();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 ******************************************************************************
 ** Disconnect USB function 
 **
 ** \param bForceDisconnect can be used to force disconnect
 **
 ** \return returns TRUE if connect was successful started
 *****************************************************************************/
boolean_t UsbFunction_Disconnect(boolean_t bForceDisconnect)
{
    uint32_t u32Timeout = 1000000;
    if (bForceDisconnect == TRUE)
    {
        DisconnectEvent();
        DEVICEVBUS_DISABLEISR;
        return TRUE;
    }
    while((u32Timeout > 0) && (UsbFunction_AllEndpointsIdle() == FALSE))
    {
        u32Timeout--;
    }
    if (u32Timeout == 0)
    {
        return FALSE;
    }
    DisconnectEvent();
    DEVICEVBUS_DISABLEISR;
    return TRUE;
}

/**
 ******************************************************************************
 ** Procedure to send buffer with Endpoint 0..5
 **
 ** \param u8EndpointNumber Endpoint number (0..5) 
 ** \param buffer pointer of buffer to send
 ** \param buffersize size of buffer to send
 **
 ** \return none
 *****************************************************************************/
void UsbFunction_SendData(uint8_t u8EndpointNumber,puint8_t buffer, uint32_t buffersize, uint8_t u8PollingMode) 
{
    volatile uint8_t * pu8EndpointDataRegHigh;
    volatile uint8_t * pu8EndpointDataRegLow;
    volatile uint16_t * pu16EndpointStatusReg;
    uint32_t i = 0;
    uint16_t u16EndpointSize = aEndpoint[u8EndpointNumber].u16Size;
    volatile uint16_t u16Size = 0;
    dbg("\n\nTX> UsbFunction_SendData()\n");
    aEndpoint[u8EndpointNumber].status |= EP_STATUS_SENDING;
    if (aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position == 0) 
    {
        aEndpoint[u8EndpointNumber].pstcEndpointBuffer->pu8Buffer = buffer;
        aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32DataLength = buffersize;
        
        dbg("TX> Beginning new package");                                      // DEBUG
        dbg("\nTX   |->Endpoint: ");                                           // DEBUG
        dbghex(u8EndpointNumber);                                              // DEBUG
        dbg("\nTX   |->Endpoint Size: ");                                      // DEBUG
        dbghex(u16EndpointSize);                                               // DEBUG
        dbg("\nTX   |->Size: ");                                               // DEBUG
        dbghex(aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32DataLength); // DEBUG
        dbg("\nTX   |->Start At: ");                                           // DEBUG
        dbghex(aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position);   // DEBUG
        dbg("\n");                                                             // DEBUG
    }
    else
    {
        dbg("TX> Resuming Transfer: ");                                        // DEBUG
        dbg("\nTX   |->Endpoint: ");                                           // DEBUG
        dbghex(u8EndpointNumber);                                              // DEBUG
        dbg("\nTX   |->Start At: ");                                           // DEBUG
        dbghex(aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position);   // DEBUG
        dbg("\n");                                                             // DEBUG
    }
    #if (debug_enabled == 1)
        dbg("TX   |->TYPE: ");
        switch (u8PollingMode)
        {
            case USB_SENDING_MODE_INTERRUPT:
                dbg("INTERRUPT\n");
                break;
            case USB_SENDING_MODE_DMA:
                dbg("DMA\n");
                break;
            case USB_SENDING_MODE_POLLED:
                dbg("POLLED\n");
                break;
            default:
                dbg("unknown\n");
                break;
        }
    #endif

    #if (USB_USE_DMA == 1)
        if (u8PollingMode == USB_SENDING_MODE_DMA)
        {
            UsbDma_SendReceiveBufferViaDma(u8EndpointNumber);
            return;
        }
    #endif
    
    pu8EndpointDataRegHigh = aEndpoint[u8EndpointNumber].EPnDTH; // EP higher byte data register
    pu8EndpointDataRegLow = aEndpoint[u8EndpointNumber].EPnDTL;  // EP lower byte data register
    pu16EndpointStatusReg = aEndpoint[u8EndpointNumber].EPnS;    // EP status register
    
    if (u8EndpointNumber == 0) 
    {
        SET_MASK(EP0IS, _EP0IS_DRQIIE); // see hw manual: spec. for use with EP0
    }
    while(pu16EndpointStatusReg_DRQ == 0)
    {
        __wait_nop(); // wait for buffer is empty (HWM 29.3.9 rev16 / HWM 29.4.3 rev16)
    }

    for(i = aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position; i<aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32DataLength; i++)
    {
        aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position = i;
        if((i & 0x01)==1)
        {
            *pu8EndpointDataRegHigh = buffer[i]; // Data Register Higher (HWM 29.3.10 rev16)
        } 
        else
        {
            *pu8EndpointDataRegLow = buffer[i];  // Data Register Lower (HWM 29.3.10 rev16)
        }
        
        #if (use_printbuffer == 1)
            dbghex(buffer[i]);  // DEBUG
            dbg(" ");           // DEBUG
        #endif
        
        if ((i + 1) % u16EndpointSize == 0) // every filled buffer a token has to be sent
        {    
            #if (use_printbuffer == 1)
                dbg("T ");      // DEBUG
            #endif
            
            if (u8PollingMode == USB_SENDING_MODE_INTERRUPT)
            {
               dbg("TX   |->In-Loop - Break: ");
               dbghex(aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position);
               dbg("\n");
               aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position += 1;
               SET_pu16EndpointStatusReg_DRQ(0); // set buffer ready to send
               SET_MASK(*pu16EndpointStatusReg,MASK_EPS_DRQIE);
               return;
            }   
            SET_pu16EndpointStatusReg_DRQ(0);    // set buffer ready to send
            while(pu16EndpointStatusReg_DRQ == 0) 
            {
           	    __wait_nop(); // wait until buffer is empty / was sent
            }
        } 
    }
    
    //-patch V1.1 || (i == 0)
    if (((i % u16EndpointSize != 0) )) // buffer empty or not?
    {  
        #if (use_printbuffer == 1)
            dbg("T \n");  // DEBUG
        #endif
       
        if (u8PollingMode == USB_SENDING_MODE_INTERRUPT)
        {
            dbg("TX   |->After-Loop SPK Break: ");
            dbghex(aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position);
            dbg("\n");
            aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position += 1;
            SET_pu16EndpointStatusReg_DRQ(0); // set buffer ready to send
            SET_MASK(*pu16EndpointStatusReg,MASK_EPS_DRQIE);
            return;
        } 
        SET_pu16EndpointStatusReg_DRQ(0); // set buffer ready to send
        while(pu16EndpointStatusReg_DRQ == 0) __wait_nop(); // wait until buffer is empty / was sent
        
    } 
    //+patch V1.2 || (i == 0)
    if (((buffersize % u16EndpointSize) == 0) || (i == 0) || (buffersize == 0) || (aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position > aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32DataLength))
    {

        dbg("TX   |->Sending null terminated end\n");
        if (u8PollingMode == USB_SENDING_MODE_INTERRUPT)
        {
            dbg("TX   |->After-Loop zerro package Break: ");
            dbghex(aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position);
            dbg("\n");
            aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position += 1;
            SET_pu16EndpointStatusReg_DRQ(0); // set buffer ready to send
            SET_MASK(*pu16EndpointStatusReg,MASK_EPS_DRQIE);
            return;
        } 
        SET_pu16EndpointStatusReg_DRQ(0); // set buffer ready to send
        while(pu16EndpointStatusReg_DRQ == 0) __wait_nop(); // wait until buffer is empty / was sent
    }
    
    
    #if (use_printbuffer == 1)
        dbg("\n");  // DEBUG
    #endif
    
    if (u8EndpointNumber == 0) 
    {
        CLEAR_MASK(EP0IS, _EP0IS_DRQIIE); // see hw manual: spec. for use with EP0
    }
    aEndpoint[u8EndpointNumber].status &= ~EP_STATUS_SENDING;
    aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position = 0;
}

/**
 ******************************************************************************
 ** only for Endpoint 0, used in data stage to send replies for requests
 **
 ** \param buffer pointer of buffer to send
 ** \param buffersize size of buffer to send
 **
 ** \return none
 *****************************************************************************/
void UsbFunction_SetDataToSendEndpoint0(uint8_t *buffer, uint32_t size) 
{
    uint32_t i;
    flag_sendEP0 = 1;
    for(i=0;i<size;i++) 
    {
        au8TransmittBuffer[i] = *buffer++;
    }
    u32TransmissionLength0 = size;

}

/**
 ******************************************************************************
 ** Routine to stall endpoint 0
 **
 ** \return none
 *****************************************************************************/
void UsbFunction_Stall(uint8_t u8EndpointNumber) 
{
    SET_MASK(*aEndpoint[u8EndpointNumber].EPnC,MASK_EPC_STAL);
    dbgval("Stall EP: ", u8EndpointNumber);
}

/**
 ******************************************************************************
 ** send wakeup to computer
 **
 ** \param bEnableDisable TRUE to enable, FALSE to disable
 **
 ** \return none
 *****************************************************************************/
void UsbFunction_EnableWakeup(boolean_t bEnableDisable)
{
	if (bEnableDisable)	
	{
		SET_MASK(UDCC,_UDCC_RESUM);
	} 
	else 
	{
		CLEAR_MASK(UDCC,_UDCC_RESUM);
	}
}
/**
 ******************************************************************************
 ** Routine to unstall endpoint 0
 **
 ** \return none
 *****************************************************************************/
void UsbFunction_StallRemove(uint8_t u8EndpointNumber) {
    if (u8EndpointNumber == 0)
    {
        if (IS_SET(EP0C,_EP0C_STAL))
        {
            CLEAR_MASK(EP0C,_EP0C_STAL);
        
            if (!IS_SET(EP0OS, _EP0OS_BFINI)) 
            {
                SET_MASK(EP0OS, _EP0OS_BFINI);
                CLEAR_MASK(EP0OS, _EP0OS_BFINI);
            }
        
            if (!IS_SET(EP0IS, _EP0IS_BFINI)) 
            {
                SET_MASK(EP0IS, _EP0IS_BFINI);
                CLEAR_MASK(EP0IS, _EP0IS_BFINI);
            }
        }
    }
    CLEAR_MASK(*aEndpoint[u8EndpointNumber].EPnC,MASK_EPC_STAL);
    dbgval("Unstall EP: ", u8EndpointNumber);
}

/**
 ******************************************************************************
 ** Routine to receive an USB setup package
 **
 ** \return none
 *****************************************************************************/
static void UsbReceiveSetup(void)
{
    uint8_t setup_handled = 0;
    uint16_t i;
    uint32_t u32ReceiveSize;
    /* USB_func_FIFO to memory */
    
     u32ReceiveSize = (EP0OS & _EP0OS_SIZE);    /* get length */
     for(i=0; i<u32ReceiveSize; i++){
        if((i & 0x01) == 1){
            au8ReceiveBuffer[i] = EP0DTH;
        }
        else{
            au8ReceiveBuffer[i] = EP0DTL;
        }
      }

    setup_handled = UsbRequestDecode(au8ReceiveBuffer,u32ReceiveSize);

    CLEAR_MASK(EP0OS,_EP0OS_DRQO);    /* DRQo <- 0 */
    #ifdef USB_FR80
      CLEAR_MASK(EP0OS,_EP0OS_BFINI);
    #endif
    CLEAR_MASK(UDCS,_UDCS_SETP);    /* SETP <- 0 */
    
    
    
    

    /* DeviceRequest Decode & Responce */
    if ((au8ReceiveBuffer[0] & 0x60) == USB_DEVREQ_CLSTYPE)
    {
        if(au8ReceiveBuffer[1] == USB_DEVREQ_SET_IDLE)
        {
            /*** recive SET_IDLE ***/
            CLEAR_MASK(EP0IS,_EP0IS_DRQI);    /* DRQi <- 0 */
        }
    }
    if (flag_sendEP0) {
      sendEP0((puint8_t)au8TransmittBuffer,u32TransmissionLength0);
      
      flag_sendEP0 = 0;
    }
    if (setup_handled == 1) {
      
    } else {
      UsbFunction_Stall(USB_EP0);
    }
}

/**
 ******************************************************************************
 ** used to reply a string descriptor request 
 **
 ** \param ptRequest pointer of USB request 
 **
 ** \return none
 *****************************************************************************/
static void SendStringDescriptor(usb_request_t * ptRequest)
{
    uint8_t u8DataToSend[255];
    uint8_t u8Size = 0;
    switch ((ptRequest->wValue) & 0xFF)
    {
        case 0: 
            // string index 0: sending language information
            u8DataToSend[0] = 0x04;
            u8DataToSend[1] = 0x03;
            u8DataToSend[2] = 0x09;
            u8DataToSend[3] = 0x04;
            break;
        case 1:
            // string index 1: sending pcManufacturerStringDescriptor
            u8DataToSend[0] = 2;
            u8DataToSend[1] = 0x03;
            for(u8Size=0;(pcManufacturerStringDescriptor[u8Size] != 0);u8Size++)
            {
                u8DataToSend[2 + u8Size * 2] = pcManufacturerStringDescriptor[u8Size];
                u8DataToSend[3 + u8Size * 2] = 0;
            }
            u8DataToSend[0] = u8DataToSend[0] + 2 * u8Size;
            break;
        case 2:
            // string index 2: sending pcProductStringDescriptor
            u8DataToSend[0] = 2;
            u8DataToSend[1] = 0x03;
            for(u8Size=0;(pcProductStringDescriptor[u8Size] != 0);u8Size++)
            {
                u8DataToSend[2 + u8Size * 2] = pcProductStringDescriptor[u8Size];
                u8DataToSend[3 + u8Size * 2] = 0;
            }
            u8DataToSend[0] = u8DataToSend[0] + 2 * u8Size;
            break;
        case 3:
            // string index 0: sending pcSerialnumberStringDescriptor
            u8DataToSend[0] = 2;
            u8DataToSend[1] = 0x03;
            for(u8Size=0;(pcSerialnumberStringDescriptor[u8Size] != 0);u8Size++)
            {
                u8DataToSend[2 + u8Size * 2] = pcSerialnumberStringDescriptor[u8Size];
                u8DataToSend[3 + u8Size * 2] = 0;
            }
            u8DataToSend[0] = u8DataToSend[0] + 2 * u8Size;
            break;
        default:
            u8DataToSend[0] = 0;
            break;
    }
    UsbFunction_SetDataToSendEndpoint0(u8DataToSend,u8DataToSend[0]);
}

/**
 ******************************************************************************
 ** decode of a USB request 
 **
 ** \param ptRequest pointer of USB request 
 **
 ** \return 1: Request was handled / 0: Request was not handled
 *****************************************************************************/
static uint8_t UsbGetDescriptor(usb_request_t * ptRequest)
{
    uint16_t rsp_size = 0;
    
    switch (((ptRequest->wValue) >> 8) & 0x00FF) 
    {
        case USB_REQUEST_DEVICE_DESCRIPTOR:
            SET_USB_STATUS_FLAG(USB_STATUS_DEVDESC);
            if(ptRequest->wLength > USB_DEVDESC_SIZE){
                rsp_size = USB_DEVDESC_SIZE;
            }
            else{
                rsp_size = ptRequest->wLength;
            }
            if (!rsp_size) {
                dbg("###### WARNING: WRONG LENGTH\n #####");  // DEBUG
            }
            if((ptRequest->Type == 0) && (IS_SET(EP0IS, _EP0IS_DRQI)) && (rsp_size))
            {
                UsbFunction_SetDataToSendEndpoint0((uint8_t*)au8DeviceDescriptor,rsp_size);
            }
            return 1;
        case USB_REQUEST_CONFIG_DESCRIPTOR:
            SET_USB_STATUS_FLAG(USB_STATUS_CONFDESC);
            if (ptRequest->wLength > USB_CNFGDESC_SIZE){
                rsp_size = USB_CNFGDESC_SIZE;
            }
            else
            {
                rsp_size = ptRequest->wLength;
            }
            if (!rsp_size) 
            {
                dbg("WARNING: WRONG LENGTH\n");  // DEBUG
            }
            if((ptRequest->Type == 0) && (IS_SET(EP0IS, _EP0IS_DRQI)) && (rsp_size))
            {
                UsbFunction_SetDataToSendEndpoint0((uint8_t*)au8ConfigDescriptor,rsp_size);
            }
            return 1;
        case USB_REQUEST_STRING_DESCRIPTOR:
            SendStringDescriptor(ptRequest);
            return 1;
        case USB_REQUEST_REPORT_DESCRIPTOR:
            if((ptRequest->Type == 0) && (IS_SET(EP0IS,_EP0IS_DRQI)))
            {
                UsbFunction_SetDataToSendEndpoint0((uint8_t*)au8ReportDescriptor,USB_RPTDESC_SIZE);
            }
            return 1;
            
        default:
            UsbFunction_Stall(USB_EP0);
            break;
    }
    return 0;
}

/**
 ******************************************************************************
 ** decode of a setup request at endpoint 0 
 **
 ** \param buffer pointer of received buffer
 ** \param size size of buffer 
 **
 ** \return 1: Request was handled / 0: Request was not handled
 *****************************************************************************/
static uint8_t UsbRequestDecode(puint8_t buffer,uint32_t size) 
{
    usb_event_t stcEvent;
    usb_request_t stcRequest;
    size=size;
    
    stcEvent.Request = &stcRequest; // Initialize request in stcEvent
    
    dbg("\n");                      // DEBUG
    printBuffer(buffer,size);       // DEBUG
    dbg("\n>USB-Setup:\n");         // DEBUG

    // fill request with information
    stcEvent.Request->Direction = buffer[0] & 0x80;
    stcEvent.Request->Type = (buffer[0] & 0x60) >> 5;
    stcEvent.Request->Target = buffer[0] & 0x1F;
    stcEvent.Request->Request = buffer[1];
    stcEvent.Request->wValue = (uint16_t)(buffer[3] << 8) | ((uint16_t)(buffer[2] & 0x00FF));
    stcEvent.Request->wIndex = (uint16_t)buffer[4] | ((uint16_t)buffer[5] >> 8);
    stcEvent.Request->wLength = (uint16_t)((uint8_t)buffer[6]) | (uint16_t)((uint8_t)buffer[7] << 8);
    
    #if (debug_enabled == 1)
        DbgUsbRequest(stcEvent.Request);  // DEBUG
    #endif
    
    if (stcEvent.Request->Type == USB_REQUEST_TYPE_CLASS) // request type is a class request
    {
        stcEvent.u8Event = USB_EVENT_CLASSREQUEST;         // event type is a class request
        if (pu8HandlerClassEvent != 0)                    // handler was set?
        {
            if (pu8HandlerClassEvent(&stcEvent))          // class events will be handled in the USB class module
            {
                return 1;                                 // successful handled
            }
        }
    }
    switch (stcEvent.Request->Request)                    // try to handle standart request
    {
        /* NOT SUPPORTED BY THE 16FX / FR80 HARDWARE ABSTRACTION LAYER
        case USB_GET_STATUS:
            UsbFunction_SetDataToSendEndpoint0((uint8_t*)"\0\0",2);
            return 1;
        case USB_CLEAR_FEATURE:
            break;  
        case USB_SET_FEATURE:
            break;  
        case USB_SET_ADDRESS:
            break;  
        case USB_GET_CONFIGURATION:
            break;   
        case USB_SET_CONFIGURATION:
            break; 
        case USB_SET_INTERFACE:
            break; 
        case USB_GET_INTERFACE:
            break; 
        */


        case USB_GET_DESCRIPTOR:
            #if (debug_enabled == 1)
              DbgUsbGetDescriptor(stcEvent.Request);  // DEBUG
            #endif
            return UsbGetDescriptor(stcEvent.Request); 
        case USB_SET_DESCRIPTOR:
            break; // Not implemented
        case USB_SYNCH_FRAME:
            break; 
        default:
            break;
    }
    return 0;

    

}


/**
 ******************************************************************************
 ** receive data on endpoint 0 
 **
 ** \return none
 *****************************************************************************/
static void UsbReceiveDataEndpoint0(void)
{
    usb_event_t stcEvent;
    uint16_t i;
    uint32_t u32ReceiveSize;
    
    #if (use_printbuffer == 1)
        dbg("### USB DATA RECEIVE ###\n  length:");  // DEBUG
    #endif
    
    u32ReceiveSize = (EP0OS & _EP0OS_SIZE); //get length of received data of endpoint 0 (HWM 29.3.8 rev16)
    
    #if (use_printbuffer == 1)
        dbghex(u32ReceiveSize);   // DEBUG
        dbg("\n");                // DEBUG
    #endif
    
    if (u32ReceiveSize == 0)           // Receive a zerro message
    {       
        au8ReceiveBuffer[0] = EP0DTL; // Dummy read
        au8ReceiveBuffer[0] = EP0DTH; // Dummy read
    } 
    else 
    {      
        for(i=0; i<u32ReceiveSize; i++)
        {
            if((i & 0x01) == 1){
                au8ReceiveBuffer[i] = EP0DTH; // Receiving odd address
            }
            else
            {
                au8ReceiveBuffer[i] = EP0DTL; // Receiving even address 
            }
        }
    }
    
    CLEAR_MASK(EP0OS,_EP0OS_DRQO);    // set DRQ0 to 0 to signalize ready to receive next package (HWM 29.3.8 rev16)
    
    #ifdef USB_FR80
      CLEAR_MASK(EP0OS, _EP0OS_BFINI); // with FR80, also the buffer has to be cleared
    #endif
    #if (use_printbuffer == 1)
        if (u32ReceiveSize) 
        {
            printBuffer(au8ReceiveBuffer,(uint32_t)u32ReceiveSize);  // DEBUG
        }
    #endif
    
    if (u8DataReceivedHandler_EP0 != 0)
    {
        stcEvent.u32DataSize = u32ReceiveSize;
        stcEvent.pstcEndpointBuffer = &stcEndpointBuffer0;
        stcEvent.u8Event = USB_EVENT_RECEIVE;
        u8DataReceivedHandler_EP0(&stcEvent);
        u8DataReceivedHandler_EP0 = 0;
    }
    else
    {
        if (u32ReceiveSize > 0)
        {
            dbg("################################\n");
            dbg("WARN: EP0 reception not handled!\n");
            dbg("################################\n");
        }
    }
}


/**
 ******************************************************************************
 ** Procedure to receive buffer with endpoint 1..5  
 **
 ** \param u8EndpointNumber endpoint number 1..5  
 ** \param buffer pointer to buffer in which the received data is stored
 **
 ** \return Size of received data
 *****************************************************************************/
static uint32_t UsbReceiveData(uint8_t u8EndpointNumber, puint8_t buffer) {
    uint32_t u32ReceiveSize;
    uint32_t u32Offset;
    volatile uint8_t * pu8EndpointDataRegHigh;
    volatile uint8_t * pu8EndpointDataRegLow;
    volatile uint16_t * pu16EndpointStatusReg;
    uint32_t i = 0;
    aEndpoint[u8EndpointNumber].status |= EP_STATUS_RECEIVING;
    u32ReceiveSize = (*aEndpoint[u8EndpointNumber].EPnS) & 0x01FF; // getting received size   
    u32Offset = aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position;
      
    pu8EndpointDataRegHigh = aEndpoint[u8EndpointNumber].EPnDTH;   // Endpoint higher byte data register
    pu8EndpointDataRegLow = aEndpoint[u8EndpointNumber].EPnDTL;    // Endpoint lower byte data register
    pu16EndpointStatusReg = aEndpoint[u8EndpointNumber].EPnS;      // Endpoint status register
    
    
    dbg("RX> Receiving Data\n");  // DEBUG
    dbg("\nRX   |->Endpoint: ");  // DEBUG
    dbghex(u8EndpointNumber);     // DEBUG
    dbg("\nRX   |->Length: ");    // DEBUG
    dbghex(u32ReceiveSize);       // DEBUG
    dbg("\n");                    // DEBUG
    
    if (u32ReceiveSize == 0)                    // Receiving a zerro message
    {
        buffer[0] = *pu8EndpointDataRegLow;     // Dummy read
        buffer[0] = *pu8EndpointDataRegHigh;    // Dummy read
    } 
    else 
    {      
        for(i=u32Offset; i<u32ReceiveSize+u32Offset; i++)
        {
            if(((i-u32Offset) & 0x01) == 1){
                buffer[i] = *pu8EndpointDataRegHigh; // Receiving odd address
            }
            else
            {
                buffer[i] = *pu8EndpointDataRegLow;  // Receiving even address 
            }
        }
        dbg("Status-Reg: ");
        dbghex(*pu16EndpointStatusReg);
        dbg("\n\n");
        
        if (!IS_SET(*pu16EndpointStatusReg,MASK_EPS_SPK))
        {
            dbg("Full Package Offset ");
            dbghex(i);
            dbg("\n\n");
            aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position = i;
            
        }
        else
        {
            aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position = 0;
            CLEAR_MASK(*pu16EndpointStatusReg,MASK_EPS_SPK);
        }
    }
    buffer[i] = 0;
    
    SET_pu16EndpointStatusReg_DRQ(0);      // if not SPK, reset DRQ
    
    /*
    if ((aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position > 0) && ((IS_SET(*pu16EndpointStatusReg,MASK_EPS_DRQ)) || (IS_SET(*pu16EndpointStatusReg,MASK_EPS_BUSY))))
    {
        // waiting for next package
    } 
    else
    {
        aEndpoint[u8EndpointNumber].pstcEndpointBuffer->u32Position = 0;
    }
    */

    
    #ifdef USB_FR80
      SET_pu16EndpointStatusReg_BFINI(0);  // with FR80, clear buffer
    #endif
    
    return i;
}






/**
 ******************************************************************************
 ** Event Routine (USB-Function Endpoint1-5 Event) Dummy Handler   
 **
 ** will be called from interrupt, if no handler was set. Sould normaly
 ** not happen!! 
 **
 ** \param pstcEvent event variable
 ** \return 1
 *****************************************************************************/
static uint8_t DummyEventHandler(usb_event_t * pstcEvent) {
    pstcEvent = pstcEvent;
    #if (debug_enabled == 1)
    dbg("Not handled Event:");                // DEBUG
    switch ((*pstcEvent).u8Event)
    {
        case USB_EVENT_CONNECT:
            dbg("USB_EVENT_CONNECT\n");       // DEBUG
            break;
        case USB_EVENT_DISCONNECT:
            dbg("USB_EVENT_DISCONNECT\n");    // DEBUG
            break;
        case USB_EVENT_RECEIVE:
            dbg("USB_EVENT_RECEIVE:");        // DEBUG
            dbg(" Endpoint ");                 // DEBUG
            dbghex((*pstcEvent).u8Endpoint);  // DEBUG
            CLEAR_MASK(EP_STATUS((*pstcEvent).u8Endpoint),MASK_EPS_DRQO);
            puts("\n");                       // DEBUG
            break;
        case USB_EVENT_SENT:
            dbg("USB_EVENT_SENT\n");          // DEBUG
            break;
        case USB_EVENT_CLASSREQUEST:
            dbg("USB_EVENT_CASSREQUEST\n");   // DEBUG
            break;
        default:
            dbg("Not known Event...\n");      // DEBUG
            break;
    }
    #endif
    __wait_nop();
    return 1;
}

// \name Interrupt Service Routines 
// @{

/**
 ******************************************************************************
 ** Interrupt Routine (USB-Function Endpoint1-5 Interrupt)  
 **
 ** \b Example Vectors.c
 ** \include Example_Vectors.c
 *****************************************************************************/
#if IS_USBMCU_FM3  
    #ifdef USE_USB0
        void USB0F_Handler (void)
    #else
        void USBF_Handler (void)
    #endif
#else
    __interrupt void UsbFunction_EndpointIsr(void)
#endif
{
    uint32_t u32ReceiveSize;
    usb_event_t stcSentReceiveEvent;
    stcSentReceiveEvent.u8Endpoint = 0;
    
    for(stcSentReceiveEvent.u8Endpoint = 5;stcSentReceiveEvent.u8Endpoint > 0;stcSentReceiveEvent.u8Endpoint--)
    {      
        if (IS_SET(EP_STATUS(stcSentReceiveEvent.u8Endpoint),MASK_EPS_DRQ) && (IS_CLEARED(EP_CONTROL(stcSentReceiveEvent.u8Endpoint),MASK_EPC_DIR) || (aEndpoint[stcSentReceiveEvent.u8Endpoint].pstcEndpointBuffer->u32Position != 0))) // happened an interrupt for endpoint (n) data register?
        {
            if (IS_CLEARED(EP_CONTROL(stcSentReceiveEvent.u8Endpoint),MASK_EPC_DIR))
            {
                aEndpoint[stcSentReceiveEvent.u8Endpoint].status |= EP_STATUS_RECEIVING;
                stcSentReceiveEvent.u8Event = USB_EVENT_RECEIVE;
            }
            else
            {
                aEndpoint[stcSentReceiveEvent.u8Endpoint].status |= EP_STATUS_SENDING;
                stcSentReceiveEvent.u8Event = USB_EVENT_SENT;
            }
            break;
        }
    }
    
    if (stcSentReceiveEvent.u8Endpoint != 0) {    // Try to handle sent/receive interrupt on endpoint u8EndpointNumber

        if (aEndpoint[stcSentReceiveEvent.u8Endpoint].pstcEndpointBuffer != 0)
        {
            if (stcSentReceiveEvent.u8Event == USB_EVENT_RECEIVE)
            {
                dbgval("RX  >>> ISR Enpoint ", stcSentReceiveEvent.u8Endpoint);
                #if (USB_USE_DMA == 1)
                if ((aEndpoint[stcSentReceiveEvent.u8Endpoint].status & EP_STATUS_DMA_ENABLED) > 0)
                {
                    dbg("RX  >>> DMA enabled\n");
                    UsbDma_SendReceiveBufferViaDma(stcSentReceiveEvent.u8Endpoint);
                    u32ReceiveSize = aEndpoint[stcSentReceiveEvent.u8Endpoint].pstcEndpointBuffer->u32DataLength;
                }
                else
                #endif //End USE_DMA
                {
                    u32ReceiveSize = UsbReceiveData(stcSentReceiveEvent.u8Endpoint,aEndpoint[stcSentReceiveEvent.u8Endpoint].pstcEndpointBuffer->pu8Buffer); // read USB endpoint fifo into buffer
                }
                stcSentReceiveEvent.pstcEndpointBuffer = aEndpoint[stcSentReceiveEvent.u8Endpoint].pstcEndpointBuffer;  // Point to received buffer in the event
                stcSentReceiveEvent.u32DataSize = u32ReceiveSize;                                         // Store received size into event
            
                if ((stcSentReceiveEvent.pstcEndpointBuffer->u32Position == 0) || ((stcSentReceiveEvent.pstcEndpointBuffer->u32BufferSize != 0) && (stcSentReceiveEvent.pstcEndpointBuffer->u32Position >= stcSentReceiveEvent.pstcEndpointBuffer->u32BufferSize))) 
                {
                    stcSentReceiveEvent.pstcEndpointBuffer->u32Position = 0;
                    aEndpoint[stcSentReceiveEvent.u8Endpoint].status &= ~EP_STATUS_RECEIVING;
                    dbg("RX  >>> Executing received event\n");
                    aEndpoint[stcSentReceiveEvent.u8Endpoint].handler(&stcSentReceiveEvent);                                // handle event
                }
            } 
            else    // sending multiple packages, package was sent (dma / interrupt)
            {
                dbgval("TX  >>> ISR Enpoint ", stcSentReceiveEvent.u8Endpoint);
                stcSentReceiveEvent.pstcEndpointBuffer = aEndpoint[stcSentReceiveEvent.u8Endpoint].pstcEndpointBuffer;
                dbg("TX> Transfer State: ");                                   //DEBUG
                dbghex(stcSentReceiveEvent.pstcEndpointBuffer->u32Position);   //DEBUG
                dbg(" Size: ");                                                //DEBUG
                dbghex(stcSentReceiveEvent.pstcEndpointBuffer->u32DataLength); //DEBUG
                dbg("\n");                                                     //DEBUG
                #if (USB_USE_DMA == 1)
                if ((aEndpoint[stcSentReceiveEvent.u8Endpoint].status & EP_STATUS_DMA_TRANSFER) > 0) // Is this an DMA transfer?
                {
                    dbg("TX> Transfer Type: DMA\n");
                    UsbDma_SendReceiveBufferViaDma(stcSentReceiveEvent.u8Endpoint);
                    if (stcSentReceiveEvent.pstcEndpointBuffer->u32Position == 0)
                    {
                        aEndpoint[stcSentReceiveEvent.u8Endpoint].status &= ~EP_STATUS_SENDING;
                        aEndpoint[stcSentReceiveEvent.u8Endpoint].handler(&stcSentReceiveEvent);
                    } 
                    
                } 
                else // interrupt transfer
                #endif //End USE_DMA
                {
                    dbg("TX> Transfer Type: ISR\n");
                    if ((stcSentReceiveEvent.pstcEndpointBuffer->u32Position) >= (stcSentReceiveEvent.pstcEndpointBuffer->u32DataLength)) 
                    {
                        if (((stcSentReceiveEvent.pstcEndpointBuffer->u32DataLength % aEndpoint[stcSentReceiveEvent.u8Endpoint].u16Size) == 0))
                        {
                            stcSentReceiveEvent.pstcEndpointBuffer->u32Position += 1;
                            UsbFunction_SendData(stcSentReceiveEvent.u8Endpoint,stcSentReceiveEvent.pstcEndpointBuffer->pu8Buffer,stcSentReceiveEvent.pstcEndpointBuffer->u32DataLength,USB_SENDING_MODE_INTERRUPT);
                        } 
                        CLEAR_MASK(*aEndpoint[stcSentReceiveEvent.u8Endpoint].EPnS,MASK_EPS_DRQIE);
                        stcSentReceiveEvent.pstcEndpointBuffer->u32Position = 0;
                        dbg("TX   |->Transfer finished...\n");
                        aEndpoint[stcSentReceiveEvent.u8Endpoint].status &= ~EP_STATUS_SENDING;
                        aEndpoint[stcSentReceiveEvent.u8Endpoint].handler(&stcSentReceiveEvent);                                // handle event
                    }
                    else
                    {
                        dbg("TX   |->Sending next data block...\n");
                        stcSentReceiveEvent.pstcEndpointBuffer = aEndpoint[stcSentReceiveEvent.u8Endpoint].pstcEndpointBuffer;
                        UsbFunction_SendData(stcSentReceiveEvent.u8Endpoint,stcSentReceiveEvent.pstcEndpointBuffer->pu8Buffer,stcSentReceiveEvent.pstcEndpointBuffer->u32DataLength,USB_SENDING_MODE_INTERRUPT);
                    }
                }
            }
        }
    }
}

/**
 ******************************************************************************
 **  Interrupt Routine (USB-Function Status & Control Interrupt) 
 **
 **  \note Used for control interrupts and endpoint 0 data receive / sent interrupts
 **
 ** \b Example Vectors.c
 ** \include Example_Vectors.c
 *****************************************************************************/
#if (USBMCUTYPE == USBMCU_MB91665) || (USBMCUTYPE == USBMCU_MB91660)
    void UsbFunction_StatusControlIsr(void)
#else
    __interrupt void UsbFunction_StatusControlIsr(void)
#endif
{
    usb_event_t stcEvent;
    
    if (IS_SET(EP0C, _EP0C_STAL))           // USB endpoint 0 was stalled?
    {
        UsbFunction_StallRemove(USB_EP0); // remove stall
    }
    if(IS_SET(UDCS, _UDCS_CONF))                     // Configure flag was set?
    {
        CLEAR_MASK(UDCS, _UDCS_CONF);                             // clear interrupt flag
        if (IS_SET(UDCIE, _UDCIE_CONFN))
        {
            stcEvent.u8Event = USB_EVENT_CONFIGURED;    // setup new event: event type configured
            SET_USB_STATUS_FLAG(USB_STATUS_CONFIGURED); // set configured status flag
            dbg("\n--> Configure Flag was set\n");      // DEBUG
            EnableEndpoints();                          // enable configured endpoints
        }
        else
        {
            stcEvent.u8Event = USB_EVENT_DECONFIGURED;    // setup new event: event type deconfigured
            CLEAR_USB_STATUS_FLAG(USB_STATUS_CONFIGURED); // set configured status flag
            dbg("\n--> Configure Flag was cleared\n");    // DEBUG
            DisableEndpoints();                           // disable configured endpoints except endpoint 0
        }
        stcEvent.u8UsbStatus = u8UsbStatus;     // setup new event: USB status
        if (pu8HandlerClassEvent != 0) 
        {
            pu8HandlerClassEvent(&stcEvent);    // Event Handler Vendor Class
        }
        if (pu8HandlerStatusEvent != 0) 
        {
            pu8HandlerStatusEvent(&stcEvent);   // Event Handler Main Application
        }
    } 

    if(IS_SET(EP0OS, _EP0OS_DRQO))                  // Data received on endpoint 0
    {
        if(IS_SET(UDCS, _UDCS_SETP))               // Type of received data is setup
        {
            UsbReceiveSetup();            // receive and handle setup
        }
        else
        {
            UsbReceiveDataEndpoint0();    // receive data on endpoint 0
        }
    }
    
    if(IS_SET(UDCS, _UDCS_SUSP)){            // Handle suspend interrupt   (HWM 29.3.5 rev16)
        dbg("--> SUSP was set\n");  // DEBUG
        CLEAR_MASK(UDCS, _UDCS_SUSP);             // clear interrupt flag       (HWM 29.3.5 rev16)
    }
    
    if(IS_SET(UDCS, _UDCS_SOF)){             // Handle SOF interrupt       (HWM 29.3.5 rev16)
        CLEAR_MASK(UDCS, _UDCS_SOF);              // clear interrupt flag       (HWM 29.3.5 rev16)
        if (SOFHandler != 0)
        {
           SOFHandler(0x00); //TMSP0 & 0x7FF);
        }
    }
    
    if(IS_SET(UDCS, _UDCS_WKUP)){            // Handle wakeup interrupt    (HWM 29.3.5 rev16)
        dbg("--> WKUP was set\n");  // DEBUG
        CLEAR_MASK(UDCS, _UDCS_WKUP);             // clear interrupt flag       (HWM 29.3.5 rev16)
    }
    
    if(IS_SET(UDCS, _UDCS_BRST)){            // Handle bus reset interrupt (HWM 29.3.5 rev16)
        dbg("--> Bus reset\n");     // DEBUG
        CLEAR_MASK(UDCS, _UDCS_BRST);             // clear interrupt flag       (HWM 29.3.5 rev16)
    }
    
}

/**
 ******************************************************************************
 **  Interrupt Routine (USB-Function VBUS Interrupt) 
 **
 ** \b Example Vectors.c
 ** \include Example_Vectors.c
 *****************************************************************************/
__interrupt void UsbFunction_VbusIsr(void)
{
        if (DEVICEVBUS_HIGHDETECT)    // interrupt while high level 
        {
            DEVICEVBUS_SETLOWDETECT;     // set detection to low level 
            ConnectEvent();    // execute connect state events
        } 
        else 
        {
            DEVICEVBUS_SETHIGHDETECT;     // set detection to high level 
            DisconnectEvent(); // execute disconnect state events 
        }
        DEVICEVBUS_CLEARISRFLAG;         // clear interrupt flag 
}

// @}
/*****************************************************************************/
/* Debug Section                                                             */
/*****************************************************************************/
// \name Debug Section 
// @{
#if (debug_enabled == 1)
static void DbgUsbRequest(usb_request_t * Request)
{
    dbg(" |->USB Request:\n");

    dbg(" |  |-> wValue: ");
    dbghex(Request->wValue);
    dbg("\n");
    
    dbg(" |  |-> wIndex: ");
    dbghex(Request->wIndex);
    dbg("\n");
    
    dbg(" |  |-> wLength: ");
    dbghex(Request->wLength);
    dbg("\n");
    
    dbg(" |  |-> Direction: ");
    if (Request->Direction) 
    {
        dbg("Device -> Host\n");
    } else 
    {
        dbg("Host -> Device\n");
    }
    
    dbg(" |  |-> Target: ");
    switch (Request->Target) {
        case USB_REQUEST_TYPE_STANDARD:
            dbg("STANDARD\n");
            break;
        case USB_REQUEST_TYPE_CLASS:
            dbg("CLASS\n");
            break;  
        case USB_REQUEST_TYPE_VENDOR:
            dbg("VENDOR SPECIFIC\n");
            break; 
    }
    dbg(" |  |-> Request: ");
    switch (Request->Request) 
    {
        case USB_GET_STATUS:
            dbg("GET_STATUS\n");
            break;
        case USB_CLEAR_FEATURE:
            dbg("CLEAR_FEATURE\n");
            break;  
        case USB_SET_FEATURE:
            dbg("SET_FEATURE\n");
            break;  
        case USB_SET_ADDRESS:
            dbg("SET_ADDRESS\n");
            break;  
        case USB_GET_DESCRIPTOR:
            dbg("GET_DESCRIPTOR\n");
            break;  
        case USB_SET_DESCRIPTOR:
            dbg("SET_DESCRIPTOR\n");
            break; 
        case USB_GET_CONFIGURATION:
            dbg("GET_CONFIGURATION\n");
            break;   
        case USB_SET_CONFIGURATION:
            dbg("SET_CONFIGURATION\n");
            break; 
        case USB_SET_INTERFACE:
            dbg("SET_INTERFACE\n");
            break; 
        case USB_GET_INTERFACE:
            dbg("GET_INTERFACE\n");
            break; 
        case USB_SYNCH_FRAME:
            dbg("SYNCH_FRAME\n");
            break;
        default:
            dbg("UNKNOWN\n");
            break;
    }
    
}

static void DbgUsbGetDescriptor(usb_request_t * ptRequest)
{
    dbg(" |-> Send Descriptor: ");
    switch (((ptRequest->wValue) >> 8) & 0x00FF) 
    {
        case  USB_REQUEST_DEVICE_DESCRIPTOR:
            dbg("DEVICE\n");
            break;
        case USB_REQUEST_CONFIG_DESCRIPTOR:
            dbg("CONFIGURATION\n");
            break;
        case USB_REQUEST_STRING_DESCRIPTOR:
            dbg("STRING\n");
            break;
        case USB_REQUEST_INTERF_DESCRIPTOR:
            dbg("INTERFACE\n");
            break;
        case USB_REQUEST_ENDPNT_DESCRIPTOR:
            dbg("Endpoint\n");
            break;
        case USB_REQUEST_DEVQUA_DESCRIPTOR:
            dbg("DEVICE_QUALIFIER\n");
            break;
        case USB_REQUEST_OTHSPE_DESCRIPTOR:
            dbg("OTHER_SPEED_CONFIGURATION\n");
            break; 
        case USB_REQUEST_IFPOWR_DESCRIPTOR:
            dbg("INTERFACE_POWER\n");
            break; 
        case USB_REQUEST_REPORT_DESCRIPTOR:
            dbg("REPORT_DESCRIPTOR\n");
            break; 
        default:
            dbg("NOT FOUND: ");
            dbghex(((ptRequest->wValue) >> 8) & 0x00FF);
            dbg("\n");
            break;
    }
}
#endif 
#endif /* (USE_USB_FUNCTION == 1) */



