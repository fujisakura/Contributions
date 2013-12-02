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
/** \file UsbHost.h
 **
 ** Spansion LLC Usb Host Header File
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version  (works with 16FX, FR80, FM3)
 **   - 2011-08-11    1.2  MSc  Support for transfer abort
 **   - 2011-08-24    1.3  MSc  Several Timing fixes
 **   - 2011-08-29    1.4  MSc  Low-Speed / Full-Speed switching bug fixes
 *****************************************************************************/

#ifndef __USBHOST_H__
#define __USBHOST_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
//#include "Usb.h"
#include "../usb/Usb_fm3.h"

#if (USE_USB_HOST == 1)
//#include "mcu.h"
#include "../usb/UsbInit.h"
#include "../base_type.h"
#include "../dbg/dbg.h"
//#include "UsbHost.h"
#include "../usb/UsbSofTimeout.h"
#include "../usb/UsbSpec.h"
#include "../usb/UsbRegisters.h"
#include "UsbHostHW.h"
/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define ZERO_STRUCT(x) xmemset((uint8_t *)&x,0,sizeof(x))
#define HSTATE0_CLKSEL _hstate0.bit._CLKSEL
#define HSTATE0_ALIVE _hstate0.bit._ALIVE

#define MAX_HOSTENDPOINTS 6
// Token mask in the HTOKEN register
#define HTOKEN_SETUP 						0x10
#define HTOKEN_IN    						0x20
#define HTOKEN_OUT   						0x30
#define HTOKEN_SOF   						0x40

#define SET_ADDRESS(Address)                 UsbHost_SetupRequest({0x00,0x05,Address,0x00,0x00,0x00,0x00,0x00})
#define SETUP_TOKEN(toggle, type, endpoint)  (toggle << 7) | type | endpoint

#if (debug_enabled == 1)
   #define dbgtoken(x) dbg_token(x)
#else
   #define dbgtoken(x)
#endif
#define SET_TOKEN(Handler,Token)             Handler->u8LastToken = SETUP_TOKEN(Handler->bToggle,Token,Handler->u8Address & 0x0F); DBGOUT(1,dbgtoken(Handler->u8LastToken)); ActiveEndpoint = Handler; u8LastToken = Handler->u8LastToken; u8SyncToken = Handler->u8LastToken
#define SET_TOKEN_NOW(Handler,Token)         Handler->u8LastToken = SETUP_TOKEN(Handler->bToggle,Token,Handler->u8Address & 0x0F); ; ActiveEndpoint = Handler; u8LastToken = Handler->u8LastToken; HTOKEN = Handler->u8LastToken

// HERR Token Completion Masks
// if HERR is zero then no error occur
//SOF token error
#define HERR_SOF_MASK  						0x80 
#define HERR_RCV_MASK  						0x40
#define HERR_TOUT_MASK 						0x20
#define HERR_CRC_MASK  						0x10
#define HERR_TOGGLE_MASK					0x8
#define HERR_BITSTUFF_MASK				    0x4

#define USBHOST_SCHEDULER_COMPLETION  0x01
#define USBHOST_SCHEDULER_SOF  0x02
#define USBHOST_SCHEDULER_IDLE 0x03
#define USBHOST_SCHEDULER_BUSY 0x04

#define USBHOST_SUCCESS 0
#define USBHOST_ERROR   1

#define USBHOST_ENDPOINTSTATUS_INITIATE    0x01
#define USBHOST_ENDPOINTSTATUS_ACTIVE      0x02
#define USBHOST_ENDPOINTSTATUS_INUSE       0x04
#define USBHOST_ENDPOINTSTATUS_DATA        0x08
#define USBHOST_ENDPOINTSTATUS_STALL       0x10

#define HERR_ACK          				      0
#define HERR_NAK          				    0x01
#define HERR_STALL        				    0x02
#define HERR_NULL         				    0x03

#define BM_REQUESTTYPE_DIROUT    0x00
#define BM_REQUESTTYPE_DIRIN     0x80

#define BM_REQUESTTYPE_DIR       0x80
#define BM_REQUESTTYPE_TYPE      0x60
#define BM_REQUESTTYPE_RECEIVER  0x1F


#define USBHOST_STAGE_IDLE      1
#define USBHOST_STAGE_SETUP     2
#define USBHOST_STAGE_DATA      3
#define USBHOST_STAGE_STATUS    4
#define USBHOST_STAGE_HANDSHAKE 5

#define USBHOST_DEVICE_IDLE 1
#define USBHOST_DEVICE_ADDRESSING 2
#define USBHOST_DEVICE_ADDRESSED 3
#define USBHOST_DEVICE_ENUMERATED 4
#define USBHOST_DEVICE_CONFIGURATING 5
#define USBHOST_DEVICE_CONFIGURED 6

#ifndef IS_SET
    #define IS_SET(x,y)  ((x & y) > 0) 
#endif

#ifndef IS_CLEARED
    #define IS_CLEARED(x,y)  ((x & y) == 0) 
#endif
    
#ifndef SET_MASK
    #define SET_MASK(x,y)  x |= y 
#endif

#ifndef CLEAR_MASK
    #define CLEAR_MASK(x,y)  x &= ~y
#endif

