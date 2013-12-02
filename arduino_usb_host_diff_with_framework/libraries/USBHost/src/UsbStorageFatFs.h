/*
  Copyright (c) 2012 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef USBSTORAGE_FATFS_H
#define USBSTORAGE_FATFS_H

#include "Arduino.h"
#include "Usb.h"

extern "C" {
#include "spansion\StorageApi.h"
#include "spansion\filesystem\ff.h"
}

class UsbStorageFatFs : public USBDeviceConfig
{
protected:
  USBHost *pUsb;

public:
  UsbStorageFatFs(USBHost &usb) : pUsb(&usb) {
    if (pUsb)
        pUsb->RegisterDeviceClass(this);
  };


  // USBDeviceConfig implementation
  virtual uint32_t Init(uint32_t parent, uint32_t port, uint32_t lowspeed) { return 0; };
  virtual uint32_t Release() { return 0; };
  virtual uint32_t Poll();
  virtual uint32_t GetAddress() { return 0; };

/* FatFs module application interface                           */
  boolean begin();								/* Begin USB disk */
  DWORD   gettotal();							/* Get total size (MB) of USB disk */
  DWORD   getfree();							/* Get Free size (MB) of USB disk */
  boolean fopen(const XCHAR*, BYTE);			/* Open or create a file */
  UINT    fread(void*, UINT);					/* Read data from a file */
  UINT    fwrite(const void*, UINT);			/* Write data to a file */
  boolean flseek(DWORD);						/* Move file pointer of a file object */
  boolean ftruncate();							/* Truncate file */
  boolean fsync();								/* Flush cached data of a writing file */
  boolean fclose();								/* Close an open file object */
  FILINFO* fstat(const XCHAR*);					/* Get file or directory status */
  boolean rename(const XCHAR*, const XCHAR*);	/* Rename/Move a file or directory */
  boolean remove(const XCHAR*);					/* Delete an existing file */
  boolean opendir(const XCHAR*);				/* Open an existing directory */
  FILINFO* readdir();							/* Read a directory item */
  boolean chdir(const XCHAR*);					/* Change current directory */
  boolean mkdir(const XCHAR*);					/* Create a new directory */
  boolean rmdir(const XCHAR*);					/* Delete an existing directory */
  boolean exists(const XCHAR*);					/* Is file or directory existing */

#if _USE_STRFUNC
  int fputc(int);								/* Put a character to the file */
  int fputs(const char*);						/* Put a string to the file */
  int fprintf(const char*, ...);				/* Put a formatted string to the file */
  char* fgets(char*, int);						/* Get a string from the file */
#endif

private:
  FATFS fatfs[1];
  FATFS *fs;
  FIL fil;
  FILINFO finfo;
  DIR dir;
  DWORD clust;
};

#endif
