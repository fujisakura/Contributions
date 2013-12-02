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

/*****************************************************************************/
/** \file UsbFunction.c
 **
 ** - See README.TXT for project description
 ** - first release for a simple universal usb function library
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX)
 **   - 2011-08-11    1.1  MSc  Several Bug fixes
 **   - 2011-08-24    1.2  MSc  Timeout Bug fixes
 *****************************************************************************/


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

//#include "Usb.h" 
#include "Arduino.h"

#if (USE_USB_HOST == 1)
#include "UsbMassStorage.h"


/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/


/*****************************************************************************/
/* function */
/*****************************************************************************/


/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/
/*
UMH_HANDLE     umhInHandle;
UMH_HANDLE     umhOutHandle;
*/

static HostEndpoint_t *EndpointOUT;
static HostEndpoint_t *EndpointIN;
static uint8_t        u8EndpointIN, u8EndpointOUT;
static uint16_t       u16MaxPackageSizeIN, u16MaxPackageSizeOUT;
volatile static uint8_t        u8Status = 0;
volatile static boolean_t      bSetupComplete = FALSE;
StorageInfo_t  stcGlobalStorageInfo;
static UsbRequest_t setupmsc;
/*SETUP_DATA     setup; 
StorageInfo_t  stcGlobalStorageInfo;
*/

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
/*void SwapBuffer(uint8_t* pu8Buffer, uint32_t u32Size)
{
    uint8_t u8Temp;
    u32Size = u32Size / 2;
    while(u32Size)
    {
       u8Temp = pu8Buffer[0];
       pu8Buffer[0] = pu8Buffer[1];
       pu8Buffer[1] = u8Temp;
       //u8Temp = pu8Buffer[1];
       //pu8Buffer[1] = pu8Buffer[2];
       //pu8Buffer[2] = u8Temp;
       pu8Buffer += 2;
       u32Size -= 1;
    }
}*/

boolean_t MassStorageInitHandler(uint8_t* pu8Configuration, uint32_t u32Length)
{
    uint8_t* pu8Buffer;
    uint8_t u8NumberOfInterfaces;
    uint8_t u8NumberOfEndpoints;
    uint8_t u8EndpointNumber;
    uint8_t u8InterfaceNumber;
    
    dbg("Yeah, called if a MassStorage Device was connected ;-)\n");
    
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
        
        if ((pu8Buffer[6] == 0x06) && (pu8Buffer[7] == 0x50)) //SubClass & Protocol
        {
            for(u8EndpointNumber=1;u8EndpointNumber<=u8NumberOfEndpoints;u8EndpointNumber++)
            {
                dbgval("Endpoint Index: ",u8EndpointNumber);
                pu8Buffer = UsbHost_GetUsbEndpointDescriptor(pu8Configuration,u32Length,u8InterfaceNumber,u8EndpointNumber);
                if (pu8Buffer[2] & USB_IN_DIRECTION)
                {
                    u8EndpointIN = pu8Buffer[2];
                    u16MaxPackageSizeIN = (uint16_t)(pu8Buffer[4] + (pu8Buffer[5] << 8));
                }
                else
                {
                    u8EndpointOUT = pu8Buffer[2];
                    u16MaxPackageSizeOUT = (uint16_t)(pu8Buffer[4] + (pu8Buffer[5] << 8));
                }
                if ((u8EndpointIN != 0) && (u8EndpointOUT != 0))
                {
                    break;
                }
            }
            if ((u8EndpointIN == 0) && (u8EndpointOUT == 0))
            {
                dbg("Hm, error while resolving endpoints...");
                return FALSE;
            }
            dbgval("EP-IN:", u8EndpointIN);

            
            dbgval("EP-OUT:", u8EndpointOUT);
            
            UsbHost_AddHostEndpoint(&EndpointOUT,u8EndpointOUT,u16MaxPackageSizeOUT,0,0);
            UsbHost_AddHostEndpoint(&EndpointIN,u8EndpointIN,u16MaxPackageSizeIN,0,0);
             
            dbgval("Endpoint-IN  Addr: ",u8EndpointIN);
            dbgval("Endpoint-IN  Size: ",u16MaxPackageSizeIN);
            dbgval("Endpoint-OUT Addr: ",u8EndpointOUT);
            dbgval("Endpoint-OUT Size: ",u16MaxPackageSizeOUT);
            break;
        }
    }
    if ((u8EndpointIN == 0) || (u8EndpointOUT == 0))
    {
        dbg("Hm, error while resolving endpoints...");
        return FALSE;
    }       
    return TRUE;               
}

