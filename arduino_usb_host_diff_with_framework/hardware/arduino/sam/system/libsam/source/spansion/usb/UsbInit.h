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

/** \file UsbInit.h
 **
 ** USB Registers File
 **
 ** History:
 **   - 2011-02-25  1.0  MSc  MCU Specific initializations FR80, FM3
 **   - 2011-03-30  1.1  MSc  Some Corrections
 **   - 2011-04-28  1.2  MSc  Support for MB91F662 template
 **   - 2011-08-12  1.3  MSc  Support for MB9AF3xx series
 **   - 2011-11-14  1.4  MSc  Support for MB9BF61x series
 **   - 2012-01-18  1.5  MSc  Support for MB9BF51x series
 **   - 2012-02-03  1.6  MSc  Support for MB9BF31xK series
 *************************************************************************/

#ifndef __USBINIT_H__
#define __USBINIT_H__
#ifndef INCLUDE_FROM_CHIP_H
#include "mcu.h"
#endif
#include "../base_type.h"
#include "../usbfunction/UsbFunction.h"

#define USBMCU_MB96330    1
#define USBMCU_MB91660    2
#define USBMCU_MB91665    3
#define USBMCU_MB9B500    4
#define USBMCU_MB9A310    5
#define USBMCU_MB9B610    6
#define USBMCU_MB9B510    7

#define WORKSPACE_SOFTUNE 1
#define WORKSPACE_IAR     2
#define WORKSPACE_KEIL    3


#if ((__TID__ >> 4) & 0x0F) == 6   // Cortex-M3
    
#endif

#if __CC_ARM == 1
    #define USED_WORKSPACE WORKSPACE_KEIL  
#endif

#if defined(_MB9AF316N_H_) || defined(_MB9AF315N_H_) || defined(_MB9AF314L_H_) || defined(_MB9AF316L_H_) || defined(_MB9AF315L_H_)
    #define USBMCUTYPE USBMCU_MB9A310
#endif

#if defined(_MB9AF314L_H_) || defined(_MB9A310K_H_)
    #define USBMCUTYPE USBMCU_MB9A310
    #ifndef USE_USB0
        #define USE_USB0
    #endif
#endif

#if defined(_MB9BF506N_H_) || defined(_MB9BF505N_H_) || defined(_MB9BF504R_H_) || defined(_MB9BF506R_H_) || defined(_MB9BF505R_H_) || defined(_MB9BF504R_H_)
    #define USBMCUTYPE USBMCU_MB9B500
#endif

#ifndef _MB9B610T_H_
#define _MB9B610T_H_
#endif

#if defined(_MB9B610T_H_)
    #ifndef USE_USB1
        #define USE_USB1
    #endif
    #define USBMCUTYPE USBMCU_MB9B610
    #define __IAR_SYSTEMS_ICC__
#endif

#if defined(_MB9B510N_H_)
    #ifndef USE_USB0
        #define USE_USB0
    #endif
    #define USBMCUTYPE USBMCU_MB9B510
#endif

#define IS_USBMCU_FM3  ((USBMCUTYPE == USBMCU_MB9B500) || (USBMCUTYPE == USBMCU_MB9A310) || (USBMCUTYPE == USBMCU_MB9B610) || (USBMCUTYPE == USBMCU_MB9B510))

#ifdef __IAR_SYSTEMS_ICC__
    #define USED_WORKSPACE WORKSPACE_IAR
#endif

#if defined(__CPU_MB96F336USA__) || defined(__CPU_MB96F338USA__) || defined(__CPU_MB96F336UWA__) || defined(__CPU_MB96F338UWA__)
    #if CPU_TYPE != CPU_TYPE_16BIT
       #line 60 "mcu.h"
       #error The MB96330 series MCU is a 16-Bit MCU. Please define in mcu.h the CPU_TYPE as CPU_TYPE_16BIT
    #endif
    #define USBMCUTYPE USBMCU_MB96330
    #define USED_WORKSPACE WORKSPACE_SOFTUNE
#endif
    
#if defined(__CPU_MB91F662__) || defined(__CPU_MB91F661__) 
    #if CPU_TYPE != CPU_TYPE_32BIT
       #line 60 "mcu.h"
       #error The MB91660 series MCU is a 32-Bit MCU. Please define in mcu.h the CPU_TYPE as CPU_TYPE_32BIT
    #endif
    #define USBMCUTYPE USBMCU_MB91660
    #define USED_WORKSPACE WORKSPACE_SOFTUNE
#endif
    
