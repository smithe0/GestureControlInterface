#include <altera_up_avalon_usb_regs.h>
#include <stdio.h>
#include "device_info.h"
#include "usb_commands.h"
#include "system.h"

unsigned short info_CheckChipID(void)
{
    unsigned short CHIP_ID;
    unsigned char LOW_ID, HIGH_ID;
    CHIP_ID = usb_ReadChipID();
    LOW_ID = (unsigned char)CHIP_ID;
    HIGH_ID = (unsigned char)(CHIP_ID >> 8) ;

    switch(HIGH_ID)
    {
     case 0x61 : {

    	 	 	 	 	 printf("               CHIP ID = 0x%04x\n\n",CHIP_ID);
    	 	 	 	 	 CHIP_ID =0x1161;
    	 	 	 	 	 return CHIP_ID;
                         break;
                 }

     case 0x36 : {
                         printf("               CHIP ID = 0x%04x\n\n",CHIP_ID);
                         CHIP_ID = 0x1362;
                         return CHIP_ID;
                         break;
                 }

     default   : {
                 printf("               UNKNOWN CHIP ID =0x%04x\n\n",CHIP_ID);
                 return CHIP_ID;
                 break;
                 }
    }
}

/*
 * An old method for setting values in structs
 *
void info_SetConfigDescriptor(struct configuration_descriptor *cd)
{
	cd->bLength = sizeof(struct configuration_descriptor);
	cd->bDescriptorType = USB_CONFIGURATION_DESCRIPTOR_TYPE;
	cd->bTotalLength = sizeof(struct configuration_descriptor);
	cd->bNumInterfaces = 2;
	cd->bConfigurationValue = 1;
	cd->iConfiguration = 0;
	cd->bmAttributes = 0xC0; //This is for no remote wake up -- Probably look more into this
	cd->MaxPower = 0x1; //Check how they arrived at this
}

void info_SetTestInterfaceDescriptor(struct interface_descriptor *id)
{
	id->bLength = sizeof(struct interface_descriptor);
	id->bDescriptorType = USB_INTERFACE_DESCRIPTOR_TYPE;
	id->bInterfaceNumber = 0;
	id->bAlternateSetting = 0;
	id->bNumEndpoints = 1;
	id->bInterfaceClass = USB_DEVICE_CLASS_COMMUNICATIONS; // Need to look up what this means specifically
	id->bInterfaceSubClass = USB_SUBCLASS_CODE_UNKNOWN;
	id->bInterfaceProtocol = USB_PROTOCOL_CODE_UNKNOWN;
	id->iInterface = STR_INDEX_INTERFACE;
}

void info_SetHIDKeyboardInterfaceDescriptor(struct interface_descriptor *id)
{
	id->bLength = sizeof(struct interface_descriptor);
	id->bDescriptorType = USB_INTERFACE_DESCRIPTOR_TYPE;
	id->bInterfaceNumber = 1;
	id->bAlternateSetting = 1;
	id->bNumEndpoints = 1;
	id->bInterfaceClass = USB_DEVICE_CLASS_HUMAN_INTERFACE; // I assume this is correct but even so continue reading HID and class specification. There is probably more to do
	id->bInterfaceSubClass = USB_SUBCLASS_CODE_UNKNOWN;
	id->bInterfaceProtocol = USB_PROTOCOL_CODE_UNKNOWN;
	id->iInterface = STR_INDEX_INTERFACE;
}

void info_SetHIDMouseInterfaceDescriptor(struct interface_descriptor *id)
{
	id->bLength = sizeof(struct interface_descriptor);
	id->bDescriptorType = USB_INTERFACE_DESCRIPTOR_TYPE;
	id->bInterfaceNumber = 2;
	id->bAlternateSetting = 2;
	id->bNumEndpoints = 1;
	id->bInterfaceClass = USB_DEVICE_CLASS_HUMAN_INTERFACE;
	id->bInterfaceSubClass = USB_SUBCLASS_CODE_UNKNOWN;
	id->bInterfaceProtocol = USB_PROTOCOL_CODE_UNKNOWN;
	id->iInterface = STR_INDEX_INTERFACE;
}

void info_SetEndpointDescriptor(struct endpoint_descriptor *ed, )
{
	ed->bLength = 1;
	ed->bDescriptorType;
	ed->bEndpointAddress;
	ed->bmAttributes;
	ed->wMaxPacketSize;
	ed->bInterval;
}

void info_SetStringDescriptor(struct string_descriptor *sd) // Okay so there are many of these. I'll pass for now to see if they are strictly mandatory. Its possible
{

}

void info_SetDeviceDescriptor(struct device_descriptor *dd)
{
	dd->bLength = 18;
	dd->bDescriptorType = USB_DEVICE_DESCRIPTOR_TYPE;
	dd->bcdUSB_L = 0x10;
	dd->bcdUSB_H = 0x01;
	dd->bDeviceClass = 0xdc;		// I should read up on classes after this
	dd->bDeviceSubClass = 0x00;		// to see if I can get HID working
	dd->bDeviceProtocol = 0x00;		//
	dd->bMaxPacketSize0 = 64;
	dd->idVendor_L = 0x71; // Vendor ID for PHILIPS. Alternate could be 0x4cc
	dd->idVendor_H = 0x04;
	dd->idProduct_L = 0x66; // Product ID for ISP 1362. Alternate 0x1a62
	dd->idProduct_H = 0x6;
	dd->bcdDevice_L = 0x00;
	dd->bcdDevice_H = 0x01;
	dd->iManufacturer = 0;	//STR_INDEX_MANUFACTURER
	dd->iProduct = 0;		//STR_INDEX_PRODUCT
	dd->iSerialNumber = 0;	//STR_INDEX_SERIALNUMBER
	dd->bNumConfigurations = 0x19;
}
*/
