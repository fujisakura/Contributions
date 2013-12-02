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
/** \file UsbSofTimeout.c
 **
 ** USB Registers File
 **
 ** History:
 **   - 2010-09-24  1.0  MSc  USB Sof Timeout Feature
 **   - 2011-03-30  1.0  MSc  Some Corrections
 *************************************************************************/

#include "UsbSofTimeout.h"
#include "dbg.h"

void (** pUsbSOFHandler)(uint16_t u16FrameNumber) = 0;
stc_timeout_handler_t stcUsbTimeOutHandler[10] = 
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

void Usb_SetSOFTimeOutHandler(void (** pSofHandler)(uint16_t u16FrameNumber))
{
    pUsbSOFHandler = pSofHandler;
}
/**
 ******************************************************************************
 ** SOF controlled Timeout Scheduler
 ** 
 ** \param ticks SOF counter
 ** 
 *****************************************************************************/

void Usb_TimeoutHandler(uint16_t ticks)
{
    uint8_t i;
    boolean_t bNothingToBeDone = TRUE;
    ticks = ticks;
    for(i = 0;i<10;i++)
    {
        if (stcUsbTimeOutHandler[i].Handler != 0)
        {
            bNothingToBeDone = FALSE;
            if (stcUsbTimeOutHandler[i].u16TimeOut == 0)
            {
                stcUsbTimeOutHandler[i].Handler();
                stcUsbTimeOutHandler[i].Handler = 0;
                return;
            }
            else
            {
                stcUsbTimeOutHandler[i].u16TimeOut--;
            }
        }
    }
    if (bNothingToBeDone == TRUE)
    {
         *pUsbSOFHandler = 0;
         dbg("Not Timouthandlers, removing SOF Handler\n");
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
 
boolean_t Usb_AddTimeOut(void (* Handler)(void), uint16_t u16TimeOut)
{
    uint8_t i;
    boolean_t bAdded = FALSE;
    for(i = 0;i<10;i++)
    {
        if (stcUsbTimeOutHandler[i].Handler == 0)
        {
            stcUsbTimeOutHandler[i].Handler = Handler;
            stcUsbTimeOutHandler[i].u16TimeOut = u16TimeOut;
            bAdded = TRUE;
            break;
        }
    }
    *pUsbSOFHandler = Usb_TimeoutHandler;
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
 
void Usb_RemoveTimeOut(void (* Handler)(void))
{
    uint8_t i;
    for(i = 0;i<10;i++)
    {
        if (stcUsbTimeOutHandler[i].Handler == Handler)
        {
            stcUsbTimeOutHandler[i].Handler = 0;
            stcUsbTimeOutHandler[i].u16TimeOut = 0;
            break;
        }
    }
}