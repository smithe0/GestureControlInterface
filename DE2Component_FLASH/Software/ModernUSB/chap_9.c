
#include <stdio.h>
#include <string.h>
#include "chap_9.h"
#include "device_info.h"
#include "usb_commands.h"
#include "isr.h"


// *************************************************************************
// Public Data
// *************************************************************************

extern D13FLAGS bD13flags;
extern USBCHECK_DEVICE_STATES bUSBCheck_Device_State;
extern CONTROL_XFER ControlData;

unsigned char ALTERNATIVE_SETTING = 0;

// *************************************************************************
// USB Device Descriptor et al
// *************************************************************************

struct device_descriptor DeviceDesc =
{
		18,							//bLength
		USB_DEVICE_DESCRIPTOR_TYPE,	//bDescriptorType
		0x10,						//bcdUSB_L
		0x01,						//bcdUSB_H
		0x00,						//bDeviceClass	-- Either 0x00 (defer to interface class) OR 0x03 HID class (But I don't know about multiple interfaces in this mode)
		0x00,						//bDeviceSubClass
		0x00,						//bDeviceProtocol
		64,							//bMaxPacketSize0
		0xcc,						//idVendor_L -- Vendor ID for PHILIPS. There is an alternate, see CD code
		0x04,						//idVendor_H
		0x66,						//idProduct_L -- Product ID for ISP 1362. Alternate 0x1a62
		0x6,						//idProduct_H
		0x00,						//bcdDevice_L
		0x01,						//bcdDevice_H
		0,							//iManufacturer --STR_INDEX_MANUFACTURER
		0,							//iProduct --STR_INDEX_PRODUCT
		0,							//iSerialNumber --STR_INDEX_SERIALNUMBER
		1,							//bNumConfigurations
};

struct complete_configuration_descriptor CompleteConfigDesc =
{
		//Configuration Descriptor
		sizeof(struct configuration_descriptor), 	//bLength
		USB_CONFIGURATION_DESCRIPTOR_TYPE,			//bDescriptorType
		sizeof(struct configuration_descriptor),	//bTotalLength
		1,		//bNumInterfaces
		1,		//bConfigurationValue
		0,		//iConfiguration
		0xC0, 	//bmAttributes: This is for no remote wake up -- Probably look more into this
		0x1 	//MaxPower: Self Powered

#if CUSTOM_UNKNOWN
		,
		/* Custom Interface Descriptor */
		sizeof(struct interface_descriptor), //bLength
		USB_INTERFACE_DESCRIPTOR_TYPE,		 //bDescriptorType
		0,	//bInterfaceNumber
		0,	//bAlternateSetting
		1,	//bNumEndpoints
		USB_CLASS_CODE_UNKNOWN, 	//bInterfaceClass
		USB_SUBCLASS_CODE_UNKNOWN,	//bInterfaceSubClass
		USB_PROTOCOL_CODE_UNKNOWN,	//bInterfaceProtocol
		STR_INDEX_INTERFACE,	//iInterface

		/* Custom Interface Endpoint 1. EP01 Interrupt IN */
		sizeof(struct endpoint_descriptor), //bLength
		USB_ENDPOINT_DESCRIPTOR_TYPE,		//bDescriptorType
		0x81,								//bEndpointAddress --ENDP01 IN (I think the 8 means IN and 0 is OUT)
		USB_ENDPOINT_TYPE_INTERRUPT,		//bmAttributes
		0x0008,								//wMaxPacketSize
		0x0A

#endif

#if HID_KEY
		,
		/* Interface Descriptor: HID Keyboard */
		sizeof(struct interface_descriptor),  //bLength
		USB_INTERFACE_DESCRIPTOR_TYPE,	//descriptor type
		1,	// Interface Number
		1,	// Alternate Number
		1,  // Number of Endpoints
		USB_DEVICE_CLASS_HUMAN_INTERFACE, 		// Interface Class
		USB_SUBCLASS_CODE_UNKNOWN, //USB_SUBCLASS_HID_BOOTINTERFACE,			// SubClass : Usually zero unless you're an HID
		USB_PROTOCOL_CODE_UNKNOWN, //USB_HID_INTERFACE_PROTOCOL_KEYBOARD,	// Protocol : Also mostly only used in HIDs
		STR_INDEX_INTERFACE,

