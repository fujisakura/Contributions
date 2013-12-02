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
/** \file UsbDma.c
 **
 ** - See README.TXT for project description
 ** - pre release for a simple universal usb function library
 **
 ** History:
 **   - 2010-04-30    1.0  MSc  Seperate DMA module for USB
 *****************************************************************************/
#include "UsbDma.h"

#if (USB_USE_DMA == 1)

#if (USB_MCU == MCU_16FX)
    dma_handler_16fx_t DmaInfo[] = {
        {0,&DISEL0,(volatile uint8_t*)0x00100},
        {1,&DISEL1,(volatile uint8_t*)0x00108},
        {2,&DISEL2,(volatile uint8_t*)0x00110},
        {3,&DISEL3,(volatile uint8_t*)0x00118},
        {4,&DISEL4,(volatile uint8_t*)0x00120},
        {5,&DISEL5,(volatile uint8_t*)0x00128},
        {6,&DISEL6,(volatile uint8_t*)0x00130},
        {7,&DISEL7,(volatile uint8_t*)0x00138},
        {8,&DISEL8,(volatile uint8_t*)0x00140},
        {9,&DISEL9,(volatile uint8_t*)0x00148}
    };
#endif
#if (USB_MCU == MCU_FR80)
    dma_handler_fr80_t DmaInfo[] = {
        {1,&DMAC_DSAR3,&DMAC_DDAR3,&DMAC_DTCR3,&DMAC_DCCR3,&DMAC_DCSR3,0x08},
        {2,&DMAC_DSAR0,&DMAC_DDAR0,&DMAC_DTCR0,&DMAC_DCCR0,&DMAC_DCSR0,0x01},
        {3,&DMAC_DSAR1,&DMAC_DDAR1,&DMAC_DTCR1,&DMAC_DCCR1,&DMAC_DCSR1,0x02},
        {4,&DMAC_DSAR4,&DMAC_DDAR4,&DMAC_DTCR4,&DMAC_DCCR4,&DMAC_DCSR4,0x10},
        {5,&DMAC_DSAR5,&DMAC_DDAR5,&DMAC_DTCR5,&DMAC_DCCR5,&DMAC_DCSR5,0x20},
    };
#endif

 
uint16_t UsbDma_Status(uint8_t u8EndpointNumber)
{
    if ((uint16_t)*DmaInfo[u8EndpointNumber - 1].DCSRn & 0x8000)
    {
        return ((uint16_t)*DmaInfo[u8EndpointNumber - 1].DTCRn);
    }
    else
    {
        return 0;
    }
}

#if (USB_MCU == MCU_16FX)
/**
 ******************************************************************************
 ** Set custom DMA settings
 **
 *****************************************************************************/

void UsbDma_ChangeDmaSettings(uint8_t u8EndpointNumber, uint8_t u8DmaNumber)
{
    (UsbFunction_GetEndpoint(u8EndpointNumber))->u8DmaNumber = u8DmaNumber;
}
#endif

/**
 ******************************************************************************
 ** Setup a DMA transfer for an endpoint
 **
 ** 
 ** \param u8EndpointNumber Endpointnumber (1..5) 
 **
 ** \param u8InterruptNumber (see hardware manual: INTERRUPT VECTOR TABLE, Index in ICR to program)
 **
 ** \param u16DataSize package size of DMA, 0 = 65536 bytes (maximum)
 **
 ** \return none
 **
 *****************************************************************************/
 