#if defined(__CPU_MB91F668__) || defined(__CPU_MB91669__) 
    #if CPU_TYPE != CPU_TYPE_32BIT
       #line 60 "mcu.h"
       #error The MB91665 series MCU is a 32-Bit MCU. Please define in mcu.h the CPU_TYPE as CPU_TYPE_32BIT
    #endif
    #define USBMCUTYPE USBMCU_MB91665
    #define USED_WORKSPACE WORKSPACE_SOFTUNE
#endif

#ifndef USBMCUTYPE
    #error USB Stack does not recognize the used MCU
#endif

#ifndef USED_WORKSPACE
    #error USB Stack does not recognize the used Workspace
#endif

#if (USED_WORKSPACE != WORKSPACE_SOFTUNE) 
    #ifndef __interrupt
        #define __interrupt
    #endif
    #ifndef __wait_nop
        #define __wait_nop()
    #endif
#endif


#if (USBMCUTYPE == USBMCU_MB91665) || (USBMCUTYPE == USBMCU_MB91660)
  #define UsbInit() UsbInit_FR80()
  void UsbInit_FR80(void);
#endif

#if IS_USBMCU_FM3
  #define UsbInit() UsbInit_FM3()
  void UsbInit_FM3(void);
#endif

#if ((USE_USB_FUNCTION == 1) && (USE_USB_HOST == 1))
  uint8_t UsbInit_HostFunctionSwitch(struct stc_usb_event* stcEvent);
  void UsbInit_SetUsbHostFunctionSwitching();
#endif

__interrupt void UsbInit_HostFunctionIsr(void);
  #if (USBMCUTYPE == USBMCU_MB91665)
        #define EP0DTH EP0DT_EP0DTH
        #define EP0DTL EP0DT_EP0DTL
        #define EP1DTH EP1DT_EP1DTH
        #define EP1DTL EP1DT_EP1DTL
        #define EP2DTH EP2DT_EP2DTH
        #define EP2DTL EP2DT_EP2DTL
        #define EP3DTH EP3DT_EP3DTH
        #define EP3DTL EP3DT_EP3DTL
        #define UDCC UDCC_
  #endif
  #if (USBMCUTYPE == USBMCU_MB91660)
        #define EP0DTH EP0DT_EP0DTH
        #define EP0DTL EP0DT_EP0DTL
        #define EP1DTH EP1DT_EP1DTH
        #define EP1DTL EP1DT_EP1DTL
        #define EP2DTH EP2DT_EP2DTH
        #define EP2DTL EP2DT_EP2DTL
        #define EP3DTH EP3DT_EP3DTH
        #define EP3DTL EP3DT_EP3DTL
        #define EP4DTH EP4DT_EP4DTH
        #define EP4DTL EP4DT_EP4DTL
        #define EP5DTH EP5DT_EP5DTH
        #define EP5DTL EP5DT_EP5DTL
        #define UDCC UDCC_
  #endif
  #if IS_USBMCU_FM3  
        #define UDCC FM3_USB1->UDCC
        #define EP0C FM3_USB1->EP0C
        #define EP1C FM3_USB1->EP1C
        #define EP2C FM3_USB1->EP2C
        #define EP3C FM3_USB1->EP3C
        #define EP4C FM3_USB1->EP4C
        #define EP5C FM3_USB1->EP5C
        #define TMSP FM3_USB1->TMSP
        #define UDCS FM3_USB1->UDCS
        #define UDCIE FM3_USB1->UDCIE
        #define EP0IS FM3_USB1->EP0IS
        #define EP0OS FM3_USB1->EP0OS
        #define EP1S FM3_USB1->EP1S
        #define EP2S FM3_USB1->EP2S
        #define EP3S FM3_USB1->EP3S
        #define EP4S FM3_USB1->EP4S
        #define EP5S FM3_USB1->EP5S
        #define EP0DT  FM3_USB1->EP0DT
        #define EP0DTL FM3_USB1->EP0DTL
        #define EP0DTH FM3_USB1->EP0DTH
        #define EP1DT  FM3_USB1->EP1DT
        #define EP1DTL FM3_USB1->EP1DTL
        #define EP1DTH FM3_USB1->EP1DTH
        #define EP2DT  FM3_USB1->EP2DT
        #define EP2DTL FM3_USB1->EP2DTL
        #define EP2DTH FM3_USB1->EP2DTH
        #define EP3DT  FM3_USB1->EP3DT
        #define EP3DTL FM3_USB1->EP3DTL
        #define EP3DTH FM3_USB1->EP3DTH
        #define EP4DT  FM3_USB1->EP4DT
        #define EP4DTL FM3_USB1->EP4DTL
        #define EP4DTH FM3_USB1->EP4DTH
        #define EP5DT  FM3_USB1->EP5DT
        #define EP5DTL FM3_USB1->EP5DTL
        #define EP5DTH FM3_USB1->EP5DTH
        #define HCNT  FM3_USB1->HCNT
        #define HIRQ  FM3_USB1->HIRQ
        #define HERR  FM3_USB1->HERR
        #define HSTATE  FM3_USB1->HSTATE
        #define HFCOMP  FM3_USB1->HFCOMP
        #define HRTIMER0  FM3_USB1->HRTIMER0
        #define HRTIMER1  FM3_USB1->HRTIMER1
        #define HRTIMER2  FM3_USB1->HRTIMER2
        #define HADR  FM3_USB1->HADR
        #define HEOF (FM3_USB1->HEOF)
        #define HFRAME  FM3_USB1->HFRAME
        #define HTOKEN  FM3_USB1->HTOKEN

  #endif

  #if (USBMCUTYPE == USBMCU_MB96330)
        #define UDCC UDCC0
        #define EP0C EP0C0
        #define EP1C EP1C0
        #define EP2C EP2C0
        #define EP3C EP3C0
        #define EP4C EP4C0
        #define EP5C EP5C0
        #define TMSP TMSP0
        #define UDCS UDCS0
        #define UDCIE UDCIE0
        #define EP0IS EP0IS0
        #define EP0OS EP0OS0
        #define EP1S EP1S0
        #define EP2S EP2S0
        #define EP3S EP3S0
        #define EP4S EP4S0
        #define EP5S EP5S0
        #define EP0DT EP0DT0
        #define EP0DTL EP0DTL0
        #define EP0DTH EP0DTH0
        #define EP1DT EP1DT0
        #define EP1DTL EP1DTL0
        #define EP1DTH EP1DTH0
        #define EP2DT EP2DT0
        #define EP2DTL EP2DTL0
        #define EP2DTH EP2DTH0
        #define EP3DT EP3DT0
        #define EP3DTL EP3DTL0
        #define EP3DTH EP3DTH0
        #define EP4DT EP4DT0
        #define EP4DTL EP4DTL0
        #define EP4DTH EP4DTH0
        #define EP5DT EP5DT0
        #define EP5DTL EP5DTL0
        #define EP5DTH EP5DTH0
        #define HCNT HCNT0
        #define HIRQ HIRQ0
        #define HERR HERR0
        #define HSTATE HSTATE0
        #define HFCOMP HFCOMP0
        #define HRTIMER0 HRTIMERL0
        #define HRTIMER1 HRTIMERM0
        #define HRTIMER2 HRTIMERH0
        #define HADR HADR0
        #define HEOF HEOF0
        #define HFRAME HFRAME0
        #define HTOKEN HTOKEN0

  #endif