boolean_t MassStorageDeinitHandler()
{
    dbg("\nUSB Device disconnected\n");
    return TRUE;
}

void MassStorageSetupComplete(uint8_t* pu8Buffer, uint32_t u32DataSize)
{
    dbg("MassStorage SetupRequest Complete\n");
    bSetupComplete = TRUE;
}

void MassStorageWaitForSetupComplete()
{
    volatile uint32_t u32Timeout = 1000000;
    while((bSetupComplete == FALSE) && (u32Timeout > 5))
    {
        u32Timeout--;
    }
}
void UsbMassStorage_BulkInCompletion()
{
    //SwapBuffer(EndpointIN->pu8Buffer,EndpointIN->u32DataSize);
    stcGlobalStorageInfo.STATUS = ((EndpointIN->u8Status) & 0xF0);
    stcGlobalStorageInfo.BYTES_TRANSFERRED = EndpointIN->u32DataSize;
    dbgval("BulkInCompletion, Size:",stcGlobalStorageInfo.BYTES_TRANSFERRED);
    UsbMassStorage_InTransferDone();
}

void UsbMassStorage_BulkOutCompletion()
{
    stcGlobalStorageInfo.STATUS = ((EndpointOUT->u8Status) & 0xF0);
    stcGlobalStorageInfo.BYTES_TRANSFERRED = EndpointOUT->u32DataSize;
    dbgval("BulkOutCompletion, Size:",stcGlobalStorageInfo.BYTES_TRANSFERRED);
    UsbMassStorage_OutTransferDone();
}

// transfer done is called from the transfer completion function
void UsbMassStorage_InTransferDone(void)
{
    CLEAR_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN);
}

void UsbMassStorage_OutTransferDone(void)
{
    CLEAR_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT);
}



StorageInfo_t* UsbMassStorage_GetGlobalStorageInfo()
{
    return &stcGlobalStorageInfo;
}



read_capacity_t ReadCapacity;
csw_t csw;
cbw_t cbw;
uint8_t CswBuffer[13];
uint8_t CbwBuffer[31];

void convBufferToSenseResponse(uint8_t *buffer, struct RequestSenseResponse *sr) 
{
    sr->ResponseCode = ((buffer[0] & 0xFE) >> 1);
    sr->VALID = (buffer[0] & 0x01);
    sr->Obsolete = buffer[1];
    sr->SenseKey = ((buffer[2] & 0xF0) >> 4);
    sr->ILI = ((buffer[2] & 0x04) >> 2);
    sr->EOM = ((buffer[2] & 0x02) >> 1);
    sr->FILEMARK = (buffer[2] & 0x01);
    sr->Information =  LENDIAN_BUFFER_TO_U32(buffer+3);
    sr->AddSenseLen = buffer[7];
    sr->CmdSpecificInfo =  LENDIAN_BUFFER_TO_U32(buffer+8);
    sr->ASC = buffer[12];
    sr->ASCQ = buffer[13];
    sr->FRUC = buffer[14];
    sr->SenseKeySpecific[0] = buffer[15];
    sr->SenseKeySpecific[1] = buffer[16];
    sr->SenseKeySpecific[2] = buffer[17];
}
void convBufferToModeSenseResponse(uint8_t *buffer, struct ModeSenseResponse *msr) {
    msr->ModeDataLength = buffer[0];
    msr->MediumType = buffer[1];
    msr->Reserved1 = ((buffer[2] & 0xF0) >> 4);
    msr->DPOFUA = ((buffer[2] & 0x08) >> 3); 
    msr->Reserved2 = ((buffer[2] & 0x06) >> 1);
    msr->WriteProtect = (buffer[2] & 0x01); 
    msr->BlockDescriptorLength = buffer[3];
}
void convBufferToInquiryResponse(uint8_t *buffer, InquiryResponse_t *ir) {
    unsigned char i;
    ir->Peripheral = buffer[0];
    ir->Removable = buffer[1];
    ir->Version = buffer[2];
    ir->ResponseDataFormat = buffer[3];
    ir->AdditionalLength = buffer[4];
    ir->Sccstp = buffer[5];
    ir->Bqueetc = buffer[6];
    ir->CmdQue = buffer[7];
    for(i=0;i<8;i++) 
    {
        ir->VendorID[i] = buffer[8+i];
    }
    for(i=0;i<16;i++) 
    {
        ir->ProductID[i] = buffer[16+i];
    }
}
void convCBWtoBuffer(cbw_t *cbw, uint8_t *Buffer)
{
    unsigned char i;
    U32_TO_LENDIAN_BUFFER(Buffer,cbw->dCBWSignature);            // index 0:Signature
    U32_TO_LENDIAN_BUFFER(Buffer+4,cbw->dCBWTag);                // index:4 Tag
    U32_TO_LENDIAN_BUFFER(Buffer+8,cbw->dCBWDataTransferLength); // index:8 DataTransferLength
    Buffer[12]=cbw->bCBWFlags;
    Buffer[13]=cbw->bCBWLUN;
    Buffer[14]=cbw->bCBWCBLength;
    for(i=0;i<16;i++)
    {
        Buffer[15 + i] = cbw->CBWCB[i];
    }
}