		/* HID Descriptor: Keyboard */
		sizeof(struct HID_descriptor), //bLength
		0x21,		// This is of HID descriptor type
		0x0110,		// HID spec. version
		0,			// No country code
		1,			// One additional descriptor
		0x22,		// of type HID Report
		sizeof(struct HID_report_descriptor),

		/* HID Keyboard Endpoint Descriptor: ENDP02 Interrupt IN */
		sizeof(struct endpoint_descriptor), //bLength
		USB_ENDPOINT_DESCRIPTOR_TYPE,		//bDescriptorType
		0x82,								//bEndpointAddress --ENDP02 IN (I think the 8 means IN and 0 is OUT)
		USB_ENDPOINT_TYPE_INTERRUPT,		//bmAttributes
		0x0004,								//wMaxPacketSize -- This is smaller than the buffer size, but the example of an HID mouse had a max packet size of 4 bytes. Keyboard might be larger.
		0x0A								//bInterval

#endif
#if HID_MOUSE
		,
		/* Interface HID Mouse */
		sizeof(struct interface_descriptor),  //bLength
		USB_INTERFACE_DESCRIPTOR_TYPE,	//descriptor type
		2,	// Interface Number
		2,	// Alternate Number
		1,  // Number of Endpoints
		USB_DEVICE_CLASS_HUMAN_INTERFACE, 		// Interface Class
		USB_SUBCLASS_CODE_UNKNOWN, //USB_SUBCLASS_HID_BOOTINTERFACE,		// SubClass : Usually zero unless you're an HID
		USB_PROTOCOL_CODE_UNKNOWN, //USB_HID_INTERFACE_PROTOCOL_KEYBOARD,	// Protocol : Also mostly only used in HIDs
		STR_INDEX_INTERFACE,

		/* HID Descriptor: Mouse */
		sizeof(struct HID_descriptor), //bLength
		0x21,		// This is of HID descriptor type
		0x0110,		// HID spec. version
		0,			// No country code
		1,			// One additional descriptor
		0x22,		// of type HID Report
		sizeof(struct HID_report_descriptor),

		/* HID Mouse Endpoint Descriptor: ENDP03 Interrupt IN */
		sizeof(struct endpoint_descriptor), //bLength
		USB_ENDPOINT_DESCRIPTOR_TYPE,		//bDescriptorType
		0x83,								//bEndpointAddress --ENDP03 IN
		USB_ENDPOINT_TYPE_INTERRUPT,		//bmAttributes
		0x0004,								//wMaxPacketSize
		0x0A								//Polled every x frames
#endif
};

struct configuration_descriptor ConfigDesc =
{
		sizeof(struct configuration_descriptor), 	//bLength
		USB_CONFIGURATION_DESCRIPTOR_TYPE,			//bDescriptorType
		sizeof(struct configuration_descriptor),	//bTotalLength
		2,		//bNumInterfaces -- Is this value zero indexed?
		1,		//bConfigurationValue
		0,		//iConfiguration
		0xC0, 	//bmAttributes: This is for no remote wake up -- Probably look more into this
		0x1 	//MaxPower: Check how they arrived at this
};

struct interface_descriptor CustomInterface =
{
		sizeof(struct interface_descriptor),
		USB_INTERFACE_DESCRIPTOR_TYPE,
		0,
		0,
		1,
		USB_DEVICE_CLASS_COMMUNICATIONS, // Need to look up what this means specifically
		USB_SUBCLASS_CODE_UNKNOWN,
		USB_PROTOCOL_CODE_UNKNOWN,
		STR_INDEX_INTERFACE
};

