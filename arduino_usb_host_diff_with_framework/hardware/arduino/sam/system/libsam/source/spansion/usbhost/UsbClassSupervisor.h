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
/** \file UsbClassSupervisor.h
 **
 ** Part of Spansion LLC USB Host Driver Module
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version
 *****************************************************************************/
#ifndef __USBCLASSSUPERVISOR_H__
#define __USBCLASSSUPERVISOR_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/


//#include "Usb.h"
#include "../usb/Usb_fm3.h"


#if (USE_USB_HOST == 1)
#include "../usb/UsbSofTimeout.h"
#include "../base_type.h"
#include "../dbg/dbg.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USBCLASSSVR_IDLE 0
#define USBCLASSSVR_REQUEST_DESCRIPTORS 1
#define USBCLASSSVR_CLASSCHECK 2
#define USBCLASSSVR_ERROR 3

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/

typedef uint8_t UsbClassDriver_t;


typedef struct UsbClassHandler
{
    boolean_t (*InitializeClass)(uint8_t* pu8Configuration, uint32_t u32Length);
    boolean_t (*DeviceDisconnection)( void );
    uint16_t u16IdVendor;
    uint16_t u16IdProduct;
    uint8_t u8MatchDeviceClass;
    uint8_t u8MatchDeviceSubClass;
    uint8_t u8MatchDeviceProtocol;
    uint8_t u8MatchInterfaceClass;
    uint8_t u8MatchInterfaceSubClass;
    uint8_t u8MatchInterfaceProtocol;
    UsbClassDriver_t stcDriverType;
} UsbClassHandler_t;

#include "ClassDriverTable.h"

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/
extern const UsbRequest_t stcDeviceDescriptorRequest;

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/
void UsbClassSupervisor_Init();
void UsbClassSupervisor_Deinit();
void UsbClassSupervisor_SysTickHandler();
boolean_t UsbSupervisor_UsbClassMatch(UsbClassHandler_t* stcA, UsbClassHandler_t* stcB);
void UsbClassSupervisor_SetErrorHandler(void (*Handler)(UsbClassHandler_t stcMatchClass));
UsbClassDriver_t UsbClassSupervisor_GetActiveDriver();

void ReturnedDeviceDescriptor(uint8_t* pu8Buffer, uint32_t u32DataSize);
void ReturnedConfigurationDescriptor(uint8_t* pu8Buffer, uint32_t u32DataSize);

#endif /* (USE_USB_HOST == 1) */
#endif /* __USBCLASSSUPERVISOR_H__ */

