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
/** \file UsbClass.h
 **
 ** USB Descriptors File
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX)
 **   - 2011-08-11    1.1  MSc  Several Bug fixes
 **   - 2011-08-24    1.2  MSc  Timeout Bug fixes
 *****************************************************************************/

#ifndef __USBMASSSTORAGE_H__
#define __USBMASSSTORAGE_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

//#include "mcu.h"
//#include "base_type.h"
//#include "UsbHost.h"
#include "Arduino.h"
#if (USE_USB_HOST == 1)
//#include "usbspec.h"
#include "UsbMassStorage.h"
//#include "StorageApi.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USBMASSSTORAGE_FLAG_TRANSFERDONE     (1<<0)
#define USBMASSSTORAGE_FLAG_TRANSFERDONE_IN  (1<<1)
#define USBMASSSTORAGE_FLAG_TRANSFERDONE_OUT (1<<2)

#define FORMAT_UNIT							0x04
#define INQUIRY_MASS_STORAGE				0x12
#define MODE_SELECT							0x55
#define MODE_SENSE							0x5A
#define PREVENT_ALLOW_MEDIUM_REMOVAL		0x1E
#define READ_MASS_STORAGE  				    0x28
#define READ_MASS_STORAGE_CAPACITY			0x25	
#define READ_FORMAT_CAPACITIES				0x23
#define MASS_STORAGE_REQUEST_SENSE			0x03
#define REZERO								0x01
#define SEEK10								0x2B
#define SEND_DIAGNOSTIC						0x1D
#define START_STOP_UNIT						0x1B
#define TEST_UNIT_READY						0x00
#define VERIFY								0x2F
#define WRITE_MASS_STORAGE   				0x2A
#define WRITE12								0xAA
#define WRITE_AND_VERIFY					0x2E

#define CBW_TAG                       0x1A2B3C4D 

#define LITTLEENDIAN 1
#define BIGENDIAN 2
#define ENDIANTYPE BIGENDIAN

#define LENDIAN_BUFFER_TO_U32(pu8Buffer)\
(uint32_t)( (uint32_t)*(uint8_t*)(pu8Buffer)                                   \
         |  ((((uint32_t)*(((uint8_t*)(pu8Buffer))+1)) <<  8  ) & 0xFF00UL    )\
         |  ((((uint32_t)*(((uint8_t*)(pu8Buffer))+2)) << 16  ) & 0xFF0000UL  )\
         |  ((((uint32_t)*(((uint8_t*)(pu8Buffer))+3)) << 24  ) & 0xFF000000UL)\
         )

#define U32_TO_LENDIAN_BUFFER(pu8Buffer,u32Data)\
    { *((pu8Buffer))  =(uint8_t)((uint32_t)(u32Data)      );\
      *((pu8Buffer)+1)=(uint8_t)((uint32_t)(u32Data) >>  8);\
      *((pu8Buffer)+2)=(uint8_t)((uint32_t)(u32Data) >> 16);\
      *((pu8Buffer)+3)=(uint8_t)((uint32_t)(u32Data) >> 24);\
    }

/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/
typedef struct cbw
{
   uint32_t dCBWSignature;
   uint32_t dCBWTag;
   uint32_t dCBWDataTransferLength;
   uint8_t  bCBWFlags;
   uint8_t  bCBWLUN;
   uint8_t  bCBWCBLength;
   uint8_t  CBWCB[16];
} cbw_t;

typedef struct csw
{
   uint32_t CSWSignatur;
   uint32_t CSWTag;
   uint32_t CSWDataResidue;
   uint8_t CSWStatus;
} csw_t;

// Response struct
typedef struct InquiryResponse
{
   uint8_t Peripheral;
   uint8_t Removable;
   uint8_t Version;
   uint8_t ResponseDataFormat;
   uint8_t AdditionalLength;
   uint8_t Sccstp;
   uint8_t Bqueetc;
   uint8_t CmdQue;
   uint8_t VendorID[8];
   uint8_t ProductID[16];
   uint8_t ProductRev[4];
} InquiryResponse_t;


// 8 byte capacity value buffer
typedef struct read_capacity
{
   uint32_t SectorCount;
   uint32_t SectorSize;
} read_capacity_t;


// Functions
typedef struct RequestSenseResponse
{
    uint16_t ResponseCode :7;
    uint16_t VALID :1;
    uint8_t Obsolete;
    uint16_t SenseKey :4;
    uint16_t Resv :1;
    uint16_t ILI :1;
    uint16_t EOM :1;
    uint16_t FILEMARK :1;
    uint32_t Information;
    uint8_t AddSenseLen;
    uint32_t CmdSpecificInfo;
    uint8_t ASC;
    uint8_t ASCQ;
    uint8_t FRUC;
    uint8_t SenseKeySpecific[3];
} RequestSenseResponse_t;

