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
/** \file UsbHostHW.h
 **
 ** USB hardware specific settings
 **
 ** History:
 **   - 2010-10-14    1.0  MSc  First version  (works with 16FX,FR80)
 **   - 2011-03-30    1.1  MSc  Compatible with Spansion LLC Host drivers 2011-03-30
 *****************************************************************************/

#ifndef __USBHOSTHW_H__
#define __USBHOSTHW_H__


#define HOSTVBUS_INIT FM3_GPIO->DDRF |= (1 << 5)
#define HOSTVBUS_SET FM3_GPIO->PDORF |= (1 << 5)
#define HOSTVBUS_CLEAR FM3_GPIO->PDORF &= ~(1 << 5)
#define HOSTVBUS_ENABLED 1

#define HOSTOTGPULLDOWN_INIT FM3_GPIO->DDR1 |= (1 << 0x02)
#define HOSTOTGPULLDOWN_SET FM3_GPIO->PDOR1 |= (1 << 0x02)
#define HOSTOTGPULLDOWN_CLEAR FM3_GPIO->PDOR1 &= ~(1 << 0x02)
#define HOSTOTGPULLDOWN_ENABLED 1

#define HOSTOVERCURRENT_DISABLEISR bFM3_EXTI_ENIR_EN2 = 0
#define HOSTOVERCURRENT_ENABLEISR bFM3_EXTI_ENIR_EN2 = 1
#define HOSTOVERCURRENT_CLEARISRFLAG bFM3_EXTI_EICL_ECL2 = 0
#define HOSTOVERCURRENT_ISRISSET bFM3_EXTI_EIRR_ER2 == 1
#define HOSTOVERCURRENT_SETLOWDETECT bFM3_EXTI_ELVR_LA2 = 0
#define HOSTOVERCURRENT_SETHIGHDETECT bFM3_EXTI_ELVR_LA2 = 1
#define HOSTOVERCURRENT_HIGHDETECT (FM3_GPIO->PDIR1 & 0x02) > 0
#define HOSTOVERCURRENT_INIT FM3_GPIO->DDR1 &= ~0x02
#define HOSTOVERCURRENT_ENABLED 1


#endif