#define _UDCC_RST	(1 << 7) // Function reset bit
#define _UDCC_RESUM	(1 << 6) // Resume setting bit
#define _UDCC_HCONX	(1 << 5) // Host connection bit
#define _UDCC_USTP	(1 << 4) // USB operation clock stop bit
#define _UDCC_RFBK	(1 << 1) // Data toggle mode bit (Rate feedback mode)
#define _UDCC_PWC	(1 << 0) // Power supply control bit

#define _EP0C_PKS0	0x7F     // EP0 packet size setting bits
#define _EP0C_STAL  (1 << 9) //STALL set bit

#define _EP1C_PKS1	0x1FF     // EP1 packet size setting bits
#define _EP1C_STAL  (1 << 9)  // STALL set bit
#define _EP1C_NULE  (1 << 10) // NULL automatic transfer enable bit
#define _EP1C_DMAE  (1 << 11) // DMA automatic transfer enable bit
#define _EP1C_DIR   (1 << 12) // Endpoint direction selection bit
#define _EP1C_TYPE  0x6000    // Endpoint transfer type bit
#define _EP1C_EPEN  (1 << 15) // Endpoint Enable bit

#define _EP2C_PKS2	0xFF      // EP2 packet size setting bits
#define _EP2C_STAL  (1 << 9)  // STALL set bit
#define _EP2C_NULE  (1 << 10) // NULL automatic transfer enable bit
#define _EP2C_DMAE  (1 << 11) // DMA automatic transfer enable bit
#define _EP2C_DIR   (1 << 12) // Endpoint direction selection bit
#define _EP2C_TYPE  0x6000    // Endpoint transfer type bit
#define _EP2C_EPEN  (1 << 15) // Endpoint Enable bit

