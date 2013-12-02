/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "../UsbMassStorage.h"
#include "../StorageApi.h"
//#include "usb.h"
#include "Arduino.h"

#if (USE_USB_HOST == 1)
/*-----------------------------------------------------------------------*/
/* Correspondence between drive number and physical drive                */
/* Note that Tiny-FatFs supports only single drive and always            */
/* accesses drive number 0.                                              */

/*
#define ATA		0    //if there are more flashdrives, use these numerations
#define MMC		1    
#define USB		2
*/
#define USB     0   



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
	int result;

	switch (drv) 
	{
	   case USB :      //There is no need to initialize the disk everytime
	                  //before writing or reading
	  result = 0;
	   
	   
		// translate the reslut code here
        if(result)
		{
		   stat = STA_NOINIT;
		}
		else
		{
		   
		   stat = 0;
		}
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
	int result;

	switch (drv) {
	case USB :

		//There is also no need for checking the disk everytime
		//before writing or reading if there is a write protection.
		//If you want to use it, use the API function ModeSense();
		
		result = 0;
        if(result)
		{
		   stat = STA_PROTECT;
		}
		else
		{ 
		 stat = 0;
		}
		return stat;
		

	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector number (LBA) */
	BYTE count		/* Sector count (1..255) */
)
{
	DRESULT res;
	int result;

	switch (drv) {
	case USB :
		result = MSD_Read(buff, sector, count);
		// translate the reslut code here
		if(result == 0x02)
		{
		   res = RES_NOTRDY;
		}
		else if(result == 0x03)
		{
		   res = RES_PARERR;
		}
        else
		{
		   res = (DRESULT)result;
		}
		return res;

	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	DRESULT res;
	int result;

	switch (drv) {
	case USB :
		result = (DRESULT)MSD_Write((BYTE*)buff, sector, count);
		// translate the reslut code here
     		if(result == 0x02)
		{
		   res = RES_NOTRDY;
		}
		else if(result == 0x03)
		{
		   res = RES_PARERR;
		}
        else
		{
		   res = (DRESULT)result;
		}   
		return res;


	}
	return RES_PARERR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    unsigned char tmp[4];
    unsigned long *ptmp;
    read_capacity_t ReadCapacity;
    DRESULT res; 
    
 	switch (drv) 
 	{
	case USB :   
    
	res = (DRESULT)MSD_Read_Capacity(&ReadCapacity);
	switch (ctrl) 
	{
		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
		

		    tmp[0] = (ReadCapacity.SectorCount >> 24);
		    tmp[1] = (ReadCapacity.SectorCount >> 16);
		    tmp[2] = (ReadCapacity.SectorCount >>  8);
		    tmp[3] = (ReadCapacity.SectorCount);
		    ptmp = (unsigned long*) &tmp[0];
			*(DWORD*)buff = *ptmp;
			return res;
			

		case GET_BLOCK_SIZE :	/* Get erase block size in sectors (DWORD) */
		
		    tmp[0] = (ReadCapacity.SectorSize >> 24);
		    tmp[1] = (ReadCapacity.SectorSize >> 16);
		    tmp[2] = (ReadCapacity.SectorSize >>  8);
		    tmp[3] = (ReadCapacity.SectorSize);
		    ptmp = (unsigned long*) &tmp[0];
			*(DWORD*)buff = (*ptmp)/(512);
			return res;

		case CTRL_SYNC :	/* Nothing to do */
			return RES_OK;


		default:
			return RES_PARERR;

	}
	default:
	return RES_PARERR;
	}

}


//Get FATTIME DUMMY

DWORD get_fattime (void){
return 0x2210821; //01.01.1981 01:01
}
#endif