int
convBufferToCSW(
    uint8_t *buffer,      // buffer that contains a status wrapper block  
    int length,                 // length of the buffer
    csw_t *csw
)
{
    if( length < 13)
    {
        return USBH_STATUS_LENGTH;
    }
    csw->CSWSignatur=LENDIAN_BUFFER_TO_U32(buffer);
    csw->CSWTag=LENDIAN_BUFFER_TO_U32(buffer+4);    // 4: tag Same as original command
    csw->CSWDataResidue=LENDIAN_BUFFER_TO_U32(buffer+8);// 8: residue, amount of bytes not transferred
    csw->CSWStatus=(uint32_t)*(buffer+12);//12:status
    return USBH_STATUS_SUCCESS;
}


void ErrorHandler(uint16_t errCode, uint8_t *clrEponly )
{
    //uint8_t InEpStatus=0, OutEpStatus=0;
    dbg("ErrorHandler\n");
    if (errCode & USBHOST_ENDPOINTSTATUS_STALL)                  
    {
        dbg("STALL Condition was set...");
        if(*clrEponly)
        {
                //UsbMassStorage_GetEPStatus(&InEpStatus, &OutEpStatus);
           if((EndpointIN->u8Status) & USBHOST_ENDPOINTSTATUS_STALL)
           {
               dbg("STALLED OUT\n");
               bSetupComplete = FALSE;
               UsbMassStorage_ClearInEndpoint();     
               EndpointIN->u8Status &= ~USBHOST_ENDPOINTSTATUS_STALL;
           }
           if((EndpointOUT->u8Status) & USBHOST_ENDPOINTSTATUS_STALL)
           {
               dbg("STALLED IN\n");
               UsbMassStorage_ClearOutEndpoint();
               EndpointOUT->u8Status &= ~USBHOST_ENDPOINTSTATUS_STALL;
           }
           *clrEponly=0;
       }
       return;
       //UsbMassStorage_ResetRecovery();
   }   
   //UsbMassStorage_ResetRecovery();
}

void UsbMassStorage_CheckForCompletion(volatile uint8_t *NAKDetected)
{
   volatile uint32_t u32Timeout = 20000000;
   *NAKDetected = 0;
   dbg("Check for completion...\n");
   while((IS_SET(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT)) || (IS_SET(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN))) // Wait for status information of previous transmission
   {  
      if ((u32Timeout < 5) || (UsbHost_GetDeviceStatus() == USBHOST_DEVICE_IDLE))  // if there is no Handshake, something went wrong!
      {
         CLEAR_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN);
         CLEAR_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT);
         EndpointOUT->bAbortTransfer = TRUE;
         EndpointIN->bAbortTransfer = TRUE;
         *NAKDetected = 1;   // There must be a circular NAK Handshake
         u32Timeout = 1000000;
         while((EndpointIN->bAbortTransfer) && (EndpointOUT->bAbortTransfer) && (u32Timeout > 5) && (UsbHost_GetDeviceStatus() != USBHOST_DEVICE_IDLE)) u32Timeout--;
         dbg("NAK!\n");
      }
      u32Timeout--;
   } 
   dbg("complete!\n");
}



