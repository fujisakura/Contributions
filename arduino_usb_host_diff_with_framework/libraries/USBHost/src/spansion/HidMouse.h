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
/** \file HidMouse.h
 **
 ** Part of Spansion LLC USB Host Driver Module
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version 
 **   - 2011-08-24    1.1  MSc  Some fixes in X/Y calculation
 *****************************************************************************/


#ifndef __HIDMOUSE_H__
#define __HIDMOUSE_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

//#include "mcu.h"
//#include "base_type.h"
//#include "UsbHost.h"
#include "Arduino.h"

#if (USE_USB_HOST == 1)
#include "HidMouse.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/
typedef uint8_t Buttons_t;

typedef struct Point
{
    uint32_t u32X;
    uint32_t u32Y;
} Point_t;

typedef struct Diff
{
    int8_t i8X;
    int8_t i8Y;
} Diff_t;

typedef struct MouseData
{
    Point_t   stcPosition;
    Buttons_t tButtons;
    Diff_t    stcDiffXY;
    uint32_t  u32ScrollPosition;
    int8_t    i8Scrolling;    
} MouseData_t;


typedef uint8_t MouseEventType_t;
#define MOUSEEVENT_POSITIONX_CHANGED 1
#define MOUSEEVENT_POSITIONY_CHANGED 2
#define MOUSEEVENT_BUTTON_CHANGED    4
#define MOUSEEVENT_SCROLLING_CHANGED  8
#define MOUSEEVENT_ANY_CHANGE        0xFF
/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/



/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

void HidMouse_Init();
boolean_t HidMouseInitHandler(uint8_t* pu8Configuration, uint32_t u32Length);
boolean_t HidMouseDeinitHandler();
void HidMouse_Init();
void HidMouse_DataReceivedHandler();

void HidMouse_SetEvent(void (*Handler)(MouseEventType_t stcEventType, MouseData_t MouseData));
void HidMouse_RemoveEvent();

Point_t HidMouse_GetCurrentPosition();
void HidMouse_SetCurrentPosition(uint32_t u32X, uint32_t u32Y);
Buttons_t HidMouse_GetButtons();
uint32_t HidMouse_GetCurrentScrollPosition();
boolean_t HidMouse_Moved();

void UMH_StallExecutionUserCallback(unsigned long time);
void Idle(void);

extern void HidDataCopy(uint32_t len, uint8_t *buf);

#endif /* (USE_USB_HOST == 1) */
#endif /* __HIDMOUSE_H__*/
