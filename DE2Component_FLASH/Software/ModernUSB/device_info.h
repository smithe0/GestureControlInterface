#ifndef   DEVICE_INFO_H
#define   DEVICE_INFO_H

#define HID_MOUSE		0
#define HID_KEY			1
#define CUSTOM_UNKNOWN	0

/* Taken from the USB.h file from the Altera example project: START */
// values for the bits returned by the USB GET_STATUS command
#define USB_GETSTATUS_SELF_POWERED                0x01
#define USB_GETSTATUS_REMOTE_WAKEUP_ENABLED       0x02

#define USB_DEVICE_DESCRIPTOR_TYPE                0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE         0x02
#define USB_STRING_DESCRIPTOR_TYPE                0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE             0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE              0x05
#define USB_POWER_DESCRIPTOR_TYPE                 0x06

#define STR_INDEX_LANGUAGE                        0x00
#define STR_INDEX_MANUFACTURER                    0x01
#define STR_INDEX_PRODUCT                         0x02
#define STR_INDEX_SERIALNUMBER                    0x03
#define STR_INDEX_CONFIGURATION                   0x04
#define STR_INDEX_INTERFACE                       0x05

#define USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(d, i) ((unsigned short)((unsigned short)d<<8 | i))

// Values for bmAttributes field of an endpoint descriptor
#define USB_ENDPOINT_TYPE_MASK                    0x03

#define USB_ENDPOINT_TYPE_CONTROL                 0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS             0x01
#define USB_ENDPOINT_TYPE_BULK                    0x02
#define USB_ENDPOINT_TYPE_INTERRUPT               0x03

// definitions for bits in the bmAttributes field of a configuration descriptor
#define USB_CONFIG_POWERED_MASK                   0xc0

#define USB_CONFIG_BUS_POWERED                    0x80
#define USB_CONFIG_SELF_POWERED                   0x40
#define USB_CONFIG_REMOTE_WAKEUP                  0x20

// Endpoint direction bit, stored in address
#define USB_ENDPOINT_DIRECTION_MASK               0x80

// USB defined request codes
// see chapter 9 of the USB 1.0 specification for
// more information.
#define USB_REQUEST_GET_STATUS                    0x00
#define USB_REQUEST_CLEAR_FEATURE                 0x01

#define USB_REQUEST_SET_FEATURE                   0x03

#define USB_REQUEST_SET_ADDRESS                   0x05
#define USB_REQUEST_GET_DESCRIPTOR                0x06
#define USB_REQUEST_SET_DESCRIPTOR                0x07
#define USB_REQUEST_GET_CONFIGURATION             0x08
#define USB_REQUEST_SET_CONFIGURATION             0x09
#define USB_REQUEST_GET_INTERFACE                 0x0A
#define USB_REQUEST_SET_INTERFACE                 0x0B
#define USB_REQUEST_SYNC_FRAME                    0x0C

//Device Classes
#define MAX_ENDPOINTS	                    	0x0F

#define USB_CLASS_CODE_UNKNOWN              	0x00
#define USB_SUBCLASS_CODE_UNKNOWN           	0x00
#define USB_PROTOCOL_CODE_UNKNOWN           	0x00

#define USB_DEVICE_CLASS_RESERVED           	0x00
#define USB_DEVICE_CLASS_AUDIO              	0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS     	0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE    	0x03
#define USB_DEVICE_CLASS_MONITOR            	0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE 	0x05
#define USB_DEVICE_CLASS_POWER              	0x06
#define USB_DEVICE_CLASS_PRINTER            	0x07
#define USB_DEVICE_CLASS_STORAGE            	0x08
#define USB_DEVICE_CLASS_HUB                	0x09
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC    	0xFF

#define USB_SUBCLASS_HID_BOOTINTERFACE			0x01

#define USB_HID_INTERFACE_PROTOCOL_KEYBOARD		0x01
#define USB_HID_INTERFACE_PROTOCOL_MOUSE		0x02


// USB defined Feature selectors
#define USB_FEATURE_ENDPOINT_STALL          0x0000
#define USB_FEATURE_REMOTE_WAKEUP           0x0001
#define USB_FEATURE_POWER_D0                0x0002
#define USB_FEATURE_POWER_D1                0x0003
#define USB_FEATURE_POWER_D2                0x0004
#define USB_FEATURE_POWER_D3                0x0005
/* Taken from the USB.h file from the Altera example project: END */

//**************************************//
// USB Protocol Descriptor Structs		//
//**************************************//

struct complete_configuration_descriptor {
	unsigned char bLength_cfg;
	unsigned char bDescriptorType_cfg;
    unsigned int bTotalLength_cfg;
    unsigned char bNumInterfaces_cfg;
    unsigned char bConfigurationValue_cfg;
    unsigned char iConfiguration_cfg;
    unsigned char bmAttributes_cfg;
    unsigned char MaxPower_cfg;

#if CUSTOM_UNKNOWN
    //First Interface Descriptor (Communication test interface)
	unsigned char bLength_intfce0;
	unsigned char bDescriptorType_intfce0;
	unsigned char bInterfaceNumber_intfce0;
	unsigned char bAlternateSetting_intfce0;
	unsigned char bNumEndpoints_intfce0;
	unsigned char bInterfaceClass_intfce0;
	unsigned char bInterfaceSubClass_intfce0;
	unsigned char bInterfaceProtocol_intfce0;
	unsigned char iInterface_intfce0;