int16_t UsbMassStorage_StorageCommandSend(StorageInfo_t *pstcStorageInfo)
{
    uint16_t status;
    volatile uint8_t NAKDetected=0;
    uint8_t clrEponly=0;
    uint16_t cnt=0;
    cnt = cnt;

    UsbMassStorage_CheckForCompletion(&NAKDetected);
    SET_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT); // Block transfer 
    status = UsbMassStorage_SendCBW(pstcStorageInfo);
    if(status)
    {
        return status;
    }
    UsbMassStorage_CheckForCompletion(&NAKDetected);
    if(pstcStorageInfo->STATUS || NAKDetected)
    { 
        ErrorHandler(pstcStorageInfo->STATUS, &clrEponly );      
        return USBH_STATUS_ERROR;  
    }

    if(cbw.dCBWDataTransferLength != 0) // Jump in, if there is a data Transfer
    {    
        dbg("StorageCommandSend: Has Data Transfer\n");
        if(pstcStorageInfo->STORAGE_CMD == WRITE_MASS_STORAGE)
        {
            SET_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT);
        }
        else
        {
            SET_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN);
        }   
        status = UsbMassStorage_Transmit_Data(pstcStorageInfo);
        if(status)
        {
            return status;
        }
        UsbMassStorage_CheckForCompletion(&NAKDetected);
        dbgval("2Storage-Status:",(pstcStorageInfo->STATUS));
        dbgval("2NAK:",(NAKDetected));
        if((pstcStorageInfo->STATUS) || (NAKDetected))
        {
             clrEponly=1;
             ErrorHandler(pstcStorageInfo->STATUS, &clrEponly) ;
             NAKDetected=0;       
        } 
    }   
    SET_MASK(u8Status,USBMASSSTORAGE_FLAG_TRANSFERDONE_IN);
    status = UsbMassStorage_GetCSW(pstcStorageInfo);
    if(status)
    {
        return status;
    }
   
    UsbMassStorage_CheckForCompletion(&NAKDetected);
    dbgval("3Storage-Status:",(pstcStorageInfo->STATUS));
    dbgval("3NAK:",(NAKDetected));
    if((pstcStorageInfo->STATUS) || (NAKDetected))
    {
        
        clrEponly=1;
        ErrorHandler(pstcStorageInfo->STATUS, &clrEponly);
        NAKDetected=0;
        status = UsbMassStorage_GetCSW(pstcStorageInfo);
        if(status)
        {
            return status;
        }
        UsbMassStorage_CheckForCompletion(&NAKDetected);
        if(pstcStorageInfo->STATUS || NAKDetected)
        {
            ErrorHandler(pstcStorageInfo->STATUS, &clrEponly);
            return USBH_STATUS_ERROR;  
        }
    }
    status = UsbMassStorage_CheckCSW(pstcStorageInfo);    
    return status;
}

