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
/** \file UsbDescriptors.h
 **
 ** USB Descriptors File
 **
 ** History:
 **   - 2010-9-30  1.0  MSc  Automatically Created by FME USB Descriptors Manager 
 *****************************************************************************/

#ifndef __USBDESCRIPTORS_H__
#define __USBDESCRIPTORS_H__

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/



/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define USB_DEVDESC_SIZE 18
#define USB_CNFGDESC_SIZE 48
#define USB_RPTDESC_SIZE 0    /// only used with HID
#define USB_FUNC_EP0_SIZE 64
#define CLASSNAME "UsbClass"


/*****************************************************************************/
/* Global type definitions ('typedef')                                       */
/*****************************************************************************/



/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

const uint8_t au8DeviceDescriptor[18] = 
{
    ///DEVICE DESCRIPTOR
    0x12,    ///bLength: Length of this descriptor
    0x01,    ///bDescriptorType: Device Descriptor Type
    0x01,    ///bcdUSB: USB Version
    0x01,    ///bcdUSB: USB Version
    0x02,    ///bDeviceClass: Class Code: COMMUNICATIONS_AND_CDC_CONTROL
    0x00,    ///bDeviceSubClass: Sub Class Code
    0x00,    ///bDeviceProtocol: Protocol Code
    0x40,    ///bMaxPacketSize0: Maximum size of endpoint 0
    0x55,    ///idVendor: Vendor ID
    0x1F,    ///idVendor: Vendor ID
    0x04,    ///idProduct: Product ID
    0x00,    ///idProduct: Product ID
    0x00,    ///bcdDevice: Release Number
    0x02,    ///bcdDevice: Release Number
    0x01,    ///iManufacture: String-Index of Manufacture
    0x02,    ///iProduct: String-Index of Product
    0x03,    ///iSerialNumber: String-Index of Serial Number
    0x01    ///bNumConfigurations: Number of possible configurations
};

const uint8_t au8ConfigDescriptor[48] =
{        ///NEW CONFIG DESCRIPTOR(1)
        0x09,    ///bLength: Length of this descriptor
        0x02,    ///bDescriptorType: Config Descriptor Type
        0x30,    ///wTotalLength: Total Length with all interface- and endpoint descriptors
        0x00,    ///wTotalLength: Total Length with all interface- and endpoint descriptors
        0x02,    ///bNumInterfaces: Number of interfaces
        0x01,    ///iConfigurationValue: Number of this configuration
        0x00,    ///iConfiguration: String index of this configuration
        0xC0,    ///bmAttributes: Bus-Powered, Remote-Wakeup not supported
        0xFA,    ///MaxPower: (in 2mA)
            ///NEW INTERFACE DESCRIPTOR(0)
            0x09,    ///bLength: Length of this descriptor
            0x04,    ///bDescriptorType: Interface Descriptor Type
            0x00,    ///bInterfaceNumber: Interface Number
            0x00,    ///bAlternateSetting: Alternate setting for this interface
            0x01,    ///bNumEndpoints: Number of endpoints in this interface excluding endpoint 0
            0x02,    ///iInterfaceClass: Class Code: COMMUNICATIONS_AND_CDC_CONTROL
            0x02,    ///iInterfaceSubClass: SubClass Code
            0x01,    ///bInterfaceProtocol: Protocol Code
            0x00,    ///iInterface: String index
                ///NEW ENDPOINT DESCRIPTOR(0)
                0x07,    ///bLength: Length of this descriptor
                0x05,    ///bDescriptorType: Endpoint Descriptor Type
                0x82,    ///bEndpointAddress: Endpoint address (IN,EP2)
                0x03,    ///bmAttributes: Transfer Type: INTERRUPT_TRANSFER
                0x40,    ///wMaxPacketSize: Endpoint Size
                0x00,    ///wMaxPacketSize: Endpoint Size
                0xFF,    ///bIntervall: Polling Intervall
            ///NEW INTERFACE DESCRIPTOR(1)
            0x09,    ///bLength: Length of this descriptor
            0x04,    ///bDescriptorType: Interface Descriptor Type
            0x01,    ///bInterfaceNumber: Interface Number
            0x00,    ///bAlternateSetting: Alternate setting for this interface
            0x02,    ///bNumEndpoints: Number of endpoints in this interface excluding endpoint 0
            0x0A,    ///iInterfaceClass: Class Code: CDC_DATA
            0x00,    ///iInterfaceSubClass: SubClass Code
            0x00,    ///bInterfaceProtocol: Protocol Code
            0x00,    ///iInterface: String index
                ///NEW ENDPOINT DESCRIPTOR(0)
                0x07,    ///bLength: Length of this descriptor
                0x05,    ///bDescriptorType: Endpoint Descriptor Type
                0x03,    ///bEndpointAddress: Endpoint address (OUT,EP3)
                0x02,    ///bmAttributes: Transfer Type: BULK_TRANSFER
                0x40,    ///wMaxPacketSize: Endpoint Size
                0x00,    ///wMaxPacketSize: Endpoint Size
                0x00,    ///bIntervall: Polling Intervall
                ///NEW ENDPOINT DESCRIPTOR(1)
                0x07,    ///bLength: Length of this descriptor
                0x05,    ///bDescriptorType: Endpoint Descriptor Type
                0x81,    ///bEndpointAddress: Endpoint address (IN,EP1)
                0x02,    ///bmAttributes: Transfer Type: BULK_TRANSFER
                0x40,    ///wMaxPacketSize: Endpoint Size
                0x00,    ///wMaxPacketSize: Endpoint Size
                0x00    ///bIntervall: Polling Intervall
};



const uint8_t au8ReportDescriptor[1] = {0};

const char_t* pcManufacturerStringDescriptor = "Spansion LLC";
const char_t* pcProductStringDescriptor = "Virtual Comm Port";
const char_t* pcSerialnumberStringDescriptor = "1.0";

const char_t** ppStringDescriptors[3] = 
{
    &pcManufacturerStringDescriptor,
    &pcProductStringDescriptor,
    &pcSerialnumberStringDescriptor
};


/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/



#endif /* __USBDESCRIPTORS_H__ */