struct interface_descriptor HIDKeyboardInterface =
{
		sizeof(struct interface_descriptor),
		USB_INTERFACE_DESCRIPTOR_TYPE,
		0,
		0,
		1,
		USB_DEVICE_CLASS_HUMAN_INTERFACE,
		USB_SUBCLASS_HID_BOOTINTERFACE,
		USB_HID_INTERFACE_PROTOCOL_KEYBOARD,
		STR_INDEX_INTERFACE
};

struct interface_descriptor HIDMouseInterface =
{
		sizeof(struct interface_descriptor),
		USB_INTERFACE_DESCRIPTOR_TYPE,
		0,
		0,
		1,
		USB_DEVICE_CLASS_HUMAN_INTERFACE,
		USB_SUBCLASS_HID_BOOTINTERFACE,
		USB_HID_INTERFACE_PROTOCOL_MOUSE,
		STR_INDEX_INTERFACE
};


USB_STRING_LANGUAGE_DESCRIPTOR  strLanguage =
{
  sizeof(USB_STRING_LANGUAGE_DESCRIPTOR),
  USB_STRING_DESCRIPTOR_TYPE,
  0x09,
  0x04
};


USB_STRING_INTERFACE_DESCRIPTOR  strInterface =
{
    sizeof(USB_STRING_INTERFACE_DESCRIPTOR),
    USB_STRING_DESCRIPTOR_TYPE,
    {'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,'0',0,'0',0}
};

USB_STRING_CONFIGURATION_DESCRIPTOR  strConfiguration =
{
    sizeof(USB_STRING_CONFIGURATION_DESCRIPTOR),
    USB_STRING_DESCRIPTOR_TYPE,
    {
    'C',0,
    'o',0,
    'n',0,
    'f',0,
    'i',0,
    'g',0,
    '0',0,
    '0',0
    }
};

USB_STRING_SERIALNUMBER_DESCRIPTOR  strSerialNum =
{
    sizeof(strSerialNum),
    USB_STRING_DESCRIPTOR_TYPE,
   {
    '0',0,
    '0',0,
    '0',0,
    '0',0,

    '0',0,
    '0',0,
    '0',0,
    '0',0,

    '0',0,
    '0',0,
    '0',0,
    '0',0
   }
};

USB_STRING_PRODUCT_DESCRIPTOR  strProduct =
{
    sizeof(USB_STRING_PRODUCT_DESCRIPTOR),
    USB_STRING_DESCRIPTOR_TYPE,
    {
    'P', 0,
    'H', 0,
    'I', 0,
    'L', 0,
    'I', 0,
    'P', 0,
    'S', 0,
    ' ', 0,
    'I', 0,
    'S', 0,
    'P', 0,
    '1', 0,
    '1', 0,
    '8', 0,
    '1', 0,
    ' ', 0,
    'T', 0,
    'e', 0,
    's', 0,
    't', 0,
    ' ', 0,
    ' ', 0,
    ' ', 0
    }
};

USB_STRING_MANUFACTURER_DESCRIPTOR  strManufacturer =
{
    sizeof(USB_STRING_MANUFACTURER_DESCRIPTOR),
    USB_STRING_DESCRIPTOR_TYPE,
    {
    'P', 0,
    'H', 0,
    'I', 0,
    'L', 0,
    'I', 0,
    'P', 0,
    'S', 0,
    ' ', 0,
    'S', 0,
    'e', 0,
    'm', 0,
    'i', 0,
    'c', 0,
    'o', 0,
    'n', 0,
    'd', 0,
    'u', 0,
    'c', 0,
    't', 0,
    'o', 0,
    'r', 0,
    's', 0,
    ' ', 0,
    'A', 0,
    'P', 0,
    'I', 0,
    'C', 0}
};

// *************************************************************************
// USB Protocol Layer
// ************************************************************************



// *************************************************************************
// USB standard device requests
// *************************************************************************

//unsigned char addr;
//addr = usb_GetAddress();
//printf("Address: %x\n", addr);

void chap9_SetAddress(void)
{

    unsigned char   j;
    {
        isr_Disable();
        if(!ControlData.Abort)
        {
            if(bUSBCheck_Device_State.State_bits.DEVICE_DEFAULT_STATE)
            {
                if((ControlData.DeviceRequest.wValue & DEVICE_ADDRESS_MASK) != 0 )
                {
                    bUSBCheck_Device_State.State_bits.DEVICE_DEFAULT_STATE = 0;
                    usb_SetAddressEnable((unsigned char)(ControlData.DeviceRequest.wValue &
                                            DEVICE_ADDRESS_MASK), 1);
                    bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 1;
                    chap9_SingleTransmitEP0(0, 0);

                }

                else
                    bUSBCheck_Device_State.State_bits.DEVICE_DEFAULT_STATE = 1;
            }

            else if(bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE)
            {
                if((ControlData.DeviceRequest.wValue & DEVICE_ADDRESS_MASK) == 0 )
                {
                    usb_SetAddressEnable((unsigned char)(ControlData.DeviceRequest.wValue &
                                            DEVICE_ADDRESS_MASK), 1);
                    bUSBCheck_Device_State.State_bits.DEVICE_DEFAULT_STATE = 1;
                    bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 0;
                    chap9_SingleTransmitEP0(0, 0);

                }

                else
                {
                    bUSBCheck_Device_State.State_bits.DEVICE_DEFAULT_STATE = 0;
                    usb_SetAddressEnable((unsigned char)(ControlData.DeviceRequest.wValue &
                                            DEVICE_ADDRESS_MASK), 1);
                    bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 1;
                    chap9_SingleTransmitEP0(0, 0);

                }

            }

            else
            {

                bUSBCheck_Device_State.State_bits.DEVICE_CONFIGURATION_STATE = 0;
                bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 0;
                bUSBCheck_Device_State.State_bits.DEVICE_DEFAULT_STATE = 1;
                j = DEV_BEHAVIOUR_NOT_SPECIFIED;
                chap9_SingleTransmitEP0(&j, 0);
            }

        }
        isr_Enable();

    }
    printf("Addr %x\n",usb_GetAddress());
}

void chap9_GetStatus(void)
{
    unsigned char   endp, txdat[2];
    unsigned char   c;
    unsigned char   bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;


    if(bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE)
    {
    switch(bRecipient)
            {
            case USB_RECIPIENT_DEVICE:
                    if(bD13flags.bits.remote_wakeup == 1)
                        txdat[0] = DEVSTS_SELFPOWERED|DEVSTS_REMOTEWAKEUP;   /* remote wakeup and Self-powered */
                    else
                        txdat[0] = DEVSTS_SELFPOWERED;   /* Self-powered*/
                    txdat[1]=0;

                    chap9_SingleTransmitEP0(txdat, 2);
                    break;

            case USB_RECIPIENT_INTERFACE:

                    chap9_StallEP0InControlRead();
                    break;

            case USB_RECIPIENT_ENDPOINT:

                    if( ControlData.DeviceRequest.wIndex == 0x00 )
                    {
                        endp = -1;
                        c = usb_GetEndpointStatusWOInteruptClear(endp + 1);
                        if(c & D13REG_EPSTS_STALL)
                            txdat[0] = ENDPSTS_HALT;   /* Halt */
                        else
                            txdat[0] = 0;

                        txdat[1] = 0;

                        chap9_SingleTransmitEP0(txdat, 2);
                    }
                    else
                        chap9_StallEP0InControlRead();

                    break;

            default:
                    chap9_StallEP0InControlRead();
                    break;
            }
    }


    else
    {
        if(ControlData.DeviceRequest.wValue == 0 && ControlData.DeviceRequest.wLength == 2 )
        {
            switch(bRecipient)
            {
            case USB_RECIPIENT_DEVICE:
                    if(bD13flags.bits.remote_wakeup == 1)
                        txdat[0] = DEVSTS_SELFPOWERED|DEVSTS_REMOTEWAKEUP;   /* remote wakeup and Self-powered */
                    else
                        txdat[0] = DEVSTS_SELFPOWERED;   /* Self-powered*/
                    txdat[1]=0;

                    chap9_SingleTransmitEP0(txdat, 2);
                    break;

            case USB_RECIPIENT_INTERFACE:
                    txdat[0]=0;
                    txdat[1]=0;

                    chap9_SingleTransmitEP0(txdat, 2);
                    break;

            case USB_RECIPIENT_ENDPOINT:

                    if( ControlData.DeviceRequest.wIndex == 0x00 )
                        endp = -1;
                    else
                        endp = (unsigned char)(ControlData.DeviceRequest.wIndex & MAX_ENDPOINTS);

                    c = usb_GetEndpointStatusWOInteruptClear(endp + 1);
                    if(c & D13REG_EPSTS_STALL)
                        txdat[0] = ENDPSTS_HALT;   /* Halt */
                    else
                        txdat[0] = 0;

                    txdat[1] = 0;

                    chap9_SingleTransmitEP0(txdat, 2);
                    break;

            default:
                    chap9_StallEP0InControlRead();
                    break;
            }
        }
    }

}

void chap9_ClearFeature(void)
{
    unsigned char   endp;
    unsigned char   bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;
    unsigned short  wFeature = ControlData.DeviceRequest.wValue;

    if(bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE)
    {
        if( ControlData.DeviceRequest.wLength == 0 )
        {
            switch(bRecipient)
            {
            case USB_RECIPIENT_DEVICE:
                        if(wFeature == USB_FEATURE_REMOTE_WAKEUP)
                        {
                        bD13flags.bits.remote_wakeup = 0;
                        chap9_SingleTransmitEP0(0, 0);
                        }
                        else
                            chap9_StallEP0InControlWrite();

                        break;



            case USB_RECIPIENT_ENDPOINT:

                        if(wFeature == USB_FEATURE_ENDPOINT_STALL)
                        {
                            if( ControlData.DeviceRequest.wIndex == 0x00 )
                            {
                                endp = 0;
                                usb_SetEndpointStatus(endp, 0);

                                chap9_SingleTransmitEP0(0, 0);
                            }
                            else
                            {
                                chap9_StallEP0InControlWrite();
                                break;
                            }

                        }

                        else
                            chap9_StallEP0InControlWrite();

                        break;


            default:
                        chap9_StallEP0InControlWrite();
                        break;
            }
      }
    }

    else
    {

        if( ControlData.DeviceRequest.wLength == 0 )
        {
            switch(bRecipient)
            {
            case USB_RECIPIENT_DEVICE:
                if(wFeature == USB_FEATURE_REMOTE_WAKEUP)
                {
                    bD13flags.bits.remote_wakeup = 0;
                    chap9_SingleTransmitEP0(0, 0);
                }
                else
                {
                    chap9_StallEP0InControlWrite();
                }

                break;

                case USB_RECIPIENT_ENDPOINT:

                if(wFeature == USB_FEATURE_ENDPOINT_STALL)
                {
                    if( ControlData.DeviceRequest.wIndex == 0x00 )
                            endp = -1;
                    else
                        endp = (unsigned char)(ControlData.DeviceRequest.wIndex & MAX_ENDPOINTS);

                    usb_SetEndpointStatus(endp+1, 0);
                    usb_SetEndpointStatus(endp+1, 0);

                    chap9_SingleTransmitEP0(0, 0);
                }
                else
                {
                    chap9_StallEP0InControlWrite();
                }
                break;

            default:
                    chap9_StallEP0InControlWrite();
                    break;
            }
      }
    }

}

void chap9_SetFeature(void)
{
	unsigned char   endp = 0;
	unsigned char   bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;
	unsigned short  wFeature = ControlData.DeviceRequest.wValue;
  
    if(bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE)
    {
        switch(bRecipient)
        {
        case USB_RECIPIENT_DEVICE:
            if(wFeature == USB_FEATURE_REMOTE_WAKEUP)
            {
                bD13flags.bits.remote_wakeup = 1;
                chap9_SingleTransmitEP0(0, 0);
            }
            else
            {
                chap9_StallEP0InControlWrite();
            }
            break;

        case USB_RECIPIENT_ENDPOINT:
            if( wFeature == USB_FEATURE_ENDPOINT_STALL)
            {
                if( ControlData.DeviceRequest.wIndex == 0x00 )
                    endp = -1;

                usb_SetEndpointStatus(endp+1, D13REG_EPSTS_STALL);
                chap9_SingleTransmitEP0(0, 0);

            }
            else
                chap9_StallEP0InControlWrite();

            break;
        default:
            chap9_StallEP0InControlWrite();
            break;
        }
    }


    else
    {
        if( ControlData.DeviceRequest.wLength == 0 )
        {
            switch(bRecipient)
            {
            case USB_RECIPIENT_DEVICE:
                if(wFeature == USB_FEATURE_REMOTE_WAKEUP)
                {
                    bD13flags.bits.remote_wakeup = 1;
                    chap9_SingleTransmitEP0(0, 0);
                }
                else
                {
                    chap9_StallEP0InControlWrite();
                }
                break;
            case USB_RECIPIENT_ENDPOINT:
                if( wFeature == USB_FEATURE_ENDPOINT_STALL)
                {
                    if( ControlData.DeviceRequest.wIndex == 0x00 )
                        endp = -1;
                    else
                        endp = (unsigned char)(ControlData.DeviceRequest.wIndex & MAX_ENDPOINTS);

                    usb_SetEndpointStatus(endp+1, D13REG_EPSTS_STALL);

                    chap9_SingleTransmitEP0(0, 0);

                }
                else
                {
                    usb_SetEndpointStatus(endp+1, 0);
                    chap9_SingleTransmitEP0(0, 0);
                }
                break;
            default:
                chap9_StallEP0InControlWrite();
                break;
            }
        }
    }



}

void chap9_GetDescriptor(void)
{
    unsigned char   bDescriptor = MSB(ControlData.DeviceRequest.wValue);
    unsigned char   bDescriptorIndex = LSB(ControlData.DeviceRequest.wValue);
    //printf("bDescri %d",bDescriptor);
    // printf("Index %d\n",bDescriptorIndex);
    //  printf("USB_DEVICE_DESCRIPTORsize %d\n",sizeof(USB_DEVICE_DESCRIPTOR));
    // printf("CONFIGURATIONsize %d\n",sizeof(USB_CONFIGURATION_DESCRIPTOR_a));
    switch(bDescriptor)
    {
    //sDevice sConfiguration
    //sizeof(struct CONFIGURATION)
    //sizeof( struct DEVICE)

    case USB_DEVICE_DESCRIPTOR_TYPE:{//1
        chap9_BurstTransmitEP0( (unsigned char *)&DeviceDesc , sizeof(struct device_descriptor));
        //      chap9_BurstTransmitEP0((unsigned char *)&sDevice,sizeof(struct DEVICE));
        break;}
    case USB_CONFIGURATION_DESCRIPTOR_TYPE:{//2
    	//      chap9_BurstTransmitEP0((unsigned char *)&ConfigDescr, CONFIG_DESCRIPTOR_LENGTH);

      
    	//printf("\nConfigDescr     \n");
    	chap9_BurstTransmitEP0((unsigned char *)&CompleteConfigDesc, sizeof(struct complete_configuration_descriptor));//sizeof(USB_CONFIGURATION_DESCRIPTOR));
    	//chap9_SetConfiguration();
        
    	//  config_endpoint();
    	//printf("\nInterfaceDescr0  ");    chap9_BurstTransmitEP0((unsigned char *)&InterfaceDescr0, sizeof(USB_INTERFACE_DESCRIPTOR));
    	//          printf("\nEP1_TXDesc       ");    chap9_BurstTransmitEP0((unsigned char *)&EP1_TXDescr, sizeof(USB_ENDPOINT_DESCRIPTOR));
    	//        printf("\nEP1_RXDescr      ");    chap9_BurstTransmitEP0((unsigned char *)&EP1_RXDescr, sizeof(USB_ENDPOINT_DESCRIPTOR));
    	//      printf("\nEP2_TXDescr      ");    chap9_BurstTransmitEP0((unsigned char *)&EP2_TXDescr, sizeof(USB_ENDPOINT_DESCRIPTOR));
    	//    printf("\nEP2_RXDescr      ");    chap9_BurstTransmitEP0((unsigned char *)&EP2_RXDescr, sizeof(USB_ENDPOINT_DESCRIPTOR));
        break;}
    case USB_STRING_DESCRIPTOR_TYPE://3
        switch(bDescriptorIndex)
        {
        case STR_INDEX_LANGUAGE:
            printf("\n str1     \n");
      chap9_BurstTransmitEP0((unsigned char *)&strLanguage, sizeof(USB_STRING_LANGUAGE_DESCRIPTOR));
            break;
        case STR_INDEX_MANUFACTURER:
            printf("\n str2     \n");chap9_BurstTransmitEP0((unsigned char *)&strManufacturer, sizeof(USB_STRING_MANUFACTURER_DESCRIPTOR));
            break;
        case STR_INDEX_PRODUCT:
            printf("\n str3     \n");chap9_BurstTransmitEP0((unsigned char *)&strProduct, sizeof(USB_STRING_PRODUCT_DESCRIPTOR));
            break;
        case STR_INDEX_SERIALNUMBER:
            printf("\n str4     \n");chap9_BurstTransmitEP0((unsigned char *)&strSerialNum, sizeof(USB_STRING_SERIALNUMBER_DESCRIPTOR));
            break;
        case STR_INDEX_CONFIGURATION:
            printf("\n str5     \n");chap9_BurstTransmitEP0((unsigned char *)&strConfiguration, sizeof(USB_STRING_CONFIGURATION_DESCRIPTOR) );
            break;
        case STR_INDEX_INTERFACE:
            printf("\n str6     \n");chap9_BurstTransmitEP0((unsigned char *)&strInterface, sizeof(USB_STRING_INTERFACE_DESCRIPTOR) );
            break;
        default:
            printf("\n str7     \n");chap9_StallEP0InControlRead();
            break;
        }
        break;
    case USB_INTERFACE_DESCRIPTOR_TYPE:
    case USB_ENDPOINT_DESCRIPTOR_TYPE:
    case USB_POWER_DESCRIPTOR_TYPE:
    default:
        chap9_StallEP0InControlRead();
        break;
    }
}

void chap9_GetConfiguration(void)
{
    unsigned char   c = bD13flags.bits.configuration;
    unsigned char   j;

    if(bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE)
    {
        j = 0;
        chap9_SingleTransmitEP0(&j, 1);
    }

    else
    {
        if (ControlData.DeviceRequest.wValue == 0 && ControlData.DeviceRequest.wIndex == 0 && ControlData.DeviceRequest.wLength == 1)
        {

        chap9_SingleTransmitEP0(&c, 1);
        }
    }

}

void chap9_SetConfiguration(void)
{
  
    if(bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE)
    {
        if(ControlData.DeviceRequest.wIndex == 0 && ControlData.DeviceRequest.wLength == 0)
        {
            if (ControlData.DeviceRequest.wValue == 0)
            {
                /* put device in unconfigured state */
                bD13flags.bits.configuration = 0;
                bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 1;
                chap9_SingleTransmitEP0(0, 0);
            }
            else if (ControlData.DeviceRequest.wValue == 1)
            {
                /* Configure device */
                bD13flags.bits.configuration = 1;
                bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 0;
                bUSBCheck_Device_State.State_bits.DEVICE_CONFIGURATION_STATE = 1;
                chap9_SingleTransmitEP0(0, 0);
            }

            else
                chap9_StallEP0InControlWrite();
        }
    }

    else
    {
        if (ControlData.DeviceRequest.wValue == 0)
        {
            /* put device in unconfigured state */
            bD13flags.bits.configuration = 0;
            bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 1;
            bUSBCheck_Device_State.State_bits.DEVICE_CONFIGURATION_STATE = 0;
            chap9_SingleTransmitEP0(0, 0);

        }

        else if (ControlData.DeviceRequest.wValue == 1)
        {
            /* Configure device */
            bD13flags.bits.configuration = 1;
            bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 0;
            bUSBCheck_Device_State.State_bits.DEVICE_CONFIGURATION_STATE = 1;
            chap9_SingleTransmitEP0(0, 0);
        }
        else
            chap9_StallEP0InControlWrite();
    }

}

void chap9_GetInterface(void)
{

    if(bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE)
        chap9_StallEP0InControlRead();


    else
    {

        if ((ControlData.DeviceRequest.wValue == 0 && ControlData.DeviceRequest.wIndex == 0 && ControlData.DeviceRequest.wLength == 1))
            chap9_SingleTransmitEP0(&ALTERNATIVE_SETTING, 1);      //txdat
        else
            chap9_StallEP0InControlRead();
    }

}

void chap9_SetInterface(void)
{
    unsigned char   dir = ControlData.DeviceRequest.bmRequestType & USB_REQUEST_DIR_MASK;

    if(dir)
        chap9_StallEP0InControlRead();


    if(bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE)
        chap9_StallEP0InControlRead();



    else
    {

        if (ControlData.DeviceRequest.wValue == 0 && ControlData.DeviceRequest.wIndex == 0)
        {
            ALTERNATIVE_SETTING = 0;
            chap9_SingleTransmitEP0(0,0);
        }

        else if (ControlData.DeviceRequest.wValue == 1 && ControlData.DeviceRequest.wIndex == 0)
        {
            ALTERNATIVE_SETTING = 1;
            chap9_SingleTransmitEP0(0,0);
        }
        else
        {
            chap9_StallEP0InControlWrite();
        }
    }

}

// *************************************************************************
// Chap9 support functions
// *************************************************************************

void chap9_StallEP0(void)
{
    unsigned char dir;
    dir = ControlData.DeviceRequest.bmRequestType & USB_REQUEST_DIR_MASK;
    if(dir)
    {
        usb_StallEP0InControlRead();
    }
    else
    {
        usb_StallEP0InControlWrite();
    }

    isr_Disable();
    if(!ControlData.Abort)
    {
        bD13flags.bits.DCP_state = USB_FSM_STALL;

    }
    isr_Enable();
}
void chap9_StallEP0InControlWrite(void)
{
    usb_StallEP0InControlWrite();

    isr_Disable();
    if(!ControlData.Abort)
    {
        bD13flags.bits.DCP_state = USB_FSM_STALL;

    }
    isr_Enable();
}

void chap9_StallEP0InControlRead(void)
{
    usb_StallEP0InControlRead();
    isr_Disable();
    if(!ControlData.Abort)
    {
        bD13flags.bits.DCP_state = USB_FSM_STALL;

    }
    isr_Enable();
}

void chap9_SingleTransmitEP0(unsigned char * buf, unsigned short len)
{

    usb_SingleTransmitEP0(buf, len);

    isr_Disable();
    if(!ControlData.Abort)
    {
        ControlData.wLength = ControlData.wCount = len;
        bD13flags.bits.DCP_state = USB_FSM_HANDSHAKE;

    }
    isr_Enable();
}

void chap9_BurstTransmitEP0(unsigned char * pData, unsigned short len)
{
    ControlData.wCount = 0;
    if(ControlData.wLength > len)
        ControlData.wLength = len;

    ControlData.Addr.pData = pData;

        if( ControlData.wLength >= NONISO_FIFO_PACKET_SIZE_64)
        {
            usb_WriteEndpoint(ENDP00_CONTROL_IN, pData, NONISO_FIFO_PACKET_SIZE_64);

            isr_Disable();
            if(!ControlData.Abort)
            {
                ControlData.wCount += NONISO_FIFO_PACKET_SIZE_64;
                bD13flags.bits.DCP_state = USB_FSM_DATAIN;

            }
            isr_Enable();
        }
        else
        {
            usb_WriteEndpoint(ENDP00_CONTROL_IN, pData, ControlData.wLength);

            isr_Disable();
            if(!ControlData.Abort)
            {
                ControlData.wCount += ControlData.wLength;
                bD13flags.bits.DCP_state = USB_FSM_HANDSHAKE;

            }
            isr_Enable();
        }
}

