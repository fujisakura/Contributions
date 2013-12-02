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
 ** - pre release for a simple universal usb function library
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX,FR80)
 **   - 2011-03-30    1.1  MSc  New HW description style
 *****************************************************************************/

#ifndef __USBFUNCTIONHW_H__
#define __USBFUNCTIONHW_H__

#define DEVICEVBUS_DISABLEISR bFM3_EXTI_ENIR_EN15 = 0
#define DEVICEVBUS_ENABLEISR bFM3_EXTI_ENIR_EN15 = 1
#define DEVICEVBUS_ISRISSET bFM3_EXTI_EIRR_ER15 == 1
#define DEVICEVBUS_CLEARISRFLAG bFM3_EXTI_EICL_ECL15 = 0
#define DEVICEVBUS_SETLOWDETECT bFM3_EXTI_ELVR_LA15 = 0
#define DEVICEVBUS_SETHIGHDETECT bFM3_EXTI_ELVR_LA15 = 1
#define DEVICEVBUS_HIGHDETECT (FM3_GPIO->PDIR6 & 0x01) > 0
#define DEVICEVBUS_INIT bFM3_GPIO_PFR6_P0  = 1; bFM3_GPIO_DDR6_P0 = 0; bFM3_GPIO_EPFR06_EINT15S1 = 1; NVIC_ClearPendingIRQ(EXINT8_15_IRQn); NVIC_EnableIRQ(EXINT8_15_IRQn); NVIC_SetPriority(EXINT8_15_IRQn,1)
#define DEVICEVBUS_ENABLED 1

#define DEVICEHCONX_INIT 
#define DEVICEHCONX_SET 
#define DEVICEHCONX_CLEAR 
#define DEVICEHCONX_ENABLED 1


#endif