typedef struct StorageInfo
{
    uint8_t STORAGE_CMD;
    uint16_t  BUFFER_SIZE;
    uint8_t*  BUFFER;
    uint16_t  IN_EP;
    uint16_t  OUT_EP;
    uint32_t BLOCK_START;
    uint8_t BLOCK_CNT;
    uint16_t  STATUS;
    uint8_t SEND_CBW;
    uint8_t GET_CSW;
    uint8_t CSW_STATUS;
    uint16_t  BYTES_TRANSFERRED;
    uint16_t  ERROR_CODE;
    uint8_t MAX_LUN;
} StorageInfo_t;


typedef struct inquiry_response
{
    uint8_t Peripheral;
    uint8_t Removable;
    uint8_t Version;
    uint8_t ResponseDataFormat;
    uint8_t AdditionalLength;
    uint8_t Sccstp;
    uint8_t Bqueetc;
    uint8_t CmdQue;
    uint8_t VendorID[8];
    uint8_t ProductID[16];
    uint8_t ProductRev[4];
} inquiry_response_t;

typedef struct ModeSenseResponse
{  
    uint8_t ModeDataLength;   
    uint8_t MediumType;     
    uint8_t Reserved1     :4;
    uint8_t DPOFUA        :1;
    uint8_t Reserved2     :2;
    uint8_t WriteProtect  :1; 
    uint8_t BlockDescriptorLength;
} ModeSenseResponse_t;

typedef struct StorageReadWrite
{
    uint8_t data[512];
} StorageReadWrite_t;


// Struct for write data
typedef struct StorageWrite
{
    uint8_t data[10];
} StorageWrite_t;


/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/



/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

void UsbMassStorage_Init();
boolean_t MassStorageDeinitHandler();
boolean_t MassStorageInitHandler(uint8_t* pu8Configuration, uint32_t u32Length);
void UsbMassStorage_BulkInCompletion();
void UsbMassStorage_BulkOutCompletion();
void UsbMassStorage_InTransferDone(void);
void UsbMassStorage_OutTransferDone(void);
void UsbMassStorage_CheckForCompletion(volatile uint8_t *NAKDetected);

StorageInfo_t* UsbMassStorage_GetGlobalStorageInfo();
int16_t UsbMassStorage_StorageCommandSend(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_SendCBW(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_Transmit_MSDC_Data(uint8_t *pu8Buffer, StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_Transmit_Data(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_GetCSW(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_CheckCSW(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_BulkOnlyMSReset(void);
int16_t UsbMassStorage_StorageStatus(void);
int16_t UsbMassStorage_ResetRecovery(void);
int16_t UsbMassStorage_ClearInEndpoint(void);
int16_t UsbMassStorage_ClearOutEndpoint(void);
void UsbMassStorage_GetEPStatus(uint8_t* InEpStatus, uint8_t* OutEpStatus);
int UsbMassStorage_TestUnitReady(StorageInfo_t *pstcStorageInfo);

void MassStorageSetupComplete(uint8_t* pu8Buffer, uint32_t u32DataSize);
void MassStorageWaitForSetupComplete();

void convBufferToSenseResponse(uint8_t *buffer, struct RequestSenseResponse *sr) ;
void convBufferToModeSenseResponse(uint8_t *buffer, struct ModeSenseResponse *msr);
void convBufferToInquiryResponse(uint8_t *buffer, InquiryResponse_t *ir);
void convCBWtoBuffer(cbw_t *cbw, uint8_t *Buffer);
int
convBufferToCSW(
    uint8_t *buffer,      // buffer that contains a status wrapper block  
    int length,                 // length of the buffer
    csw_t *csw
);

/*
void UsbMassStorage_BulkInCompletion(UMH_STATUS Status,unsigned int BytesTransferred,void* Context);
void UsbMassStorage_BulkOutCompletion(UMH_STATUS Status,unsigned int BytesTransferred,void* Context);


void UsbMassStorage_Init();
StorageInfo_t* UsbMassStorage_GetGlobalStorageInfo();

void UsbMassStorage_CheckForCompletion(uint8_t *NAKDetected);
void UsbMassStorage_InTransferDone(void);
void UsbMassStorage_OutTransferDone(void);
int16_t UsbMassStorage_StorageCommandSend(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_SendCBW(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_Transmit_MSDC_Data(uint8_t *pu8Buffer, StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_Transmit_Data(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_GetCSW(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_CheckCSW(StorageInfo_t *pstcStorageInfo);
int16_t UsbMassStorage_BulkOnlyMSReset(void);
int16_t UsbMassStorage_StorageStatus(void);
int16_t UsbMassStorage_ResetRecovery(void);
int16_t UsbMassStorage_ClearInEndpoint(void);
int16_t UsbMassStorage_ClearOutEndpoint(void);
int UsbMassStorage_TestUnitReady(StorageInfo_t *pstcStorageInfo);
void UsbMassStorage_GetEPStatus(uint8_t* InEpStatus, uint8_t* OutEpStatus);


void UMH_StallExecutionUserCallback(unsigned long time);
void Idle(void);
*/
#endif /* (USE_USB_HOST == 1) */
#endif /* __USBMASSSTORAGE_H__*/
