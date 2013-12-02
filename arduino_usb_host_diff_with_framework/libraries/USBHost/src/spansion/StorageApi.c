/*---------------------------------------------------------------------------*/
/* THIS SAMPLE CODE IS PROVIDED AS IS AND IS SUBJECT TO ALTERATIONS. Spansion*/
/* LLC ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR */
/* ELIGIBILITY FOR ANY PURPOSES.                                             */
/*                 (C) Spansion LLC                  */
/*---------------------------------------------------------------------------*/

//#include "Usb.h"
#include "Arduino.h"

#if (USE_USB_HOST == 1)
#include <stdlib.h>
#include "UsbMassStorage.h"
#include "StorageApi.h"


StorageInfo_t* pstcStorageInfo;

int MSD_Read(BYTE *buff, DWORD sector, BYTE count)
{  
   unsigned int status;
   unsigned char NAKDetected = 0;
   pstcStorageInfo = UsbMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   pstcStorageInfo->STORAGE_CMD = READ_MASS_STORAGE;
   pstcStorageInfo->BUFFER = buff;
   pstcStorageInfo->BLOCK_START = sector;
   pstcStorageInfo->BLOCK_CNT = count;

   status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);
   if (status)
   {
       return status;
   }
   if (pstcStorageInfo->CSW_STATUS == 1)
   {
      struct RequestSenseResponse SenseResponse;
      uint8_t RSRBuffer[sizeof(SenseResponse)];
      ZERO_STRUCT(*pstcStorageInfo);
      ZERO_STRUCT(SenseResponse);
      pstcStorageInfo->STORAGE_CMD = MASS_STORAGE_REQUEST_SENSE;
      pstcStorageInfo->BUFFER = RSRBuffer; //(uint8_t*) &SenseResponse;
      status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);
      UsbMassStorage_CheckForCompletion(&NAKDetected);
      convBufferToSenseResponse(RSRBuffer,&SenseResponse);
      if(status)
      {
         return status;
      }
      status = SenseResponse.SenseKey;
      return status;
   }
  
   return status;
}

int MSD_Write(BYTE *buff, DWORD sector, BYTE count)
{
   unsigned int status;
   pstcStorageInfo = UsbMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   pstcStorageInfo->STORAGE_CMD = WRITE_MASS_STORAGE;
   pstcStorageInfo->BUFFER = buff;
   pstcStorageInfo->BLOCK_START = sector;
   pstcStorageInfo->BLOCK_CNT = count;
   
   status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);

   if (pstcStorageInfo->CSW_STATUS == 1)
   {
      RequestSenseResponse_t SenseResponse;
      uint8_t NAKDetected;
      uint8_t RSRBuffer[sizeof(SenseResponse)];
      ZERO_STRUCT(*pstcStorageInfo);
      ZERO_STRUCT(SenseResponse);
      pstcStorageInfo->STORAGE_CMD = MASS_STORAGE_REQUEST_SENSE;
      pstcStorageInfo->BUFFER = RSRBuffer; //(unsigned char *) &SenseResponse;
      status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);
      
      UsbMassStorage_CheckForCompletion(&NAKDetected);
      convBufferToSenseResponse(RSRBuffer,&SenseResponse);
      if (status)
      {
         return status;
      }
      
      if(SenseResponse.SenseKey == 0x7)
      {
         return 0x02;  // Write protect
      }
      
      status = SenseResponse.SenseKey;
      return status;      
   }
   
   return status; 
}