int16_t UsbMassStorage_SendCBW(StorageInfo_t *pstcStorageInfo)
{  
    uint16_t status;
    ZERO_STRUCT(cbw);
    pstcStorageInfo->SEND_CBW = 1;
    pstcStorageInfo->BUFFER_SIZE = 31; // CBW Size must always be 31 byte
 
    cbw.dCBWSignature = 0x43425355; 
    cbw.dCBWTag = CBW_TAG; 
    cbw.bCBWLUN = 0x00; 
    cbw.CBWCB[1] = 0x00;
    cbw.CBWCB[2] = (0xff & (pstcStorageInfo->BLOCK_START >> 24));
    cbw.CBWCB[3] = (0xff & (pstcStorageInfo->BLOCK_START >> 16));
    cbw.CBWCB[4] = (0xff & (pstcStorageInfo->BLOCK_START >> 8));
    cbw.CBWCB[5] = (0xff & (pstcStorageInfo->BLOCK_START));
    cbw.CBWCB[6] = 0x00;
    cbw.CBWCB[7] = (uint8_t)(((uint16_t)(pstcStorageInfo->BLOCK_CNT) >> 8) & 0xff);
    cbw.CBWCB[8] = (uint8_t)(((uint16_t)(pstcStorageInfo->BLOCK_CNT)  & 0xff));
    cbw.CBWCB[9] = 0x00;
   
    switch(pstcStorageInfo->STORAGE_CMD)                  
    {
        case READ_MASS_STORAGE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000200;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x0A; 
            break;
      
        case WRITE_MASS_STORAGE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000200;
            cbw.bCBWFlags = 0x00;
            cbw.bCBWCBLength = 0x0A; 
            break;
      
        case INQUIRY_MASS_STORAGE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000024;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x06;
            cbw.CBWCB[4] = 0x24; 
            break;
      
        case READ_MASS_STORAGE_CAPACITY:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000008;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x0A ;
            break;

        case TEST_UNIT_READY:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000000;
            cbw.bCBWFlags = 0x00;
            cbw.bCBWCBLength = 0x06;
            break;
            
        case MASS_STORAGE_REQUEST_SENSE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000012;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x0C;
            cbw.CBWCB[4] = 0x12; 
            break;
      
        case SEND_DIAGNOSTIC:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.CBWCB[1] = 0x04;
            cbw.dCBWDataTransferLength = 0x00000000;
            cbw.bCBWCBLength = 0x0A;
            break;
      
        case MODE_SENSE:
            cbw.CBWCB[0] = pstcStorageInfo->STORAGE_CMD;
            cbw.dCBWDataTransferLength = 0x00000004;
            cbw.bCBWFlags = 0x80;
            cbw.bCBWCBLength = 0x06;
            cbw.CBWCB[2] = 0x3F;
            cbw.CBWCB[4] = 0x04;
            break;
   }
   #if (ENDIANTYPE == LITTLEENDIAN)
       status = UsbMassStorage_Transmit_MSDC_Data((uint8_t*)&cbw, pstcStorageInfo); 
   #else
       convCBWtoBuffer(&cbw,CbwBuffer);
       status = UsbMassStorage_Transmit_MSDC_Data((uint8_t*)CbwBuffer, pstcStorageInfo); 
       if (status)
       {
           //status = UsbMassStorage_Transmit_MSDC_Data((uint8_t*)CbwBuffer, pstcStorageInfo); 
       }
       
   #endif 
   pstcStorageInfo->SEND_CBW = 0;
   return status;
   
}

int16_t UsbMassStorage_Transmit_MSDC_Data(uint8_t *pu8Buffer, StorageInfo_t *pstcStorageInfo)
{   
   if (pstcStorageInfo == 0)
   {
      dbg("WARNING: UsbMassStorage_Transmit_MSDC_Data - StorageInfo is empty!\n");
      return 2;
   }
   //SwapBuffer(pu8Buffer,pstcStorageInfo->BUFFER_SIZE);
   if((EndpointOUT != 0) && (EndpointIN != 0))                 
   {
      
      if(pstcStorageInfo->SEND_CBW)
      {
         dbgval("Sending CBW, Size: ",(pstcStorageInfo->BUFFER_SIZE));
         UsbHost_TransferData(EndpointOUT,pu8Buffer, pstcStorageInfo->BUFFER_SIZE, UsbMassStorage_BulkOutCompletion);
         return USBH_STATUS_SUCCESS;
      }
      
      if(pstcStorageInfo->GET_CSW)
      {
         dbgval("Receiving Request CSW, Size: ",(pstcStorageInfo->BUFFER_SIZE));
         UsbHost_TransferData(EndpointIN,pu8Buffer, pstcStorageInfo->BUFFER_SIZE, UsbMassStorage_BulkInCompletion);
         return USBH_STATUS_SUCCESS;
      }
       
      switch(pstcStorageInfo->STORAGE_CMD)                  
      {

         case WRITE_MASS_STORAGE:
         case TEST_UNIT_READY:
         dbgval("Sending CMD, Size: ",(pstcStorageInfo->BUFFER_SIZE));
         UsbHost_TransferData(EndpointOUT,pu8Buffer, pstcStorageInfo->BUFFER_SIZE, UsbMassStorage_BulkOutCompletion);      
         break; 
         

         case MASS_STORAGE_REQUEST_SENSE:
         case MODE_SENSE:
         case READ_MASS_STORAGE_CAPACITY:
         case INQUIRY_MASS_STORAGE:     
         case READ_MASS_STORAGE:     
         dbgval("Reading, Size: ",(pstcStorageInfo->BUFFER_SIZE));  
         UsbHost_TransferData(EndpointIN,pu8Buffer, pstcStorageInfo->BUFFER_SIZE, UsbMassStorage_BulkInCompletion);
         break;
       }   
       return USBH_STATUS_SUCCESS;     
   }
   
   dbg("No In or Out Handle found\n");
   return USBH_STATUS_ERROR;   
         
}

