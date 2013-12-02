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

#include <UsbStorageFatFs.h>

boolean UsbStorageFatFs::begin() {
	boolean ret = FALSE;

	if ((uint8_t)UsbClassSupervisor_GetActiveDriver() == USBCLASSDRIVER_MASSSTORAGE)
	{
		delay(200);
		while((StorageInit() != 0) && (UsbHost_GetDeviceStatus() != USBHOST_DEVICE_IDLE))
		{
			UsbHost_DeInit();
			delay(200);
			UsbHost_Init();
			delay(200);
		}
		if (UsbHost_GetDeviceStatus() != USBHOST_DEVICE_IDLE)
		{
			f_mount(0,fatfs);
			if (f_opendir(&dir, "0:") == FR_OK)
			{
				ret = TRUE;
			}
		}
	}

	return ret;
}

DWORD UsbStorageFatFs::gettotal() {
	DWORD ret = 0;

	if (f_getfree("0:", &clust, &fs) == FR_OK)
		ret = ((fs->max_clust - 2) * fs->csize) / 2;

	return ret/1024;
}

DWORD UsbStorageFatFs::getfree() {
	DWORD ret = 0;

	if (f_getfree("0:", &clust, &fs) == FR_OK)
		ret = (clust * fs->csize) / 2;

	return ret/1024;
}

boolean UsbStorageFatFs::fopen(const XCHAR *path, BYTE mode) {
	boolean ret = FALSE;

	if (f_open(&fil, path, mode) == FR_OK)
		ret = TRUE;

	return ret;
}

UINT UsbStorageFatFs::fread(void *buff, UINT btr) {
	UINT ret;

	if (f_read(&fil, buff, btr, &ret) != FR_OK)
		ret = 0;

	return ret;
}

UINT UsbStorageFatFs::fwrite(const void *buff, UINT btw) {
	UINT ret;

	if (f_write(&fil, buff, btw, &ret) != FR_OK)
		ret = 0;

	return ret;
}

boolean UsbStorageFatFs::flseek(DWORD ofs) {
	boolean ret = FALSE;

	if (f_lseek(&fil, ofs) == FR_OK)
		ret = TRUE;

	return ret;
}

boolean UsbStorageFatFs::ftruncate() {
	boolean ret = FALSE;

	if (f_truncate(&fil) == FR_OK)
		ret = TRUE;

	return ret;
}

boolean UsbStorageFatFs::fsync() {
	boolean ret = FALSE;

	if (f_sync(&fil) == FR_OK)
		ret = TRUE;

	return ret;
}

boolean UsbStorageFatFs::fclose() {
	boolean ret = FALSE;

	if (f_close(&fil) == FR_OK)
		ret = TRUE;

	return ret;
}

FILINFO* UsbStorageFatFs::fstat(const XCHAR *path) {
	FILINFO* ret = NULL;

	if (f_stat(path, &finfo) == FR_OK)
		ret = &finfo;

	return ret;
}

boolean UsbStorageFatFs::rename(const XCHAR *path_old, const XCHAR *path_new) {
	boolean ret = FALSE;

	if (f_rename(path_old, path_new) == FR_OK)
		ret = TRUE;

	return ret;
}

boolean UsbStorageFatFs::remove(const XCHAR *path) {
	boolean ret = FALSE;

	if (f_unlink(path) == FR_OK)
		ret = TRUE;

	return ret;
}

boolean UsbStorageFatFs::opendir(const XCHAR *path) {
	boolean ret = FALSE;

	if (f_opendir(&dir, path) == FR_OK)
		ret = TRUE;

	return ret;
}

FILINFO* UsbStorageFatFs::readdir() {
	FILINFO* ret = NULL;

	if (f_readdir(&dir, &finfo) == FR_OK)
		ret = &finfo;

	return ret;
}

boolean UsbStorageFatFs::chdir(const XCHAR *path) {
	boolean ret = FALSE;

	if (f_chdir(path) == FR_OK)
		ret = TRUE;

	return ret;
}

boolean UsbStorageFatFs::mkdir(const XCHAR *path) {
	boolean ret = FALSE;

	if (f_mkdir(path) == FR_OK)
		ret = TRUE;

	return ret;
}

boolean UsbStorageFatFs::rmdir(const XCHAR *path) {
	boolean ret = FALSE;

	if (f_unlink(path) == FR_OK)
		ret = TRUE;

	return ret;
}

boolean UsbStorageFatFs::exists(const XCHAR *filepath) {
	boolean ret = FALSE;
	FILINFO* pfinfo;

	if (!opendir("0:"))
		return ret;

	while ((pfinfo = readdir()) && pfinfo->fname[0]) 
	{
		if (strcmp((char*)pfinfo->fname, filepath) == 0)
			ret = TRUE;
	}

	return ret;
}


#if _USE_STRFUNC
int UsbStorageFatFs::fputc(int chr) {
	return f_putc(chr, &fil);
}

int UsbStorageFatFs::fputs(const char* str) {
	return f_puts(str, &fil);
}

int UsbStorageFatFs::fprintf(const char* str, ...) {
	//return f_printf(&fil, str, ...);
	return 0;
}

char* UsbStorageFatFs::fgets(char* buff, int len) {
	return f_gets(buff, len, &fil);
}
#endif


uint32_t UsbStorageFatFs::Poll() {
	return 0;
}
