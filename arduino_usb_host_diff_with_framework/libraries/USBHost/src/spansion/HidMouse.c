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
/** \file HidMouse.c
 **
 ** Part of Spansion LLC USB Host Driver Module
 **
 ** History:
 **   - 2011-03-30    1.0  MSc  First version 
 **   - 2011-08-24    1.1  MSc  Some fixes in X/Y calculation
 *****************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

//#include "Usb.h"
#include "Arduino.h"

#if (USE_USB_HOST == 1)
#include "HidMouse.h"


/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/


/*****************************************************************************/
/* function */
/*****************************************************************************/


/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/
static HostEndpoint_t *EndpointIN;
static uint8_t        u8MouseData[5];
static uint8_t        u8EndpointIN;
static uint16_t       u16MaxPackageSizeIN;
void (*MouseEventHandler)(MouseEventType_t stcEventType, MouseData_t MouseData) = 0;
static MouseData_t stcMouseData;
static boolean_t bMouseMoved = FALSE;
/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/



/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

boolean_t HidMouseInitHandler(uint8_t* pu8Configuration, uint32_t u32Length)
{
    uint8_t* pu8Buffer;
    uint8_t u8NumberOfInterfaces;
    uint8_t u8NumberOfEndpoints;
    uint8_t u8EndpointNumber;
    uint8_t u8InterfaceNumber;
    uint8_t u8Interval;
    
    dbg("Yeah, called if a HID Device was connected ;-)\n");
    
    u8NumberOfInterfaces=pu8Configuration[4];
    for(u8InterfaceNumber=0;u8InterfaceNumber < u8NumberOfInterfaces;u8InterfaceNumber++)
    {
        dbgval("Interface Index: ",u8InterfaceNumber);
        pu8Buffer = UsbHost_GetUsbInterfaceDescriptor(pu8Configuration,u32Length,u8InterfaceNumber);
        if (pu8Buffer == 0)
        {
            dbg("Error: Could not Request Interface Descriptor.\n");
            return FALSE;
        }
        u8NumberOfEndpoints = pu8Buffer[4];
        
        if ((pu8Buffer[6] == 0x01) && (pu8Buffer[7] == 0x2)) //SubClass & Protocol
        {
            for(u8EndpointNumber=1;u8EndpointNumber<=u8NumberOfEndpoints;u8EndpointNumber++)
            {
                dbgval("Endpoint Index: ",u8EndpointNumber);
                pu8Buffer = UsbHost_GetUsbEndpointDescriptor(pu8Configuration,u32Length,u8InterfaceNumber,u8EndpointNumber);
                if (pu8Buffer[2] & USB_IN_DIRECTION)
                {
                    u8EndpointIN = pu8Buffer[2];
                    u16MaxPackageSizeIN = (uint16_t)(pu8Buffer[4] + (pu8Buffer[5] << 8));
                    u8Interval = pu8Buffer[6]; 
                }
                if (u8EndpointIN != 0)
                {
                    break;
                }
            }
            if (u8EndpointIN == 0)
            {
                dbg("Hm, error while resolving endpoints...");
                return FALSE;
            }
            dbgval("EP-IN:",u8EndpointIN);

            if (u8EndpointIN != 0)
            {
                UsbHost_AddHostEndpoint(&EndpointIN,u8EndpointIN,u16MaxPackageSizeIN,u8Interval,HidMouse_DataReceivedHandler);
                EndpointIN->pu8Buffer = u8MouseData;
                EndpointIN->u32DataSize = 4;
            } 
            dbgval("Endpoint-IN  Addr: ",u8EndpointIN);
            dbgval("Endpoint-IN  Size: ",u16MaxPackageSizeIN);
            break;
        }
    }     
    if (u8EndpointIN == 0)
    {
        dbg("Hm, error while resolving endpoints...");
        return FALSE;
    }
    return TRUE;                       
}

void HidMouse_SetEvent(void (*Handler)(MouseEventType_t stcEventType, MouseData_t MouseData))
{
    MouseEventHandler = Handler;
}

void HidMouse_RemoveEvent()
{
    MouseEventHandler = 0;
}
 