#define _EP3C_PKS3	0xFF      // EP3 packet size setting bits
#define _EP3C_STAL  (1 << 9)  // STALL set bit
#define _EP3C_NULE  (1 << 10) // NULL automatic transfer enable bit
#define _EP3C_DMAE  (1 << 11) // DMA automatic transfer enable bit
#define _EP3C_DIR   (1 << 12) // Endpoint direction selection bit
#define _EP3C_TYPE  0x6000    // Endpoint transfer type bit
#define _EP3C_EPEN  (1 << 15) // Endpoint Enable bit

#define _EP4C_PKS4	0xFF      // EP4 packet size setting bits
#define _EP4C_STAL  (1 << 9)  // STALL set bit
#define _EP4C_NULE  (1 << 10) // NULL automatic transfer enable bit
#define _EP4C_DMAE  (1 << 11) // DMA automatic transfer enable bit
#define _EP4C_DIR   (1 << 12) // Endpoint direction selection bit
#define _EP4C_TYPE  0x6000    // Endpoint transfer type bit
#define _EP4C_EPEN  (1 << 15) // Endpoint Enable bit

#define _EP5C_PKS5	0xFF      // EP  packet size setting bits
#define _EP5C_STAL  (1 << 9)  // STALL set bit
#define _EP5C_NULE  (1 << 10) // NULL automatic transfer enable bit
#define _EP5C_DMAE  (1 << 11) // DMA automatic transfer enable bit
#define _EP5C_DIR   (1 << 12) // Endpoint direction selection bit
#define _EP5C_TYPE  0x6000    // Endpoint transfer type bit
#define _EP5C_EPEN  (1 << 15) // Endpoint Enable bit

#define _EPC_TYPE_BULK       0x4000
#define _EPC_TYPE_INTERRUPT  0x6000

#define _UDCS_CONF	(1 << 0) // Configuration detection bit
#define _UDCS_SETP	(1 << 1) // Setup stage detection bit
#define _UDCS_WKUP	(1 << 2) // Wake-up detection bit
#define _UDCS_BRST	(1 << 3) // Bus reset detection bit
#define _UDCS_SOF	(1 << 4) // SOF reception detection bit
#define _UDCS_SUSP	(1 << 5) // Suspend detection bit

#define _UDCIE_CONFIE (1 << 0) // Configuration interrupt enable bit
#define _UDCIE_CONFN	 (1 << 1) // Configuration number bit
#define _UDCIE_WKUPIE (1 << 2) // Wake-up interrupt enable bit
#define _UDCIE_BRSTIE (1 << 3) // Bus reset interrupt enable bit
#define _UDCIE_SOFIE  (1 << 4) // SOF receive interrupt enable bit
#define _UDCIE_SUSPIE (1 << 5) // Suspend interrupt enable bit

#define _EP0IS_DRQI   (1 << 10) // Transmit Data interrupt request bit
#define _EP0IS_DRQIIE (1 << 14) // Transmit Data Interrupt enable bit
#define _EP0IS_BFINI  (1 << 15) // Transmission buffer initialization bit

#define _EP0OS_SIZE        0x7F  // Packet size bits
#define _EP0OS_SPK    (1 <<  9)  // Short Packet interrupt flag bit
#define _EP0OS_DRQO   (1 << 10)  // Received data interrupt flag bit
#define _EP0OS_SPKIE  (1 << 13)  // Short packet interrupt enable bit
#define _EP0OS_DRQOIE (1 << 14)  // Receive Data Interrupt enable bit
#define _EP0OS_BFINI  (1 << 15)  // Receive buffer initialization bit

#define _EP1S_SIZE        0x1FF  // Packet size bits
#define _EP1S_SPK     (1 <<  9)  // Short Packet interrupt flag bit
#define _EP1S_DRQ     (1 << 10)  // Packet transfer interrupt flag bit
#define _EP1S_BUSY    (1 << 11)  // Busy flag bit
#define _EP1S_SPKIE   (1 << 13)  // Short packet interrupt enable bit
#define _EP1S_DRQIE   (1 << 14)  // DRQ Interrupt enable bit
#define _EP1S_BFINI   (1 << 15)  // Transmission/Reception buffer initialization bit

#define _EP2S_SIZE         0xFF  // Packet size bits
#define _EP2S_SPK     (1 <<  9)  // Short Packet interrupt flag bit
#define _EP2S_DRQ     (1 << 10)  // Packet transfer interrupt flag bit
#define _EP2S_BUSY    (1 << 11)  // Busy flag bit
#define _EP2S_SPKIE   (1 << 13)  // Short packet interrupt enable bit
#define _EP2S_DRQIE   (1 << 14)  // DRQ Interrupt enable bit
#define _EP2S_BFINI   (1 << 15)  // Transmission/Reception buffer initialization bit

