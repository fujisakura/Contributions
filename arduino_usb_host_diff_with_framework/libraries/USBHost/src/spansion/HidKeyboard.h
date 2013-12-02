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
/** \file HidKeyboard.h
 **
 ** Part of Spansion LLC USB Host Driver Module
 **
 ** History:
 **   - 2010-12-13    1.0  MSc  First version  (works with 16FX)
 **   - 2010-01-05    1.1  MSc  API Updates
 **   - 2011-03-30    1.2  MSc  Public Release
 **   - 2011-08-24    1.3  MSc  Bug fixes
 *****************************************************************************/

#ifndef __HIDKEYBOARD_H__
#define __HIDKEYBOARD_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

//#include "mcu.h"
//#include "base_type.h"
//#include "UsbHost.h"
#include "Arduino.h"

#if (USE_USB_HOST == 1)
#include "HidKeyboard.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define KEYCODE_ASCII(keycode)  u8Keycodes[keycode]

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/


typedef struct KeyboardData
{
    uint8_t u8ModifierKeys;
    uint8_t u8Reserved;
    uint8_t u8KeyCode1;
    uint8_t u8KeyCode2;
    uint8_t u8KeyCode3;
    uint8_t u8KeyCode4;
    uint8_t u8KeyCode5;
    uint8_t u8KeyCode6;
} KeyboardData_t;



/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

#ifndef __HIDKEYBOARD_C__
    extern const uint8_t u8Keycodes[];
#endif

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

void HidKeyboard_Init();
boolean_t HidKeyboardInitHandler(uint8_t* pu8Configuration, uint32_t u32Length);
boolean_t HidKeyboardDeinitHandler();
void HidKeyboard_Init();
void HidKeyboard_DataReceivedHandler();
void HidKeyboard_SetEventHandler(void (*Handler)(KeyboardData_t *stcData));
void HidKeyboard_RemoveEventHandler();
char_t HidKeyBoard_GetCh();

void HidKeyboard_RemoveEvent();
void UMH_StallExecutionUserCallback(unsigned long time);
void Idle(void);

extern void HidDataCopy(uint32_t len, uint8_t *buf);

#endif /* (USE_USB_HOST == 1) */
#endif /* __HidKeyboard_H__*/