	//Comm Test Endpoint
	unsigned char bLength_endp1;
	unsigned char bDescriptorType_endp1;
	unsigned char bEndpointAddress_endp1;
	unsigned char bmAttributes_endp1;
	unsigned short wMaxPacketSize_endp1;
	unsigned char bInterval_endp1;
#endif

#if HID_KEY //Removed for testing and IO demo (presumably). Incomplete HID functionality. ReAdded for HID testing.
    //Second Interface Descriptor (HID keyboard)
	unsigned char bLength_intfce_key;
	unsigned char bDescriptorType_intfce_key;
	unsigned char bInterfaceNumber_intfce_key;
	unsigned char bAlternateSetting_intfce_key;
	unsigned char bNumEndpoints_intfce_key;
	unsigned char bInterfaceClass_intfce_key;
	unsigned char bInterfaceSubClass_intfce_key;
	unsigned char bInterfaceProtocol_intfce_key;
	unsigned char iInterface_intfce_key;

	//HID descriptor
	unsigned char 	bLength_key;				// Valid length in bytes
	unsigned char 	bDescriptorType_key;		// 0x21 for HID
	unsigned short	bcdHID_key;					// Version of HID device is compliant with
	unsigned char	bCountryCode_key;			// Set to zero for no specific localization
	unsigned char 	bNumDescriptors_key;		// Number of descriptor children
	unsigned char 	bNextDescriptorType_key;	// Same as bDescriptorType but for the children. Usually report.
	unsigned short	wDescriptorLength_key;		// Length of report descriptor

	//Keyboard Interrupt Endpoint
	unsigned char bLength_endp_key;
	unsigned char bDescriptorType_endp_key;
	unsigned char bEndpointAddress_endp_key;
	unsigned char bmAttributes_endp_key;
	unsigned short wMaxPacketSize_endp_key;
	unsigned char bInterval_endp_key;
#endif

#if HID_MOUSE
	//Third Interface Descriptor (HID Mouse)
	unsigned char bLength_intfce_mouse;
	unsigned char bDescriptorType_intfce_mouse;
	unsigned char bInterfaceNumber_intfce_mouse;
	unsigned char bAlternateSetting_intfce_mouse;
	unsigned char bNumEndpoints_intfce_mouse;
	unsigned char bInterfaceClass_intfce_mouse;
	unsigned char bInterfaceSubClass_intfce_mouse;
	unsigned char bInterfaceProtocol_intfce_mouse;
	unsigned char iInterface_intfce2;

	//HID descriptor
	unsigned char 	bLength_mouse;				// Valid length in bytes
	unsigned char 	bDescriptorType_mouse;		// 0x21 for HID
	unsigned short	bcdHID_mouse;					// Version of HID device is compliant with
	unsigned char	bCountryCode_mouse;			// Set to zero for no specific localization
	unsigned char 	bNumDescriptors_mouse;		// Number of descriptor children
	unsigned char 	bNextDescriptorType_mouse;	// Same as bDescriptorType but for the children. Usually report.
	unsigned short	wDescriptorLength_mouse;		// Length of report descriptor

	//Mouse Interrupt Endpoint
	unsigned char bLength_endp_mouse;
	unsigned char bDescriptorType_endp_mouse;
	unsigned char bEndpointAddress_endp_mouse;
	unsigned char bmAttributes_endp_mouse;
	unsigned short wMaxPacketSize_endp_mouse;
	unsigned char bInterval_endp_mouse;
#endif
};

// 	DeviceDescriptor
//  Suffixes of _L and _H indicate low and high bytes of a value that is supposed to be 2 bytes long
struct device_descriptor {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bcdUSB_L;
	unsigned char bcdUSB_H;
	unsigned char bDeviceClass;
	unsigned char bDeviceSubClass;
	unsigned char bDeviceProtocol;
	unsigned char bMaxPacketSize0;
	unsigned char idVendor_L;
	unsigned char idVendor_H;
	unsigned char idProduct_L;
	unsigned char idProduct_H;
	unsigned char bcdDevice_L;
	unsigned char bcdDevice_H;
	unsigned char iManufacturer;
	unsigned char iProduct;
	unsigned char iSerialNumber;
	unsigned char bNumConfigurations;
};

// When a configuration descriptor request is made, config, interface, and endpoint descriptors are sent.
struct configuration_descriptor{
	unsigned char bLength;
	unsigned char bDescriptorType;
    unsigned int bTotalLength;
    unsigned char bNumInterfaces;
    unsigned char bConfigurationValue;
    unsigned char iConfiguration;
    unsigned char bmAttributes;
    unsigned char MaxPower;
};