#define _EP3S_SIZE         0xFF  // Packet size bits
#define _EP3S_SPK     (1 <<  9)  // Short Packet interrupt flag bit
#define _EP3S_DRQ     (1 << 10)  // Packet transfer interrupt flag bit
#define _EP3S_BUSY    (1 << 11)  // Busy flag bit
#define _EP3S_SPKIE   (1 << 13)  // Short packet interrupt enable bit
#define _EP3S_DRQIE   (1 << 14)  // DRQ Interrupt enable bit
#define _EP3S_BFINI   (1 << 15)  // Transmission/Reception buffer initialization bit

#define _EP4S_SIZE         0xFF  // Packet size bits
#define _EP4S_SPK     (1 <<  9)  // Short Packet interrupt flag bit
#define _EP4S_DRQ     (1 << 10)  // Packet transfer interrupt flag bit
#define _EP4S_BUSY    (1 << 11)  // Busy flag bit
#define _EP4S_SPKIE   (1 << 13)  // Short packet interrupt enable bit
#define _EP4S_DRQIE   (1 << 14)  // DRQ Interrupt enable bit
#define _EP4S_BFINI   (1 << 15)  // Transmission/Reception buffer initialization bit

#define _EP5S_SIZE         0xFF  // Packet size bits
#define _EP5S_SPK     (1 <<  9)  // Short Packet interrupt flag bit
#define _EP5S_DRQ     (1 << 10)  // Packet transfer interrupt flag bit
#define _EP5S_BUSY    (1 << 11)  // Busy flag bit
#define _EP5S_SPKIE   (1 << 13)  // Short packet interrupt enable bit
#define _EP5S_DRQIE   (1 << 14)  // DRQ Interrupt enable bit
#define _EP5S_BFINI   (1 << 15)  // Transmission/Reception buffer initialization bit

#define _HCNT_HOST    (1 <<  0) // HOST Mode bit
#define _HCNT_URST	  (1 <<  1) // USB Bus Reset bit
#define _HCNT_SOFIRE  (1 <<  2) // SOF interrupt Enable bit
#define _HCNT_DIRE    (1 <<  3) // Disconnection interrupt Enable bit
#define _HCNT_CNNIRE  (1 <<  4) // Connection interrupt Enable bit
#define _HCNT_CMPIRE  (1 <<  5) // Completion interrupt Enable bit
#define _HCNT_URIRE   (1 <<  6) // USB bus Reset interrupt Enable bit
#define _HCNT_RWKIRE  (1 <<  7) // Remote Wake-up interrupt Enable bit
#define _HCNT_RETRY   (1 <<  8) // Retry enable bit
#define _HCNT_CANCEL  (1 <<  9) // Token cancel enable bit
#define _HCNT_SOFSTEP (1 << 10) // SOF interrupt trigger bit

#define _HIRQ_SOFIRQ  (1 <<  0) // HOST Mode bit
#define _HIRQ_DIRQ	  (1 <<  1) // USB Bus Reset bit
#define _HIRQ_CNNIRQ  (1 <<  2) // SOF interrupt Enable bit
#define _HIRQ_CMPIRQ  (1 <<  3) // Disconnection interrupt Enable bit
#define _HIRQ_URIRQ   (1 <<  4) // Connection interrupt Enable bit
#define _HIRQ_RWKIRQ  (1 <<  5) // Completion interrupt Enable bit
#define _HIRQ_TCAN    (1 <<  7) // Remote Wake-up interrupt Enable bit

#define _HERR_HS           0x03 // Handshake Status
#define _HERR_STUFF	  (1 <<  2) // Stuffing Error bit
#define _HERR_TGERR   (1 <<  3) // Toggle Error bit
#define _HERR_CRC     (1 <<  4) // CRC Error bit
#define _HERR_TOUT    (1 <<  5) // Time Out bit
#define _HERR_RERR    (1 <<  6) // Receive Error bit
#define _HERR_LSTSOF  (1 <<  7) // Lost SOF bit

#define _HSTATE_CSTAT   (1 <<  0) // Connection status bit
#define _HSTATE_TMODE	(1 <<  1) // Transfer Mode bit
#define _HSTATE_SUSP    (1 <<  2) // Suspend bit
#define _HSTATE_SOFBUSY (1 <<  3) // SOF timer operation bit
#define _HSTATE_CLKSEL  (1 <<  4) // Clock Selection bit (Full / Low Speed)
#define _HSTATE_ALIVE   (1 <<  5) // keep alive bit (for low speed)

#endif