#ifndef TOGGLE_MASK
    #define TOGGLE_MASK(x,y) SET_MASK(x,y); CLEAR_MASK(x,y) 
#endif
#define ISOUT(x)    (!ISIN(x))
#define ISIN(x)     ((Handler->u8Address) & 0x80)

#define USB_DESCRIPTOR_LENGTH                    0x00
#define USB_DESCRIPTOR_DESCRIPTORTYPE            0x01
#define USB_INTERFACE_DESCRIPTOR_INTERFACENUMBER 0x03

#define USBH_STATUS_SUCCESS 0x0000
#define USBH_STATUS_ERROR   0x0001
#define USBH_STATUS_STALL   0x0008
#define USBH_STATUS_LENGTH  0x000D
/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/
typedef struct UsbRequest
{
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} UsbRequest_t;

typedef struct SetupPackage
{
    UsbRequest_t* pstcRequest;
    uint8_t*      pu8Buffer;    
} SetupPackage_t;

typedef struct HostEndpoint
{
    uint8_t*  pu8Buffer;
    uint8_t*  pu8BufferPos;
    uint8_t*  pu8BufferPosNextPackage;
    uint32_t  u32DataSize;
    uint32_t  u32BufferSize;
    uint16_t  u16FifoSize;
    void      (* CompletionHandler)(void);
    boolean_t bToggle;
    uint8_t   u8LastToken;
    volatile uint8_t   u8Status;
    volatile uint8_t   u8Stage;
    uint8_t   u8Address;
    uint8_t   u8Interval;
    uint8_t   u8IntervalCount;
    uint8_t   u8Retries;
    boolean_t bAbortTransfer;
} HostEndpoint_t;



/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/


/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/
void UsbHost_Init();
void UsbHost_DeInit(void);
void UsbHost_BusReset();
void UsbHost_StartSOFToken();
uint8_t UsbHost_GetDeviceStatus();

uint16_t UsbHost_TransferFifoToBuffer(uint8_t* pu8Buffer);

void UsbHost_SetConfigurationDescriptor(uint8_t u8Configuration);
void UsbHost_GetConfigurationDescriptor(uint8_t u8Configuration);
void UsbHost_AddressDevice(uint8_t NewAddress);
void UsbHost_SetupRequest(UsbRequest_t* pstcSetup, void(*SetupCompletion)(uint8_t* pu8Buffer, uint32_t u32DataSize));
void UsbHost_SetupRequestWithData(UsbRequest_t* pstcSetup, uint8_t* pu8Data, void(*SetupCompletion)(uint8_t* pu8Buffer, uint32_t u32DataSize));

#define  UsbHost_GetUsbInterfaceDescriptor(x,y,z)    UsbHost_GetUsbEndpointDescriptor(x,y,z, 0)
uint8_t* UsbHost_GetUsbEndpointDescriptor(uint8_t* pu8Buffer, uint16_t u16Size, uint8_t u8InterfaceNumber, uint8_t u8EndpointNumber);




uint8_t UsbHost_AddHostEndpoint(HostEndpoint_t** Handler, uint8_t u8EndpointAddress, uint16_t u16FifoSize, uint8_t u8Interval, void (* CompletionHandler)(void));
void UsbHost_ClearAllEndpoints();
HostEndpoint_t* FromEndpointNumber(uint8_t EndpointNumber);

void UsbHost_TransferDataToFifo(uint8_t* pu8Buffer, uint16_t u16Size, uint16_t u16FifoSize);
uint16_t UsbHost_TransferFifoToBuffer(uint8_t* pu8Buffer);

void UsbHost_RequestReceiveData(HostEndpoint_t* Handler);
boolean_t UsbHost_ReceiveData(HostEndpoint_t* Handler);
void UsbHost_SendData(HostEndpoint_t* Handler, boolean_t bNow);
void UsbHost_TransferData(HostEndpoint_t* Handler,uint8_t* pu8Buffer, uint32_t u32BufferSize,  void (* CompletionHandler)(void));

void TimeoutHandler(uint16_t ticks);
boolean_t UsbHost_AddTimeOut(void (* Handler)(void), uint16_t u16TimeOut);
void UsbHost_RemoveTimeOut(void (* Handler)(void));


void UsbHost_Scheduler(uint8_t u8Option);

void EndpointInStatusUpdate(uint8_t* pu8Buffer, uint32_t u32Size);
void EndpointOutStatusUpdate(uint8_t* pu8Buffer, uint32_t u32Size);
void SetupComplete(void);
void ProcessSetup();
uint32_t ReceiveSetupData(uint8_t** pu8Buffer);
void ProcessEndpointStage(HostEndpoint_t* Handler);

void ConvertRequestToByteArray(uint8_t* pu8Buffer, UsbRequest_t* pstcSetup);

void xmemset(uint8_t *dest, uint8_t fill, uint32_t length);
void xmemcpy(uint8_t *dest, uint8_t *src, uint32_t length);
void UsbDummyFunction(void);

#if (USBMCUTYPE == USBMCU_MB91665)
    void UsbHost_ISR(void);
#else
    __interrupt void UsbHost_ISR(void);
#endif

#if (debug_enabled == 1)
    void dbg_HERR(void);
    void dbg_token(uint8_t u8Token);
#endif
#endif /* (USE_USB_HOST == 1) */

#endif /* __USBHOST_H__*/