struct interface_descriptor{
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bInterfaceNumber;
	unsigned char bAlternateSetting;
	unsigned char bNumEndpoints;
	unsigned char bInterfaceClass;
	unsigned char bInterfaceSubClass;
	unsigned char bInterfaceProtocol;
	unsigned char iInterface;
};

struct endpoint_descriptor{
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bEndpointAddress;
	unsigned char bmAttributes;
	unsigned short wMaxPacketSize;
	unsigned char bInterval;
};

struct string_descriptor{
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bString[1];
};

struct HID_report_descriptor {

};

struct HID_descriptor {
	unsigned char 	bLength;			// Valid length in bytes
	unsigned char 	bDescriptorType;	// 0x21 for HID
	unsigned short	bcdHID;				// Version of HID device is compliant with
	unsigned char	bCountryCode;		// Set to zero for no specific localization
	unsigned char 	bNumDescriptors;	// Number of descriptor children
	unsigned char 	bNextDescriptorType;	//
	unsigned short	wDescriptorLength;	// Length of report descriptor
};

struct power_descriptor{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bCapabilitiesFlags;
    unsigned short EventNotification;
    unsigned short D1LatencyTime;
    unsigned short D2LatencyTime;
    unsigned short D3LatencyTime;
    unsigned char PowerUnit;
    unsigned short D0PowerConsumption;
    unsigned short D1PowerConsumption;
    unsigned short D2PowerConsumption;
};

//***************************************//
// Flow Control Flags (Global variables) //
//***************************************//

#define  DEV_BEHAVIOUR_NOT_SPECIFIED  0

typedef union _USBCHECK_DEVICE_STATES
{
    struct _USBCHECK_FLAGS
    {
        unsigned char   DEVICE_DEFAULT_STATE       : 1;
        unsigned char   DEVICE_ADDRESS_STATE       : 1;
        unsigned char   DEVICE_CONFIGURATION_STATE : 1;
        unsigned char   DEVICE_INTERFACE_STATE     : 1;
        unsigned char   FOUR_RESERVED_BITS         : 8;
        unsigned char   RESET_BITS                 : 8;
    }State_bits;

    unsigned char MY_WAY;
}USBCHECK_DEVICE_STATES;

typedef union _D13FLAGS
{
    struct _D13FSM_FLAGS
    {
    	// Byte 0
        unsigned char    bus_reset					: 1;
        unsigned char    suspend					: 1;
        unsigned char    At_IRQL1					: 1;
        unsigned char    configuration				: 1;
        unsigned char    remote_wakeup				: 1;
        unsigned char    Reserved00         		: 3;
        // Byte 1
        unsigned char    DCP_state					: 4;
        unsigned char    DCP_IOorMEM				: 1;
        unsigned char    ISO_state					: 3;
        // Byte 2
        unsigned char      setup_dma                : 1;
        unsigned char      control_write_processed  : 1;
        unsigned char      dma_disable              : 1;
        unsigned char      dma_state                : 2;
        unsigned char      toggle_bit_corrected    	: 1;
        unsigned char      control_out_done         : 1;
        unsigned char      Reserved20               : 1;
        // Byte 3
        unsigned char      verbose                  : 1;
        unsigned char      timer               		: 1;
        unsigned char      dbg                 		: 1;
        unsigned char      Reserved30          		: 5;
    } bits;

    unsigned long value;

} D13FLAGS;

typedef struct _device_request
{
	unsigned char bmRequestType;
	unsigned char bRequest;

    unsigned short int  wValue;
    unsigned short int  wIndex;
    unsigned short int  wLength;
} DEVICE_REQUEST;

typedef union Address {
    unsigned short  IoPort;     /* IoData Port */
    unsigned char * pData;      /* Memory Address */
} ADDRESS;

#define MAX_CONTROLDATA_SIZE    16
typedef struct _CONTROL_XFER
{
    unsigned char      		Abort;      /*Indicate this transfer shall be aborted ASAP*/
    DEVICE_REQUEST   		DeviceRequest;
    unsigned short          wLength;
    unsigned short          wCount;
    ADDRESS          		Addr;
    unsigned char           dataBuffer[MAX_CONTROLDATA_SIZE];

} CONTROL_XFER, * PCONTROL_XFER;

typedef struct _IO_REQUEST
{
    unsigned short  uAddressL;
    unsigned char   bAddressH;
    unsigned short  uSize;
    unsigned char   bCommand;
} IO_REQUEST, *PIO_REQUEST;






//***********************
// Device Info Functions
//***********************

unsigned short info_CheckChipID(void);

void info_SetDeviceDescriptor(struct device_descriptor *dd);
void info_SetConfigDescriptor(struct configuration_descriptor *cd);
void info_SetTestInterfaceDescriptor(struct interface_descriptor *id);
void info_SetHIDKeyboardInterfaceDescriptor(struct interface_descriptor *id);
void info_SetHIDMouseInterfaceDescriptor(struct interface_descriptor *id);
void info_SetEndpointDescriptor(struct endpoint_descriptor *ed);
void info_SetStringDescriptor(struct string_descriptor *sd);


#endif