int16_t UsbMassStorage_Transmit_Data(StorageInfo_t *pstcStorageInfo)
{
    uint16_t status;
    pstcStorageInfo->BUFFER_SIZE = cbw.dCBWDataTransferLength;
    dbg("Transmit_Data:\n");
    dbgval("#",pstcStorageInfo->BUFFER);
    status = UsbMassStorage_Transmit_MSDC_Data(pstcStorageInfo->BUFFER, pstcStorageInfo);
    return status;
}

int16_t UsbMassStorage_GetCSW(StorageInfo_t *pstcStorageInfo)
{  
    uint16_t status;
    volatile uint8_t NAKDetected=0;
    ZERO_STRUCT(csw);
    pstcStorageInfo->GET_CSW = 1;
    pstcStorageInfo->BUFFER_SIZE = 13; //CBW Size must always be 13 byte
    #if (ENDIANTYPE == LITTLEENDIAN)
        status = UsbMassStorage_Transmit_MSDC_Data((uint8_t*)&csw, pstcStorageInfo);
    #else 
        status = UsbMassStorage_Transmit_MSDC_Data((uint8_t*)CswBuffer, pstcStorageInfo); 
        UsbMassStorage_CheckForCompletion(&NAKDetected);
        if ((status == 0) && (NAKDetected == 1))
        {
            status = 3;
        }
        if (!status) 
        {
            status = convBufferToCSW(CswBuffer,13,&csw);
        }
    #endif
    pstcStorageInfo->GET_CSW = 0;
    return status;
}

int16_t UsbMassStorage_CheckCSW(StorageInfo_t *pstcStorageInfo)
{  // Check if CSW is valid
    uint8_t clrEponly=0;
    dbg("UsbMassStorage_CheckCSW\n");
    dbgval("csw.CSWTag ",(csw.CSWTag));
    dbgval("csw.CSWSignatur ",(csw.CSWSignatur));
    dbgval("pstcStorageInfo->BYTES_TRANSFERRED ",(pstcStorageInfo->BYTES_TRANSFERRED));
    if((csw.CSWTag != CBW_TAG) || (csw.CSWSignatur != 0x53425355) ||((pstcStorageInfo->BYTES_TRANSFERRED) != 13 ))
    {
        ErrorHandler((uint16_t)NULL, &clrEponly);
        return USBH_STATUS_ERROR;
    }  // Check if CSW is meaningful
    if((csw.CSWStatus == 2) || (csw.CSWDataResidue >> cbw.dCBWDataTransferLength)) 
    {
        dbg("Storage data transmission failed\n");
        ErrorHandler((uint16_t)NULL, &clrEponly);
        return USBH_STATUS_ERROR;
    }
    if(csw.CSWStatus == 1)
    {
        pstcStorageInfo->CSW_STATUS = 1;   
    }
    return USBH_STATUS_SUCCESS;
}


int16_t UsbMassStorage_BulkOnlyMSReset(void)
{
    ZERO_STRUCT(setupmsc);
    setupmsc.bmRequestType=0x21;  
    setupmsc.bRequest=0xFF;
    setupmsc.wValue=0;
    setupmsc.wIndex=0;
    setupmsc.wLength=0;
    UsbHost_SetupRequest(&setupmsc,MassStorageSetupComplete);
    MassStorageWaitForSetupComplete();
    return USBH_STATUS_SUCCESS;
}

