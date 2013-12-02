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
/** \file UsbFunction.h
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
 *****************************************************************************/

#ifndef __USBFUNCTION_H__
#define __USBFUNCTION_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
//#include "Usb.h"
#include "../usb/Usb_fm3.h"
#if (USE_USB_FUNCTION == 1)
#include "base_type.h"
#include "mcu.h"
#include "dbg.h"
#include "UsbRegisters.h"
#include "UsbFunctionHW.h"
#include "UsbSofTimeout.h"
/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USB_USE_DMA 0
#define USB_BUFFER_FAR 0

#if (USB_USE_DMA == 1)
    #if (CPU_TYPE == CPU_TYPE_32BIT)
       #error At the moment DMA is only running at 16FX MCUs
    #endif
    #include "UsbDma.h"
#endif

#define IS_SET(x,y)  ((x & y) > 0) 
#define IS_CLEARED(x,y)  ((x & y) == 0) 
#define SET_MASK(x,y)  x |= y 
#define CLEAR_MASK(x,y)  x &= ~y
#define EP_CONTROL(x)  *(aEndpoint[x].EPnC)
#define EP_STATUS(x)  *(aEndpoint[x].EPnS)
#define GET_ENDPOINT(x)         (*UsbFunction_GetEndpoint(x))
#define EXT_ENDPOINT_STATUS(x)  (*((volatile uint16_t *)(GET_ENDPOINT(x).EPnS)))
#define EXT_ENDPOINT_CONTROL(x) (*((volatile uint16_t *)(GET_ENDPOINT(x).EPnC)))

#define EP_STATUS_IDLE          0
#define EP_STATUS_CONFIGURED    1
#define EP_STATUS_DISABLED      2
#define EP_STATUS_ENABLED       4
#define EP_STATUS_RECEIVING     8
#define EP_STATUS_SENDING       16
#define EP_STATUS_DMA_TRANSFER  32
#define EP_STATUS_DMA_ENABLED   64

#define SET_USB_STATUS_FLAG(x)    u8UsbStatus |= x  // Set a flag in the USB Status #u8UsbStatus
#define CLEAR_USB_STATUS_FLAG(x)  u8UsbStatus &= ~x // Clear a flag in the USB Status #u8UsbStatus
#define CLEAR_USB_STATUS_FLAGS()  u8UsbStatus = 0   // Clear all flags in the USB Status #u8UsbStatus
#define pu16EndpointStatusReg_DRQ    (*pu16EndpointStatusReg & 0x0400)
#define SET_pu16EndpointStatusReg_DRQ(x)    *pu16EndpointStatusReg = ((*pu16EndpointStatusReg & 0xFBFF) | x << 10)
#define SET_pu16EndpointStatusReg_BFINI(x)    *pu16EndpointStatusReg = ((*pu16EndpointStatusReg & 0x7FFF) | x << 15)

#define MASK_EPC_PKS            0x007F
#define MASK_EPC_PKS1           0x01FF
#define MASK_EPC_STAL           0x0200
#define MASK_EPC_NULE           0x0400
#define MASK_EPC_DMAE           0x0800
#define MASK_EPC_DIR            0x1000
#define MASK_EPC_TYPE           0x6000
#define MASK_EPC_EPEN           0x8000

#define MASK_EPS_DRQIIE         0x4000
#define MASK_EPS_BFINI          0x8000
#define MASK_EPS_SIZE1          0x01FF
#define MASK_EPS_SIZE           0x007F
#define MASK_EPS_SPK            0x0200
#define MASK_EPS_DRQI           0x0400
#define MASK_EPS_DRQO           0x0400
#define MASK_EPS_DRQ            0x0400
#define MASK_EPS_BUSY           0x0800
#define MASK_EPS_SPKIE          0x2000
#define MASK_EPS_DRQIE          0x4000
#define MASK_EPS_DRQOIE         0x4000
#define MASK_EPS_BFINI          0x8000

#define USBDESCR_DEVICE         1
#define USBDESCR_CONFIG         2
#define USBDESCR_STRING         3
#define USBDESCR_INTERFACE      4
#define USBDESCR_ENDPOINT       5

#define USBATTR_BUSPOWER        0x80
#define USBATTR_SELFPOWER       0xC0
#define USBATTR_REMOTEWAKE      0x20

