/* THIS SAMPLE CODE IS PROVIDED AS IS AND IS SUBJECT TO ALTERATIONS. Spansion*/
/* LLC ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR */
/* ELIGIBILITY FOR ANY PURPOSES.                                             */
/*                 (C) Spansion LLC                  */
/*---------------------------------------------------------------------------*/


#ifndef _STORAGE_API_H_
#define _STORAGE_API_H_
//#include "Usb.h"
#include "Arduino.h"

#if (USE_USB_HOST == 1)
#include "filesystem/diskio.h"
//defines

//UFI Command Descriptors (SCSI)

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

/*
//Structs
typedef struct _StorageInfo_t
{
unsigned char STORAGE_CMD;
unsigned int  BUFFER_SIZE;
unsigned char *BUFFER;
unsigned int  IN_EP;
unsigned int  OUT_EP;
unsigned long BLOCK_START;
unsigned char BLOCK_CNT;
unsigned int  STATUS;
unsigned char SEND_CBW;
unsigned char GET_CSW;
unsigned char CSW_STATUS;
unsigned int  BYTES_TRANSFERRED;
unsigned int  ERROR_CODE;
unsigned char MAX_LUN;
}STORAGE_INFO_t;


struct INQUIRY_RESPONSE
{
   UCHAR Peripheral;
   UCHAR Removable;
   UCHAR Version;
   UCHAR ResponseDataFormat;
   UCHAR AdditionalLength;
   UCHAR Sccstp;
   UCHAR Bqueetc;
   UCHAR CmdQue;
   UCHAR VendorID[8];
   UCHAR ProductID[16];
   UCHAR ProductRev[4];
};

struct RequestSenseResponse
{
   UCHAR ResponseCode :7;
   UCHAR VALID :1;
   UCHAR Obsolete;
   UCHAR SenseKey :4;
   UCHAR Resv :1;
   UCHAR ILI :1;
   UCHAR EOM :1;
   UCHAR FILEMARK :1;
   ULONG Information;
   UCHAR AddSenseLen;
   ULONG CmdSpecificInfo;
   UCHAR ASC;
   UCHAR ASCQ;
   UCHAR FRUC;
   UCHAR SenseKeySpecific[3];
};

struct ModeSenseResponse
{  
   UCHAR ModeDataLength;   
   UCHAR MediumType;     
   UCHAR Reserved1     :4;
   UCHAR DPOFUA        :1;
   UCHAR Reserved2     :2;
   UCHAR WriteProtect  :1; 
   UCHAR BlockDescriptorLength;
};
*/
int MSD_Read(BYTE *buff, DWORD BlockStart, BYTE BlockCnt);
int MSD_Write(BYTE *buff, DWORD BlockStart, BYTE BlockCnt);
int MSD_Read_Capacity(void (*Buff));
int StorageInit(void); 
int ModeSense(void);

#endif /* (USE_USB_HOST == 1) */
#endif	// _STORAGE_API_H_