int MSD_Read_Capacity(void (*Buff))
{
   unsigned int status;
   pstcStorageInfo = UsbMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   pstcStorageInfo->STORAGE_CMD = READ_MASS_STORAGE_CAPACITY;
   pstcStorageInfo->BUFFER = Buff;
   
   status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);

   if (pstcStorageInfo->CSW_STATUS == 1)
   {
      RequestSenseResponse_t SenseResponse;
      uint8_t NAKDetected;
      uint8_t RSRBuffer[sizeof(SenseResponse)];
      ZERO_STRUCT(*pstcStorageInfo);
      ZERO_STRUCT(SenseResponse);
      pstcStorageInfo->STORAGE_CMD = MASS_STORAGE_REQUEST_SENSE;
      pstcStorageInfo->BUFFER = RSRBuffer; //(unsigned char *)&SenseResponse;
      status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);
      
      UsbMassStorage_CheckForCompletion(&NAKDetected);
      convBufferToSenseResponse(RSRBuffer,&SenseResponse);
      if (status)
      {
         return status;
      }
      status = SenseResponse.SenseKey;
      return status;
   }
   
   return status; 
}


int StorageInit(void)
{
   unsigned char cnt=0;
   unsigned char CheckAgain = 1; 
   unsigned char NAKDetected = 0; 
   InquiryResponse_t InquiryResponse;
   uint8_t IRbuffer[sizeof(InquiryResponse)];
   unsigned int status=0;
   pstcStorageInfo = UsbMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   ZERO_STRUCT(InquiryResponse);
   pstcStorageInfo->STORAGE_CMD = INQUIRY_MASS_STORAGE;
   pstcStorageInfo->BUFFER = IRbuffer;
   //(unsigned char *)&InquiryResponse;
      
   status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);
   UsbMassStorage_CheckForCompletion(&NAKDetected);
   convBufferToInquiryResponse(IRbuffer,&InquiryResponse);  
   
   if (status)
   {
      return status;
   }
   
   while(CheckAgain)
   {
      if(cnt >= 2)
      {
         return USBH_STATUS_ERROR;
      }
      
      ZERO_STRUCT(*pstcStorageInfo);
      pstcStorageInfo->STORAGE_CMD = TEST_UNIT_READY;      
      dbg("Testing unit ready\n");
      status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);
      
      if (status)
      {
         return status;
      }   
      
      if (pstcStorageInfo->CSW_STATUS == 1)
      {
         struct RequestSenseResponse SenseResponse;
         uint8_t RSRBuffer[sizeof(SenseResponse)];
         ZERO_STRUCT(*pstcStorageInfo);
         ZERO_STRUCT(SenseResponse);
         pstcStorageInfo->STORAGE_CMD = MASS_STORAGE_REQUEST_SENSE;
         pstcStorageInfo->BUFFER = RSRBuffer;
         //(uint8_t *)&SenseResponse;
         status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);
         UsbMassStorage_CheckForCompletion(&NAKDetected);
         convBufferToSenseResponse(RSRBuffer,&SenseResponse);
         if (status)
         {
            return status;
         }
         if (SenseResponse.SenseKey != 0x06)
         {
            CheckAgain = 0;        
         }
         cnt++;
         status = SenseResponse.SenseKey;         
      }
      else
      {
         CheckAgain = 0;
      }
   } // end of while 
   status = UsbMassStorage_StorageStatus();   
   return status;
}

int ModeSense(void)
{
   uint16_t status;
   struct ModeSenseResponse ModeSense;
   uint8_t MSRBuffer[sizeof(ModeSense)];
   uint8_t NAKDetected;
   
   pstcStorageInfo = UsbMassStorage_GetGlobalStorageInfo();
   ZERO_STRUCT(*pstcStorageInfo);
   ZERO_STRUCT(ModeSense);
   pstcStorageInfo->STORAGE_CMD = MODE_SENSE;
   pstcStorageInfo->BUFFER = MSRBuffer; //(uint8_t*)&ModeSense;   
   status = UsbMassStorage_StorageCommandSend(pstcStorageInfo);
   UsbMassStorage_CheckForCompletion(&NAKDetected);
   convBufferToModeSenseResponse(MSRBuffer,&ModeSense);
   if (ModeSense.WriteProtect == 1)
   {
      return USBH_STATUS_ERROR;
   }      
   return status; 
}

#endif /* (USE_USB_HOST == 1) */