#define USB_FUNC_INTR_IN    0xF000
#define USB_FUNC_INTR_OUT    0xE000
#define USB_FUNC_BULK_IN    0xD000
#define USB_FUNC_BULK_OUT    0xC000

#define MAX_RECV_BUFFER        512

#define USB_DEVREQ_STDTYPE    0x00
#define USB_DEVREQ_CLSTYPE    0x20
#define USB_DEVREQ_VNDTYPE    0x40

#define USB_DEVREQ_GET_DESC    0x06
#define USB_DEVREQ_SET_DESC    0x07
#define USB_DEVREQ_SET_IDLE    0x0A

#define USB_GET_STATUS        0x00
#define USB_CLEAR_FEATURE     0x01
#define USB_SET_FEATURE       0x03
#define USB_SET_ADDRESS       0x05
#define USB_GET_DESCRIPTOR    0x06
#define USB_SET_DESCRIPTOR    0x07
#define USB_GET_CONFIGURATION 0x08
#define USB_SET_CONFIGURATION 0x09
#define USB_GET_INTERFACE     0x0A
#define USB_SET_INTERFACE     0x0B
#define USB_SYNCH_FRAME       0x0C


#define USB_STATUS_CONNECTED 0x01
#define USB_STATUS_DEVDESC 0x02
#define USB_STATUS_CONFDESC 0x04
#define USB_STATUS_CONFIGURED 0x08


#define USB_REQUEST_TYPE_STANDARD 0
#define USB_REQUEST_TYPE_CLASS 1
#define USB_REQUEST_TYPE_VENDOR 2

#define USB_REQUEST_DEVICE_DESCRIPTOR 0x01
#define USB_REQUEST_CONFIG_DESCRIPTOR 0x02
#define USB_REQUEST_STRING_DESCRIPTOR 0x03
#define USB_REQUEST_INTERF_DESCRIPTOR 0x04
#define USB_REQUEST_ENDPNT_DESCRIPTOR 0x05
#define USB_REQUEST_DEVQUA_DESCRIPTOR 0x06
#define USB_REQUEST_OTHSPE_DESCRIPTOR 0x07
#define USB_REQUEST_IFPOWR_DESCRIPTOR 0x08
#define USB_REQUEST_REPORT_DESCRIPTOR 0x22

#define USB_EVENT_TARGET_GENERAL         0x01
#define USB_EVENT_TARGET_MAINAPP         0x02
#define USB_EVENT_TARGET_VENDOR          0x03
#define USB_EVENT_TARGET_INTERFACE       0x04
#define USB_EVENT_DATARECEIVED_ENDPOINT0 0x05

#define USB_EVENT_CONNECT      0x01
#define USB_EVENT_DISCONNECT   0x02
#define USB_EVENT_RECEIVE      0x04
#define USB_EVENT_SENT         0x08
#define USB_EVENT_CLASSREQUEST 0x10
#define USB_EVENT_CONFIGURED   0x20
#define USB_EVENT_DECONFIGURED 0x40

#define USB_SENDING_MODE_INTERRUPT 0
#define USB_SENDING_MODE_POLLED    1
#define USB_SENDING_MODE_DMA       2

#define USB_EP0  0x00
#define USB_EP_1 0x01
#define USB_EP_2 0x02
#define USB_EP_3 0x03
#define USB_EP_4 0x04
#define USB_EP_5 0x05

#define sendEP0(x,y)    UsbFunction_SendData(0,x,y,1)
#define sendEP1(x,y)    UsbFunction_SendData(1,x,y,1)
#define sendEP2(x,y)    UsbFunction_SendData(2,x,y,1)
#define sendEP3(x,y)    UsbFunction_SendData(3,x,y,1)
#define sendEP4(x,y)    UsbFunction_SendData(4,x,y,1)
#define sendEP5(x,y)    UsbFunction_SendData(5,x,y,1)

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/

#if (USB_BUFFER_FAR == 1)
    typedef __far uint8_t*   puint8_t;
    typedef __far uint8_t    fnuint8_t;
#else
    typedef uint8_t*         puint8_t;
    typedef uint8_t          fnuint8_t;
#endif