void UsbDma_SetupDma(uint8_t u8EndpointNumber,uint8_t u8InterruptNumber, uint16_t u16DataSize)
{
    uint32_t u32StartAt = (uint32_t)GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->pu8Buffer;
    uint8_t u8Direction = DMA_DIR_HOST_OUT;
    uint16_t temp;
    
    CLEAR_MASK(EXT_ENDPOINT_STATUS(u8EndpointNumber),MASK_EPS_DRQIE);
    
    if ((((uint16_t)EXT_ENDPOINT_CONTROL(u8EndpointNumber)) & MASK_EPC_DIR))
    {
        u8Direction = DMA_DIR_HOST_IN;
    }
    u32StartAt += GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position; //calculating start of memory address
    
    if (u16DataSize == 0)	// DMA size 0 means 0xFFFF + 1 bytes transfer, correct this in position update...
    {
        GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position += 0x10000;	// increment of position
        u16DataSize = 65536;
    } 
    else
    {
        GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position += u16DataSize;	// increment of position
    }
    
    dbg   ("DMA   |->DMA Setup\n");                            //DBUG
    dbgval("DMA      |->for Endpoint: ",   u8EndpointNumber);  //DBUG
    dbgval("DMA      |->ICR: ",            u8InterruptNumber); //DBUG
    dbgval("DMA      |->Size: ",           u16DataSize);       //DBUG
    dbgval("DMA      |->StartAt: ",        u32StartAt);        //DBUG

    GET_ENDPOINT(u8EndpointNumber).status |= EP_STATUS_DMA_TRANSFER;         //Setting DMA transfer status flag for this endpoint
    #if (USB_MCU == MCU_16FX)
        DSR &= ~(1 << DMAHANDLER(u8EndpointNumber).u8DmaNumber); //clearing DMA status flag for this DMA channel
        DER &= ~(1 << DMAHANDLER(u8EndpointNumber).u8DmaNumber); //clearing DMA interrupt flag for this DMA channel
        *(DMAHANDLER(u8EndpointNumber).DISELn) = u8InterruptNumber; //setup interrupt number for this DMA channel
        *(uint16_t*)(DMAHANDLER(u8EndpointNumber).pu8DmaDescriptor + DMA_OFFSET_DCTL) = u16DataSize;        //setup DMA transfer size
        *(DMAHANDLER(u8EndpointNumber).pu8DmaDescriptor + DMA_OFFSET_IOAH) = (uint8_t)GET_ENDPOINT(u8EndpointNumber).EPnDTL >> 8;   //setup DMA transfer IO register address (Endpoint Data Registers) higher byte
        *(DMAHANDLER(u8EndpointNumber).pu8DmaDescriptor + DMA_OFFSET_IOAL) = (uint8_t)GET_ENDPOINT(u8EndpointNumber).EPnDTL & 0xFF; //setup DMA transfer IO register address (Endpoint Data Registers) lower byte
        *(DMAHANDLER(u8EndpointNumber).pu8DmaDescriptor + DMA_OFFSET_DMACS) = 0x18 + (u8Direction << 1);   // BAP increment; IOA stay fixed; word transfer; BAP <-> IOA
        *(DMAHANDLER(u8EndpointNumber).pu8DmaDescriptor + DMA_OFFSET_BAPH) = (uint8_t)(u32StartAt >> 16);  // Setup memory copy to / copy from
        *(DMAHANDLER(u8EndpointNumber).pu8DmaDescriptor + DMA_OFFSET_BAPM) = (uint8_t)(u32StartAt >> 8);   // Setup memory copy to / copy from
        *(DMAHANDLER(u8EndpointNumber).pu8DmaDescriptor + DMA_OFFSET_BAPL) = (uint8_t)(u32StartAt & 0xFF); // Setup memory copy to / copy from
        DSR &= ~(1 << DMAHANDLER(u8EndpointNumber).u8DmaNumber);
        SET_MASK(EXT_ENDPOINT_CONTROL(u8EndpointNumber),MASK_EPC_DMAE);
        //SET_MASK(EXT_ENDPOINT_CONTROL(u8EndpointNumber),MASK_EPC_NULE);
        DER |= (1 << DMAHANDLER(u8EndpointNumber).u8DmaNumber);
        
    #endif
    
    #if (USB_MCU == MCU_FR80)
        DMACR |= 0x80000000;
        *DmaInfo[u8EndpointNumber - 1].DCCRn = 0;
        *DmaInfo[u8EndpointNumber - 1].DCSRn = 0;
        SET_MASK(EXT_ENDPOINT_CONTROL(u8EndpointNumber),MASK_EPC_DMAE);
        //SET_MASK(EXT_ENDPOINT_CONTROL(u8EndpointNumber),MASK_EPC_NULE);
        DREQSEL |= DmaInfo[u8EndpointNumber - 1].DREQSEL_MASK;
        *DmaInfo[u8EndpointNumber - 1].DTCRn = u16DataSize / 2;        //setup DMA transfer size
        *DmaInfo[u8EndpointNumber - 1].DCCRn = BIT_DCCR_RS0 | BIT_DCCR_RS1
                                               | BIT_DCCR_TM0 | BIT_DCCR_TM1
                                               | BIT_DCCR_DT
                                               | BIT_DCCR_TS0;
                                               //BIT_DCCR_AIE | BIT_DCCR_NIE
        if (u8Direction == DMA_DIR_HOST_OUT)
        {
            *DmaInfo[u8EndpointNumber - 1].DSARn = (uint32_t)GET_ENDPOINT(u8EndpointNumber).EPnDTL;
            *DmaInfo[u8EndpointNumber - 1].DDARn = u32StartAt;
            *DmaInfo[u8EndpointNumber - 1].DCCRn |= BIT_DCCR_SAC0 | BIT_DCCR_SAC1;
        }
        else
        {
            *DmaInfo[u8EndpointNumber - 1].DDARn = (uint32_t)GET_ENDPOINT(u8EndpointNumber).EPnDTL;
            *DmaInfo[u8EndpointNumber - 1].DSARn = u32StartAt;
            *DmaInfo[u8EndpointNumber - 1].DCCRn |= BIT_DCCR_DAC0 | BIT_DCCR_DAC1;
        }
        *DmaInfo[u8EndpointNumber - 1].DCCRn |= (uint32_t)BIT_DCCR_CE;
        
    #endif
    SET_MASK(EXT_ENDPOINT_STATUS(u8EndpointNumber),MASK_EPS_DRQIE);
}

