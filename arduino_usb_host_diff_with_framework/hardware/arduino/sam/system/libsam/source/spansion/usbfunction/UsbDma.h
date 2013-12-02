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
/** \file UsbDma.h
 **
 ** - See README.TXT for project description
 ** - pre release for a simple universal usb function library
 **
 ** History:
 **   - 2010-04-30    1.0  MSc  Seperate DMA module for USB
 *****************************************************************************/

#ifndef __USBDMA_H__
#define __USBDMA_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "../base_type.h"
//#include "mcu.h"
#include "UsbFunction.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define MCU_16FX 1
#define MCU_FR80 2
#if (CPU_TYPE == CPU_TYPE_16BIT)
    #define USB_MCU  MCU_16FX
#else
    #define USB_MCU  MCU_FR80
    #define BIT_DCCR_CE      (1 << 31)
    #define BIT_DCCR_AIE     (1 << 26)
    #define BIT_DCCR_SIE     (1 << 25)
    #define BIT_DCCR_NIE     (1 << 24)
    #define BIT_DCCR_RS1     (1 << 21)
    #define BIT_DCCR_RS0     (1 << 20)
    #define BITS_DCCR_RSUSB  (BIT_DCCR_RS1 | BIT_DCCR_RS0)
    #define BIT_DCCR_TM1     (1 << 17)
    #define BIT_DCCR_TM0     (1 << 16)
    #define BITS_DCCR_TMUSB  BIT_DCCR_TM0
    #define BIT_DCCR_ST      (1 << 15)
    #define BIT_DCCR_SAR     (1 << 14)
    #define BIT_DCCR_SAC1    (1 << 13)
    #define BIT_DCCR_SAC0    (1 << 12)
    #define BITS_DCCR_SACUSB (BIT_DCCR_SAC1 | BIT_DCCR_SAC0)
    #define BIT_DCCR_DT      (1 << 11)
    #define BIT_DCCR_DAR     (1 << 10)
    #define BIT_DCCR_DAC1    (1 <<  9)
    #define BIT_DCCR_DAC0    (1 <<  8)
    #define BIT_DCCR_TCR     (1 <<  7)
    #define BIT_DCCR_TS1     (1 <<  5)
    #define BIT_DCCR_TS0     (1 <<  4)
    #define BITS_DCCR_TSUSB  BIT_DCCR_TS0
    #define BIT_DCCR_BLK3    (1 <<  3)
    #define BIT_DCCR_BLK2    (1 <<  2)
    #define BIT_DCCR_BLK1    (1 <<  1)
    #define BIT_DCCR_BLK0    (1 <<  0)
#endif

#define DMAHANDLER(x)  (DmaInfo[GET_ENDPOINT(x).u8DmaNumber])

//#if (USB_USE_DMA == 1)
    #define DMA_OFFSET_BAPL  0
    #define DMA_OFFSET_BAPM  1
    #define DMA_OFFSET_BAPH  2
    #define DMA_OFFSET_DMACS 3
    #define DMA_OFFSET_IOAL  4
    #define DMA_OFFSET_IOAH  5
    #define DMA_OFFSET_DCTL  6
    #define DMA_OFFSET_DCTH  7
    #define DMA_DIR_HOST_OUT 0
    #define DMA_DIR_HOST_IN  1
//#endif
/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

#if (USB_MCU == MCU_16FX)
    typedef struct stc_dma_handler_16fx 
    {
        uint8_t u8DmaNumber;
        volatile uint8_t * DISELn;
        volatile uint8_t * pu8DmaDescriptor;
    } dma_handler_16fx_t;
#endif
#if (USB_MCU == MCU_FR80)
    typedef struct stc_dma_handler_fr80 
    {
        uint8_t u8DmaNumber;
        volatile uint32_t * DSARn;
        volatile uint32_t * DDARn;
        volatile uint16_t * DTCRn;
        volatile uint32_t * DCCRn;
        volatile uint16_t * DCSRn;
        volatile uint8_t    DREQSEL_MASK;
    } dma_handler_fr80_t;
#endif
/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/


#if (USB_USE_DMA == 1)
    uint16_t UsbDma_Status(uint8_t u8EndpointNumber);
    void UsbDma_SetupDma(uint8_t u8EndpointNumber,uint8_t u8InterruptNumber, uint16_t u16DataSize);
    void UsbDma_SendReceiveBufferViaDma(uint8_t u8EndpointNumber);
    void UsbDma_ReleaseDma(uint8_t u8EndpointNumber);
    void UsbDma_ChangeDmaSettings(uint8_t u8EndpointNumber, uint8_t u8DmaNumber);
    void UsbDma_EnableReceiveDma(uint8_t u8EndpointNumber, uint32_t u32ReceiveSize);
    __interrupt void UsbDma_DmaEndedIsr(void);
#endif


#endif