typedef struct stc_usb_request 
{
    uint8_t Request;
    uint8_t Direction;
    uint8_t Type;
    uint8_t Target;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_request_t;

typedef struct stc_usb_buffer 
{
    puint8_t pu8Buffer;
    uint32_t u32BufferSize;
    uint32_t u32Position;
    uint32_t u32DataLength;
} usb_buffer_t;

typedef struct stc_usb_event 
{
    uint8_t u8UsbStatus;
    uint8_t u8Event;
    usb_request_t * Request;
    usb_buffer_t * pstcEndpointBuffer;
    uint32_t u32DataSize;
    uint8_t u8Endpoint;
} usb_event_t;

typedef struct stc_usb_EP {
    uint8_t status;
    volatile uint8_t * EPnDTH;
    volatile uint8_t * EPnDTL;
    volatile uint16_t * EPnS;
    volatile uint16_t * EPnC;
    uint16_t u16Size;
    usb_buffer_t * pstcEndpointBuffer;
    uint8_t (* handler)(usb_event_t * stcEvent);
    uint8_t u8DmaNumber;
} usb_EP_t;

/*typedef struct stc_timeout_handler
{
    uint16_t u16TimeOut;
    void (* Handler)(void);
} stc_timeout_handler_t;*/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/





void SetupEndpoints();
void EnableEndpoints(void); 
void DisableEndpoints(void);
void ConnectEvent();
void DisconnectEvent();

#if USB_USE_DMA
    #define UsbFunction_ChangeDmaSettings(x, y) UsbDma_ChangeDmaSettings(x,y)
    #define UsbFunction_EnableReceiveDma(x, y)  UsbDma_ChangeDmaSettings(x,y)
#endif

boolean_t UsbFunction_AddTimeOut(void (* Handler)(void), uint16_t u16TimeOut);
void UsbFunction_RemoveTimeOut(void (* Handler)(void));
usb_EP_t* UsbFunction_GetEndpoint(uint8_t u8EndpointNumber);
uint8_t UsbFunction_GetStatus();
uint8_t UsbFunction_GetEndpointStatus(uint8_t u8EndpointNumber);
void UsbFunction_SetEventHandler(uint8_t,uint8_t,uint8_t,uint8_t (*) ());
void UsbFunction_SetCustomEndpointBuffer(uint8_t,usb_buffer_t *);
void UsbFunction_EnableSOF(void);
void UsbFunction_DisableSOF(void);
void UsbFunction_SetStartOfFrameHandler(void(* handler)(uint16_t u16FrameNumber));
void UsbFunction_ClearStartOfFrameHandler(void);
void UsbFunction_Initialize();
boolean_t UsbFunction_Connect(boolean_t bAutoConnect);
boolean_t UsbFunction_Disconnect(boolean_t bForceDisconnect);
void UsbFunction_SendData(uint8_t,puint8_t,uint32_t, uint8_t);
void UsbFunction_SetDataToSendEndpoint0(uint8_t *,uint32_t);
void UsbFunction_EnableWakeup(boolean_t bEnableDisable);
void UsbFunction_Stall(uint8_t u8EndpointNumber);
void UsbFunction_StallRemove(uint8_t u8EndpointNumber);

void TimeoutHandler(uint16_t ticks);
void UsbReceiveSetup();
uint8_t UsbGetDescriptor(usb_request_t *);
uint8_t UsbRequestDecode(puint8_t,uint32_t);
void UsbReceiveDataEndpoint0();
uint32_t UsbReceiveData(uint8_t,puint8_t);
uint8_t DummyEventHandler(usb_event_t *);

#if ((__CC_ARM != 1) && !defined(__IAR_SYSTEMS_ICC__))
__interrupt void UsbFunction_EndpointIsr();
#endif
#if (USBMCUTYPE == USBMCU_MB91665) || (USBMCUTYPE == USBMCU_MB91660)
    void UsbFunction_StatusControlIsr(void);
#else
    __interrupt void UsbFunction_StatusControlIsr(void);
#endif
#if ((__CC_ARM != 1) && !defined(__IAR_SYSTEMS_ICC__))
__interrupt void UsbFunction_VbusIsr();
#else
void UsbFunction_VbusIsr();
#endif


#if (debug_enabled == 1)
    void DbgUsbRequest(usb_request_t * Request);
    void DbgUsbGetDescriptor(usb_request_t * ptRequest);
#endif
#endif /* (USE_USB_FUNCTION == 1) */
#endif /* __USBFUNCTION_H__ */

