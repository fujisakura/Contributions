/*
 USB Storage FatFs Example
 
 Shows how to use udisk with FAT file system.
  
 created 11 Nov 2013
 by Spansion 
 */

// Require USB Storage FatFs library
#include <UsbStorageFatFs.h>

// Initialize USB Controller
USBHost usb;

// Attach usb storage fatfs to USB
UsbStorageFatFs udisk(usb);

#define TEST_DIR "TEST_DIR"
#define UPPER_DIR ".."
#define TEST_FILE "test.txt"
#define TEST_STR "This is a test string writed to file!"

void setup()
{
  Serial.begin(9600);
  Serial.println("Program started");
  delay(200);

  while(!udisk.begin());
  Serial.println("USB Disk is ready!");

  Serial.print("Total size(MB):");
  Serial.println(udisk.gettotal());
  Serial.print("Free  size(MB):");
  Serial.println(udisk.getfree());
  
  if(udisk.exists(TEST_DIR))
  {
     Serial.println("Directory exists!");
     if(udisk.chdir(TEST_DIR))
     {
       Serial.println("chdir OK!");
       if(udisk.exists(TEST_FILE))
       {
         Serial.println("File exists!");
         if(udisk.remove(TEST_FILE))
           Serial.println("Remove file OK!");         
       }
     }
     if(udisk.chdir(UPPER_DIR))
     {
       Serial.println("Back to upper directory OK!");
       if(udisk.exists(TEST_DIR))
       {
         Serial.println("Directory exists!");
         if(udisk.rmdir(TEST_DIR))
           Serial.println("Remove directory OK!");
       }
     }
  }

  if(udisk.exists(TEST_DIR) || udisk.mkdir(TEST_DIR))
  {
    Serial.println("TEST_DIR OK!");
    if(udisk.chdir(TEST_DIR))
    {
      Serial.println("chdir OK!");
      if(udisk.fopen(TEST_FILE,FA_READ | FA_WRITE | FA_CREATE_NEW))
      {
        Serial.println("fopen OK!");
        if(udisk.fwrite(TEST_STR, sizeof(TEST_STR) - 1))
          Serial.println("fwrite OK!");
        if(udisk.fclose())
          Serial.println("fclose OK!");
      }
    }
  }
}

void loop()
{
  // Process USB tasks
  usb.Task();
}