int16_t UsbMassStorage_StorageStatus(void)
{  
    if( UsbHost_GetDeviceStatus() == USBHOST_DEVICE_CONFIGURED)
    {
        dbg("O.K.!\n");
        return 0;
    }
    else
    {
        dbg("Not O.K.!\n");
        return 1;
    }
} 

int16_t UsbMassStorage_ResetRecovery(void)
{
    uint16_t status;
    status = UsbMassStorage_BulkOnlyMSReset();
    status = UsbMassStorage_ClearOutEndpoint();
    status = UsbMassStorage_ClearInEndpoint();
    status = UsbMassStorage_TestUnitReady(&stcGlobalStorageInfo);
    return status; 

}

int16_t UsbMassStorage_ClearInEndpoint(void)
{
    ZERO_STRUCT(setupmsc);
    setupmsc.bmRequestType=0x02;  
    setupmsc.bRequest=0x01;
    setupmsc.wValue=0x00;
    setupmsc.wIndex = USB_IN_DIRECTION | u8EndpointIN;
    setupmsc.wLength=0x00;
    //UMH_AbortTransfer(umhInHandle); TBD
    UsbHost_SetupRequest(&setupmsc,MassStorageSetupComplete);
    MassStorageWaitForSetupComplete();
    //UMH_ResetDataToggleBit(umhInHandle); TBD
    return USBH_STATUS_SUCCESS;
}

int16_t UsbMassStorage_ClearOutEndpoint(void)
{
    ZERO_STRUCT(setupmsc);
    setupmsc.bmRequestType=0x02;  
    setupmsc.bRequest=0x01;
    setupmsc.wValue=0x00;  
    setupmsc.wIndex=USB_OUT_DIRECTION | u8EndpointOUT;
    setupmsc.wLength=0x00;
    //UMH_AbortTransfer(umhOutHandle);  TBD
    UsbHost_SetupRequest(&setupmsc,MassStorageSetupComplete);
    MassStorageWaitForSetupComplete();
    //UMH_ResetDataToggleBit(umhOutHandle);  TBD   
    return USBH_STATUS_SUCCESS;
}

int UsbMassStorage_TestUnitReady(StorageInfo_t *pstcStorageInfo)
{  
    uint16_t status;
    ZERO_STRUCT(pstcStorageInfo);
    pstcStorageInfo->STORAGE_CMD = TEST_UNIT_READY;

    status = UsbMassStorage_SendCBW(pstcStorageInfo);
    //UMH_StallExecutionUserCallback(200);
    UsbMassStorage_GetCSW(pstcStorageInfo);

    return status;
}

/*
void UsbMassStorage_GetEPStatus(uint8_t* InEpStatus, uint8_t* OutEpStatus)
{
    SETUP_DATA setup;
    int16_t status;
    uint16_t transf=0;
  
    ZERO_STRUCT(setup);
    setup.Bytes.Type=0x82;  
    setup.Bytes.Request=0x00;
    setup.Words.Value=0x00;  
    setup.Words.Index=USB_IN_DIRECTION | u8EndpointIN;
    setup.Words.Length=0x02;
    status = UMH_SetupRequest((uint8_t*)&setup,(uint8_t*)InEpStatus,(unsigned int*)&transf);     
  
    ZERO_STRUCT(setup);
    setup.Bytes.Type=0x82;  
    setup.Bytes.Request=0x00;
    setup.Words.Value=0x00;  
    setup.Words.Index=USB_OUT_DIRECTION | u8EndpointOUT;
    setup.Words.Length=0x02;
    status = UMH_SetupRequest((uint8_t*)&setup,(uint8_t*)OutEpStatus,(unsigned int*)&transf);     
}
*/

// max. execution time is 100us
void Idle(void)
{
  return;
}

// this function is called from the library to wait 
// a specified time. This function must be implemented by the user
// with the fixed function name UmhStallExecutionCallback.
// Return: nothing
// parameter: time in 0.1 milliseconds units

/*void UMH_StallExecutionUserCallback(unsigned long time)
{ 
  unsigned long startTime=GetCurrentTime();

  while(1)
  {
    if( IsTimeOver(time,startTime)  )
      break;
    Idle();
  }
}*/
#endif /* (USE_USB_HOST == 1) */
