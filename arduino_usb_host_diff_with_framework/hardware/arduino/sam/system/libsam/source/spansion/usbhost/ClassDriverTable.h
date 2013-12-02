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
/** \file ClassDriverTable.h
 **
 ** Part of FSEU USB Host Driver Module
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version
 *****************************************************************************/

#ifndef __CLASSDRIVERTABLE_H__
#define __CLASSDRIVERTABLE_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

//#include "Usb.h"
#include "../usb/Usb_fm3.h"

#if (USE_USB_HOST == 1)
//#include "UsbMassStorage.h"
//#include "HidMouse.h"
//#include "HidKeyboard.h"
/* OTHER USB CLASS DRIVER INCLUDES CAN BE ADDED HERE */

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

/* DEFINEPARSER(DRIVERTYPES) START */
#define USBCLASSDRIVER_MASSSTORAGE 1
#define USBCLASSDRIVER_MOUSE 2
#define USBCLASSDRIVER_JOYSTICK 3
#define USBCLASSDRIVER_KEYBOARD 4

/* DEFINEPARSER(DRIVERTYPES) STOP */

extern boolean_t MassStorageInitHandler(uint8_t* pu8Configuration, uint32_t u32Length);
extern boolean_t MassStorageDeinitHandler();
extern boolean_t HidMouseInitHandler(uint8_t* pu8Configuration, uint32_t u32Length);
extern boolean_t HidMouseDeinitHandler();
extern boolean_t HidKeyboardInitHandler(uint8_t* pu8Configuration, uint32_t u32Length);
extern boolean_t HidKeyboardDeinitHandler();

/* OTHER USB CLASS DRIVER DEFINES CAN BE ADDED HERE */

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

#ifdef __USBCLASSSUPERVISOR_C__
#define MAX_CLASSHANDLERS 3
    const UsbClassHandler_t UsbClassHandlers[] =
    {
        {MassStorageInitHandler,MassStorageDeinitHandler,0,0,0,0,0,8,6,80,USBCLASSDRIVER_MASSSTORAGE},
        {HidMouseInitHandler,HidMouseDeinitHandler,0,0,0,0,0,3,1,2,USBCLASSDRIVER_MOUSE},
        {HidKeyboardInitHandler,HidKeyboardDeinitHandler,0,0,0,0,0,3,1,1,USBCLASSDRIVER_KEYBOARD},
    };
#endif

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

#endif /* (USE_USB_HOST == 1) */

#endif /* __CLASSDRIVERTABLE_H__ */