/**
 ******************************************************************************
 ** Clear the DMA settings for an endpoint
 **
 ** 
 ** \param u8EndpointNumber Endpointnumber (1..5) 
 **
 ** \return none
 **
 *****************************************************************************/
 
void UsbDma_ReleaseDma(uint8_t u8EndpointNumber)
{
    uint8_t u8Direction = DMA_DIR_HOST_OUT;
    
    #if (USB_MCU == MCU_16FX)
        DER &= ~(1 << DMAHANDLER(u8EndpointNumber).u8DmaNumber);
        DSR &= ~(1 << DMAHANDLER(u8EndpointNumber).u8DmaNumber);
    #endif
    #if (USB_MCU == MCU_FR80)
        DMACR |= 0x80000000;
        *DmaInfo[u8EndpointNumber - 1].DCCRn &= ~(uint32_t)BIT_DCCR_CE;
        DMACR |= 0x80000000;
        *DmaInfo[u8EndpointNumber - 1].DCCRn = 0;
        *DmaInfo[u8EndpointNumber - 1].DCSRn = 0;
        DREQSEL &= ~(DmaInfo[u8EndpointNumber - 1].DREQSEL_MASK);
        if (DREQSEL == 0)
        {
            DMACR &= ~0x80000000;
        }
    #endif
    if ((EXT_ENDPOINT_CONTROL(u8EndpointNumber) & MASK_EPC_DIR))
    {
        u8Direction = DMA_DIR_HOST_IN;
    }
    
    dbg("DMA   |->DMA Clear Setup\n");
    dbg("DMA      |->for Endpoint: ");
    dbghex(u8EndpointNumber);
    dbg("\n");
    GET_ENDPOINT(u8EndpointNumber).status &= ~EP_STATUS_DMA_TRANSFER;
    
    
    CLEAR_MASK(EXT_ENDPOINT_CONTROL(u8EndpointNumber),MASK_EPC_NULE);
    CLEAR_MASK(EXT_ENDPOINT_CONTROL(u8EndpointNumber),MASK_EPC_DMAE);
    CLEAR_MASK(EXT_ENDPOINT_STATUS(u8EndpointNumber),MASK_EPS_DRQIE);
    
    if (u8Direction == DMA_DIR_HOST_OUT)
    {
        SET_MASK(EXT_ENDPOINT_STATUS(u8EndpointNumber),MASK_EPS_DRQIE);
    }
}

/**
 ******************************************************************************
 ** Enabled a DMA transfer for an endpoint
 **
 ** 
 ** \param u8EndpointNumber Endpointnumber (1..5) 
 **
 ** \param u32ReceiveSize size of data to be sent (max. 4294967296 bytes)
 **
 ** \return none
 **
 *****************************************************************************/
 
void UsbDma_EnableReceiveDma(uint8_t u8EndpointNumber, uint32_t u32ReceiveSize)
{
	UsbDma_ReleaseDma(u8EndpointNumber);
    GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position = 0;
    GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength = u32ReceiveSize;
    GET_ENDPOINT(u8EndpointNumber).status |= EP_STATUS_DMA_ENABLED;
    dbg   ("\nDMA   UsbFunction_EnableReceiveDma()\n");      //DEBUG
    dbgval("DMA      |->u8EndpointNumber",u8EndpointNumber); //DEBUG
    dbgval("DMA      |->u32ReceiveSize",u32ReceiveSize);     //DEBUG
    UsbDma_SendReceiveBufferViaDma(u8EndpointNumber); 
}

