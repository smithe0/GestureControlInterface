#include <stdio.h>
#include "request_handler.h"
#include "usb_commands.h"
#include "device_info.h"
#include "chap_9.h"
#include "isr.h"

//-------------------------------------------------------------------------//
//  Global Variable
extern D13FLAGS bD13flags;
extern CONTROL_XFER ControlData;
//-------------------------------------------------------------------------//

//*************************************************************************
// USB protocol function pointer arrays
//*************************************************************************

#define MAX_STANDARD_REQUEST    0x0D
void (*StandardDeviceRequest[])(void) =
{
  chap9_GetStatus,
  chap9_ClearFeature,
  chap9_StallEP0,
  chap9_SetFeature,
  chap9_StallEP0,
  chap9_SetAddress,
  chap9_GetDescriptor,
  chap9_StallEP0,
  chap9_GetConfiguration,
  chap9_SetConfiguration,
  chap9_GetInterface,
  chap9_SetInterface,
  chap9_StallEP0
};


char * _NAME_USB_REQUEST_DIRECTION[] =
{
"Host_to_device",
"Device_to_host"
};

char * _NAME_USB_REQUEST_RECIPIENT[] =
{
"Device",
"Interface",
"Endpoint(0)",
"Other"
};

char * _NAME_USB_REQUEST_TYPE[] =
{
"Standard",
"Class",
"Vendor",
"Reserved"
};

char * _NAME_USB_STANDARD_REQUEST[] =
{
"GET_STATUS",
"CLEAR_FEATURE",
"RESERVED",
"SET_FEATURE",
"RESERVED",
"SET_ADDRESS",
"GET_DESCRIPTOR",
"SET_DESCRIPTOR",
"GET_CONFIGURATION",
"SET_CONFIGURATION",
"GET_INTERFACE",
"SET_INTERFACE",
"SYNC_FRAME"
};

//*************************************************************************
// Class device requests
//*************************************************************************

#define MAX_CLASS_REQUEST    0x06
void (*ClassDeviceRequest[])(void) =
{
    Reserved,
    Reserved,
    Reserved,
    Reserved,
    Reserved,
    Reserved
};

char * _NAME_USB_CLASS_REQUEST[] =
{
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved"
};

//*************************************************************************
// Vendor Device Request
//*************************************************************************

#define MAX_VENDOR_REQUEST    0x0f
void (*VendorDeviceRequest[])(void) =
{
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0,
	chap9_StallEP0
};

char * _NAME_USB_VENDOR_REQUEST[] =
{
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
"RESERVED",
};

