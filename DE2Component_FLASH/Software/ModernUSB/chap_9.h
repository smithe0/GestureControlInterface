
#ifndef CHAP_9_H
#define CHAP_9_H

// **************
// Marcos
// **************

#define MSB(x)    (((x) >> 8) & 0xFF)
#define LSB(x)    ((x) & 0xFF)

// *************************************************************************
// basic #defines
// *************************************************************************

#define MAX_ENDPOINTS 			        	                    0x0F

#define USB_CLASS_CODE_UNKNOWN  	            			    0x00
#define USB_SUBCLASS_CODE_UNKNOWN   			                0x00
#define USB_PROTOCOL_CODE_UNKNOWN                               0x00

#define USB_CLASS_CODE_MASSSTORAGE_CLASS_DEVICE   				0x08

#define USB_SUBCLASS_CODE_RBC                           		0x01
#define USB_SUBCLASS_CODE_SFF8020I                    			0x02
#define USB_SUBCLASS_CODE_QIC157                        		0x03
#define USB_SUBCLASS_CODE_UFI                           		0x04
#define USB_SUBCLASS_CODE_SFF8070I                    			0x05
#define USB_SUBCLASS_CODE_SCSI                        			0x06

#define USB_PROTOCOL_CODE_CBI0                                  0x00
#define USB_PROTOCOL_CODE_CBI1                                  0x01
#define USB_PROTOCOL_CODE_BULK                                  0x50

   // *************************************************************************
   // masks
   // *************************************************************************


#define USB_RECIPIENT            (unsigned char)0x1F
#define USB_RECIPIENT_DEVICE     (unsigned char)0x00
#define USB_RECIPIENT_INTERFACE  (unsigned char)0x01
#define USB_RECIPIENT_ENDPOINT   (unsigned char)0x02

#define USB_REQUEST_DIR_MASK     (unsigned char)0x80

#define USB_REQUEST_TYPE_MASK    (unsigned char)0x60
#define USB_STANDARD_REQUEST     (unsigned char)0x00
#define USB_CLASS_REQUEST        (unsigned char)0x20
#define USB_VENDOR_REQUEST       (unsigned char)0x40

#define USB_REQUEST_MASK         (unsigned char)0xFF

#define DEVICE_ADDRESS_MASK      0x7F

/* GetStatus */
#define DEVSTS_SELFPOWERED       0x01
#define DEVSTS_REMOTEWAKEUP      0x02

#define ENDPSTS_HALT             0x01

   // *************************************************************************
   // USB Protocol Layer
   // *************************************************************************

typedef struct _USB_STRING_LANGUAGE_DESCRIPTOR 
{
    unsigned char  bLengthL;
    unsigned char  bDescriptorTypeL;
    unsigned char ulanguageIDL;
    unsigned char ulanguageIDH;
} USB_STRING_LANGUAGE_DESCRIPTOR,*PUSB_STRING_LANGUAGE_DESCRIPTOR;

typedef struct _USB_STRING_INTERFACE_DESCRIPTOR 
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  Interface[22];
} USB_STRING_INTERFACE_DESCRIPTOR,*PUSB_STRING_INTERFACE_DESCRIPTOR;

typedef struct _USB_STRING_CONFIGURATION_DESCRIPTOR 
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  Configuration[16];
} USB_STRING_CONFIGURATION_DESCRIPTOR,*PUSB_STRING_CONFIGURATION_DESCRIPTOR;

typedef struct _USB_STRING_SERIALNUMBER_DESCRIPTOR 
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  SerialNum[24];
} USB_STRING_SERIALNUMBER_DESCRIPTOR,*PUSB_STRING_SERIALNUMBER_DESCRIPTOR;

typedef struct _USB_STRING_PRODUCT_DESCRIPTOR 
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  Product[64];
} USB_STRING_PRODUCT_DESCRIPTOR,*PUSB_STRING_PRODUCT_DESCRIPTOR;

typedef struct _USB_STRING_MANUFACTURER_DESCRIPTOR 
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  Manufacturer[54];
} USB_STRING_MANUFACTURER_DESCRIPTOR,*PUSB_STRING_MANUFACTURER_DESCRIPTOR;

// *************************************************************************
// USB standard device requests
// *************************************************************************

void chap9_GetStatus(void);
void chap9_ClearFeature(void);
void chap9_SetFeature(void);
void chap9_SetAddress(void);
void chap9_GetDescriptor(void);
void chap9_GetConfiguration(void);
void chap9_SetConfiguration(void);
void chap9_GetInterface(void);
void chap9_SetInterface(void);

// *************************************************************************
// Chap9 support functions
// *************************************************************************

void chap9_StallEP0(void);
void chap9_StallEP0InControlWrite(void);
void chap9_StallEP0InControlRead(void);
void chap9_SingleTransmitEP0(unsigned char * buf, unsigned short len);
void chap9_BurstTransmitEP0(unsigned char * pData, unsigned short len);
#endif