void UsbDma_SendReceiveBufferViaDma(uint8_t u8EndpointNumber)
{
    volatile uint16_t u16Size;
    uint8_t u8Direction = DMA_DIR_HOST_OUT;

    if ((EXT_ENDPOINT_CONTROL(u8EndpointNumber) & MASK_EPC_DIR))  //check the direction of data transfer
    {
        u8Direction = DMA_DIR_HOST_IN;
    }
    
    dbg   ("\nDMA   SendReceiveBufferViaDma()\n");  //DEBUG
    
    if (u8Direction == DMA_DIR_HOST_OUT) //on data receive
    {
        CLEAR_MASK(EXT_ENDPOINT_STATUS(u8EndpointNumber),MASK_EPS_DRQ); //clear the received flag
    }
    
    UsbDma_ReleaseDma(u8EndpointNumber); //release the DMA first
    
    #if (debug_enabled == 1)                        //DEBUG
        dbg   ("DMA  |->New DMA Package\n");        //DEBUG
        if (u8Direction == DMA_DIR_HOST_OUT)        //DEBUG
        {                                           //DEBUG
            dbg   ("DMA  |->Type: MCU receive\n");  //DEBUG
        }                                           //DEBUG
        else                                        //DEBUG
        {                                           //DEBUG
            dbg   ("DMA  |->Type: MCU receive\n");  //DEBUG
        }                                           //DEBUG
    #endif                                          //DEBUG
    
    dbgval("DMA   |->Position: ", GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position);    //DEBUG
    dbgval("DMA   |->Size: ",     GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength);  //DEBUG
    
    /* Position = DataLength ==> End of DMA */
    if ((GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position) >= (GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength))
    {
        if (u8Direction == DMA_DIR_HOST_OUT)
        {
            dbg("DMA   |->Received end...\n");
            GET_ENDPOINT(u8EndpointNumber).status &= ~EP_STATUS_DMA_ENABLED;
            GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position = 0;
        }
        else
        {
            GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position = GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength;
            dbg("TX   |->Transfer finished...\n");
            
            if ((GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength % GET_ENDPOINT(u8EndpointNumber).u16Size) == 0)
            {
                dbg("sending 0 byte\n");
                //GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength = 0;
                //GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position += 1;
                //UsbFunction_SendData(u8EndpointNumber,GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->pu8Buffer,GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength,USB_SENDING_MODE_INTERRUPT);
                //UsbFunction_SendData(u8EndpointNumber,0,0,USB_SENDING_MODE_INTERRUPT);
                GET_ENDPOINT(u8EndpointNumber).status &= ~EP_STATUS_DMA_ENABLED;
            }
        }
        return;
    }
    
    /* Position = (DataLength - 1) ==> Sending / Receiving last byte */
    if ((GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position) >= (GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength - 1)) // packages were sent (odd end)
    {
        if (u8Direction == DMA_DIR_HOST_OUT)
        {
            dbg("DMA   |->receiving last byte\n");
            GET_ENDPOINT(u8EndpointNumber).status &= ~EP_STATUS_DMA_ENABLED;
        }
        else
        {
            dbg("DMA   |->sending last byte\n");
            UsbFunction_SendData(u8EndpointNumber,GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->pu8Buffer,GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength,USB_SENDING_MODE_INTERRUPT);
        }
        return;
    }
    
    
    dbg("DMA   |->Creating new DMA package\n");
    /* DMA is not at the end, creating new package */
    /* Check for the PackageSize < 64KByte */
    if ((GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength - GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position) <= 0x10000)
    {
        /* the remaining data is lower than 64KByte */
        dbg("DMA   >>>Package <= 64KByte\n");
        
        /* get the remaining datasize */
        u16Size =  (uint16_t)(GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32DataLength - GET_ENDPOINT(u8EndpointNumber).pstcEndpointBuffer->u32Position);
        

        if (u16Size > GET_ENDPOINT(u8EndpointNumber).u16Size)
        {
            /* if the remaining data is bigger than the endpointbuffer size, it as to been fit into
               a multiple of the endpointbuffer size                                                */
            u16Size = u16Size / GET_ENDPOINT(u8EndpointNumber).u16Size;
            u16Size = u16Size * GET_ENDPOINT(u8EndpointNumber).u16Size;
        } 
        else
        {
            /* if the remaining data is lower than the endpointbuffer size, it as to been fit into
               a multiple of 2                                                                      */
            u16Size = u16Size / 2;
            u16Size = u16Size * 2;
        }
        UsbDma_SetupDma(u8EndpointNumber,u8EndpointNumber + 113,u16Size); //setup a new DMA package
    }
    else
    {
        /* the remaining data is bigger than 64KByte, but the DMA can only send max. 64KByte        */
        dbg("DMA   >>>Package > 64KByte\n");
        UsbDma_SetupDma(u8EndpointNumber,u8EndpointNumber + 113, 0); // DMA Size = 0 means 65536 Bytes
    }
}

__interrupt void UsbDma_DmaEndedIsr(void)
{
    uint8_t i;
    for(i=0;i<5;i++)
    {
       if((*DmaInfo[i].DCSRn & 0x01) > 0)
       {
          UsbDma_SendReceiveBufferViaDma(i);
       }
       if((*DmaInfo[i].DCSRn & 0x04) > 0)
       {
          puts("Normal Completion of DMA\n");
       }
    }
    *DmaInfo[i].DCSRn = 0x0000;
}
#endif