void SetupTokenHandler(void)
{
  unsigned short j;
  isr_Disable();
  bD13flags.bits.At_IRQL1 = 1;
  ControlData.Abort = 0;
  ControlData.wLength = 0;
  ControlData.wCount = 0;

  j = usb_ReadEndpointWOClearBuffer(ENDP00_CONTROL_OUT, &ControlData.DeviceRequest, sizeof(ControlData.DeviceRequest) );

  printf("j:%d\n",j);
  printf("sizeof(DEVICE_REQUEST):%ld\n",sizeof(DEVICE_REQUEST));
  /*
  printf("1:%X 2:%X 34:%X 56:%X 78:%X\n",
  ControlData.DeviceRequest.bmRequestType,
  ControlData.DeviceRequest.bRequest,
  ControlData.DeviceRequest.wValue,
  ControlData.DeviceRequest.wIndex,
  ControlData.DeviceRequest.wLength);
  */
  if( j == sizeof(DEVICE_REQUEST) )


	  //  if( usb_ReadEndpointWOClearBuffer(EPINDEX4EP0_CONTROL_OUT, (UCHAR *)(&(ControlData.DeviceRequest)), sizeof(ControlData.DeviceRequest))
	  //   == sizeof(DEVICE_REQUEST) )
  {

  //printf("ControlData.DeviceRequest_size:%d\n",sizeof(ControlData.DeviceRequest));
    //printf("ControlData.DeviceRequest.bmRequestType:0x%X\n",ControlData.DeviceRequest.bmRequestType);   //1BYTE
    //printf("ControlData.DeviceRequest.bRequest:0x%X\n",ControlData.DeviceRequest.bRequest);//1BYTE
    //printf("ControlData.DeviceRequest.wValue:0x%X\n",ControlData.DeviceRequest.wValue); //2BYTE
    //printf("ControlData.DeviceRequest.wIndex:0x%X\n",ControlData.DeviceRequest.wIndex); //2BYTE
    //printf("ControlData.DeviceRequest.wLength:0x%X\n",ControlData.DeviceRequest.wLength);//2BYTE

    bD13flags.bits.At_IRQL1 = 0;
    isr_Enable();
    ControlData.wLength = ControlData.DeviceRequest.wLength;
    ControlData.wCount = 0;

    if (ControlData.DeviceRequest.bmRequestType & (unsigned char)USB_ENDPOINT_DIRECTION_MASK)
    {
      /* get command */
      isr_Disable();
      AcknowledgeSETUP();
      if((ControlData.DeviceRequest.bRequest == 0) & (ControlData.DeviceRequest.bmRequestType == 0xc0))
        bD13flags.bits.DCP_state = USB_FSM_HANDSHAKE;
      else
        bD13flags.bits.DCP_state = USB_FSM_REQUESTPROC;

      isr_Enable();
    }
    else
    {
      /* set command */

      if (ControlData.DeviceRequest.wLength == 0)
      {
        /* Set command  without Data stage*/
        isr_Disable();
        AcknowledgeSETUP();
        bD13flags.bits.DCP_state = USB_FSM_REQUESTPROC;
        isr_Enable();
      }
      else
      {
        /*
        // Set command  with Data stage
        // get Data Buffer
        */
        if(ControlData.DeviceRequest.wLength <= MAX_CONTROLDATA_SIZE)
        {
          /* set command with OUT token */
          isr_Disable();
          bD13flags.bits.DCP_state = USB_FSM_DATAOUT;
          isr_Enable();
          AcknowledgeSETUP();

        }
        else
        {
          isr_Disable();
          AcknowledgeSETUP();
          usb_StallEP0InControlWrite();
          bD13flags.bits.DCP_state = USB_FSM_STALL;
          printf("bD13flags.bits.DCP_state = x%hx\n Unknown set up command\n", bD13flags.bits.DCP_state);
          isr_Enable();
        }
      }
    }
  }
  else
  {
    printf("wrong setup command\n");
    bD13flags.bits.At_IRQL1 = 0;
    isr_Enable();
    chap9_StallEP0();
  }

 // printf("To_Ha_end\n");
}


void DeviceRequestHandler(void)
{
  unsigned char type, req;

  type = ControlData.DeviceRequest.bmRequestType & USB_REQUEST_TYPE_MASK;
  req =  ControlData.DeviceRequest.bRequest & USB_REQUEST_MASK;

  //if (bD13flags.bits.verbose==1)
  //printf("type = 0x%02x, req = 0x%02x\n", type, req);

   // help_devreq(type, req); /* print out device request */

  if ((type == USB_STANDARD_REQUEST) && (req < MAX_STANDARD_REQUEST))
  {
    (*StandardDeviceRequest[req])();
  }
  else if ((type == USB_CLASS_REQUEST) && (req < MAX_CLASS_REQUEST))
    (*ClassDeviceRequest[req])();
  else if ((type == USB_VENDOR_REQUEST) && (req   < MAX_VENDOR_REQUEST))
    (*VendorDeviceRequest[req])();
  else{
    chap9_StallEP0();
  }
}

void SuspendChange(void) {
	printf("SUSPEND CHANGE \n");
}

/* Checks to make sure packet is valid before sending an ACK back across the bus */
void AcknowledgeSETUP(void){
	if( usb_IsSetupPktInvalid() || ControlData.Abort)
	    {
	        return;
	    }

	    usb_AcknowledgeSETUP();
	    usb_ClearBuffer(ENDP00_CONTROL_OUT);
}

void Reserved(void) {
	usb_ClearBuffer(ENDP00_CONTROL_OUT);
}