void HidMouse_DataReceivedHandler()
{
    MouseEventType_t tEvent = 0;
    bMouseMoved = TRUE;
    if (EndpointIN->pu8Buffer[0] != stcMouseData.tButtons)
    {
        tEvent |= MOUSEEVENT_BUTTON_CHANGED;
    }
    if (EndpointIN->pu8Buffer[1] != 0)
    {
        tEvent |= MOUSEEVENT_POSITIONX_CHANGED;
    }
    if (EndpointIN->pu8Buffer[2] != 0)
    {
        tEvent |= MOUSEEVENT_POSITIONY_CHANGED;
    }
    if (EndpointIN->pu8Buffer[3] != 0)
    {
        tEvent |= MOUSEEVENT_SCROLLING_CHANGED;
    }
    
    if (((int8_t)EndpointIN->pu8Buffer[1]) > 0)
    {
        if (((int8_t)EndpointIN->pu8Buffer[1]) > (0xFFFFFFFF - stcMouseData.stcPosition.u32X))
        {
            stcMouseData.stcPosition.u32X = 0xFFFFFFFF;
        }
        else
        {
            stcMouseData.stcPosition.u32X += ((int8_t)EndpointIN->pu8Buffer[1]);
        }
    }
    else if (((int8_t)EndpointIN->pu8Buffer[1]) < 0)
    {
        if ((-((int8_t)EndpointIN->pu8Buffer[1])) > stcMouseData.stcPosition.u32X)
        {
            stcMouseData.stcPosition.u32X = 0;
        }
        else
        {
            stcMouseData.stcPosition.u32X += ((int8_t)EndpointIN->pu8Buffer[1]);
        }
    }
    
    if (((int8_t)EndpointIN->pu8Buffer[2]) > 0)
    {
        if (((int8_t)EndpointIN->pu8Buffer[2]) > (0xFFFFFFFF - stcMouseData.stcPosition.u32Y))
        {
            stcMouseData.stcPosition.u32Y = 0xFFFFFFFF;
        }
        else
        {
            stcMouseData.stcPosition.u32Y += ((int8_t)EndpointIN->pu8Buffer[2]);
        }
    }
    else if (((int8_t)EndpointIN->pu8Buffer[2]) < 0)
    {
        if ((-((int8_t)EndpointIN->pu8Buffer[2])) > stcMouseData.stcPosition.u32Y)
        {
            stcMouseData.stcPosition.u32Y = 0;
        }
        else
        {
            stcMouseData.stcPosition.u32Y += ((int8_t)EndpointIN->pu8Buffer[2]);
        }
    }
    stcMouseData.tButtons = EndpointIN->pu8Buffer[0];
    stcMouseData.i8Scrolling = (int8_t)(EndpointIN->pu8Buffer[3]);
    
    if (stcMouseData.i8Scrolling > 0)
    {
        stcMouseData.u32ScrollPosition++;
    }
    if (stcMouseData.i8Scrolling < 0)
    {
        stcMouseData.u32ScrollPosition--;
    }

    HidDataCopy(4,EndpointIN->pu8Buffer);

    if ((tEvent) && (MouseEventHandler))
    {
        MouseEventHandler(tEvent,stcMouseData);
    }
    EndpointIN->CompletionHandler = HidMouse_DataReceivedHandler;
}

Point_t HidMouse_GetCurrentPosition()
{
    return stcMouseData.stcPosition;
}

void HidMouse_SetCurrentPosition(uint32_t u32X, uint32_t u32Y)
{
    stcMouseData.stcPosition.u32X = u32X;
    stcMouseData.stcPosition.u32Y = u32Y;
}

uint32_t HidMouse_GetCurrentScrollPosition()
{
    return stcMouseData.u32ScrollPosition;
}

void HidMouse_SetCurrentScrollPosition(uint32_t u32ScrollPosition)
{
    stcMouseData.u32ScrollPosition = u32ScrollPosition;
}

Buttons_t HidMouse_GetButtons()
{
    return stcMouseData.tButtons;
}

boolean_t HidMouse_Moved()
{
    if (bMouseMoved)
    {
        bMouseMoved = FALSE;
        return TRUE;
    }
    return FALSE;
}



boolean_t HidMouseDeinitHandler()
{
    return TRUE;
}

void HidMouse_Init()
{

}

#endif /* (USE_USB_HOST == 1) */
