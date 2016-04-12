/*************************************************************************
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
**************************************************************************
* Description:                                                           *
* The following is a simple hello world program running MicroC/OS-II.The * 
* purpose of the design is to be a very simple application that just     *
* demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
* for issues such as checking system call return codes. etc.             *
*                                                                        *
* Requirements:                                                          *
*   -Supported Example Hardware Platforms                                *
*     Standard                                                           *
*     Full Featured                                                      *
*     Low Cost                                                           *
*   -Supported Development Boards                                        *
*     Nios II Development Board, Stratix II Edition                      *
*     Nios Development Board, Stratix Professional Edition               *
*     Nios Development Board, Stratix Edition                            *
*     Nios Development Board, Cyclone Edition                            *
*   -System Library Settings                                             *
*     RTOS Type - MicroC/OS-II                                           *
*     Periodic System Timer                                              *
*   -Know Issues                                                         *
*     If this design is run on the ISS, terminal output will take several*
*     minutes per iteration.                                             *
**************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <altera_up_avalon_usb.h>
#include <altera_up_avalon_usb_ptd.h>
#include <altera_up_avalon_usb_regs.h>
#include <altera_up_avalon_usb_high_level_driver.h>
#include <altera_up_avalon_usb_low_level_driver.h>

#include "includes.h"
#include "system.h"
#include "device_info.h"
#include "usb_commands.h"
#include "isr.h"
#include "request_handler.h"

//-------------------------------------------------------------------------//
//  Global Variable
D13FLAGS bD13flags;
USBCHECK_DEVICE_STATES bUSBCheck_Device_State;
CONTROL_XFER ControlData;
IO_REQUEST ioRequest;
//-------------------------------------------------------------------------//

/* Tester variables that will be used for debugging in many places */
unsigned short us_check;
unsigned char ch_check;

enum StateMachine {
	Setup,
	Request,
	Suspended
} state;

struct USBDevice {
	enum StateMachine state2;
	unsigned char successful_setup;

} myDevice;

void decoupleHcAndDcINT(void);

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

/* Maintains USB communication with host computer */
void task1(void* pdata)
{
	if(usb_GetAddress() == 0){
		printf("\n reset successful\n");
	}
	printf("TASK1\n");
	info_CheckChipID(); // Just checks which version of controller chip this device is. Which is 1362, in case you were curious.

	while (1)
	{

		usleep(1);
	}
	//OSTimeDlyHMSM(0, 0, 1, 0);
}

/* Task 2, other tasks */
void task2(void* pdata)
{
	while (1)
	{
		OSTimeDlyHMSM(0, 0, 3, 0);
	}
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
	state = Setup;

	/* Start by resetting device so its in a known state */
	usb_ResetDevice();
	usleep(1000000); //1 second sleep.
	isr_Disable();
	usb_UnlockDevice(0);
	decoupleHcAndDcINT(); // This may well not be required, but the register values by default didn't seem to be what I wanted them to be.
	usb_ConfigureEndpoints();

	/* Set Device Hardware Configuration */
    usb_SetDevConfig(D13REG_DEVCNFG_NOLAZYCLOCK
    		| D13REG_DEVCNFG_CLOCKDIV_120M
    		| D13REG_DEVCNFG_DMARQPOL
//          | D13REG_DEVCNFG_EXPULLUP
    		| D13REG_DEVCNFG_INTEDGE // Pulse triggered int2 (rather than level) can cause some missed interrupts
//    		| D13REG_DEVCNFG_INTPOL	 // Positive logic for int2
    );

    usb_SetMode( D13REG_MODE_SOFTCONNECT
               | D13REG_MODE_DMA16
//	           | D13REG_MODE_OFFGOODLNK
               | D13REG_MODE_DBG
               | D13REG_MODE_INT_EN 	//Enable interrupts
//		       | D13REG_MODE_SUSPND
        	);
    isr_Enable();

    OSTaskCreateExt(task1,
    			NULL,
    			(void *)&task1_stk[TASK_STACKSIZE-1],
    			TASK1_PRIORITY,
    			TASK1_PRIORITY,
    			task1_stk,
    			TASK_STACKSIZE,
    			NULL,
    			0);

    alt_irq_register(USB_0_IRQ, NULL, (void*)isr_usb);


              
/*
	OSTaskCreateExt(task2,
			NULL,
			(void *)&task2_stk[TASK_STACKSIZE-1],
			TASK2_PRIORITY,
			TASK2_PRIORITY,
			task2_stk,
			TASK_STACKSIZE,
			NULL,
			0);
*/
	OSStart();
	return 0;
}

void decoupleHcAndDcINT() {
	unsigned char mode;
	IOWR_ALT_UP_USB_HC_COMMAND(USB_0_BASE, 0x20);
	mode = (unsigned char)( IORD_ALT_UP_USB_HC_DATA(USB_0_BASE));
	mode = mode & ~(0x20);

	IOWR_ALT_UP_USB_HC_COMMAND(USB_0_BASE, 0xA0);
	IOWR_ALT_UP_USB_HC_DATA(USB_0_BASE, mode);
}


#if 0 // This is taken out because it is an old control loop that I am re-doing
//printf("While\n");
		if (bUSBCheck_Device_State.State_bits.RESET_BITS == 1)
		{
			printf("Disable\n");
			isr_Disable();
			break;
		}
		if (bD13flags.bits.suspend)
		{
			printf("Setup Token\n");
			isr_Disable();
			bD13flags.bits.suspend= 0;
			isr_Enable();
			SuspendChange();
		} // Suspend Change Handler
		if (bD13flags.bits.DCP_state == USB_FSM_SETUPPROC)
		{
			printf("Setup Token\n");
			isr_Disable();
			SetupTokenHandler();
			isr_Enable();
		} // Setup Token Handler
		if ((bD13flags.bits.DCP_state == USB_FSM_REQUESTPROC) && !ControlData.Abort)
		{
			printf("Device Request\n");
			isr_Disable();
			bD13flags.bits.DCP_state = 0x00;
			DeviceRequestHandler();
			isr_Enable();
		} // Device Request Handler
		usleep(1);
		//OSTimeDlyHMSM(0, 0, 0, 1);
#endif

#if 0
		if(state == Setup) {
			//unsigned long int_reg;
			//int_reg = usb_ReadInterruptRegister();
		//	SetupTokenHandler();

			unsigned short cntrl;
			//printf("Setup case\n");
			isr_Disable();
			usb_UnlockDevice(0);

			cntrl = usb_ReadEndpointWOClearBuffer(ENDP00_CONTROL_OUT, (unsigned char *)&ControlData.DeviceRequest, sizeof(ControlData.DeviceRequest));


			//printf("Size of setup packet: %lu\n", sizeof(DEVICE_REQUEST));
			//printf("Size received: %u\n", cntrl);

			if(cntrl == sizeof(DEVICE_REQUEST)) {
				printf("RequestType: 0x%x\n", ControlData.DeviceRequest.bmRequestType);
				printf("Request: 0x%x\n", ControlData.DeviceRequest.bRequest);
				printf("Value: 0x%x\n", ControlData.DeviceRequest.wValue);
				printf("Index: 0x%x\n", ControlData.DeviceRequest.wIndex);
				printf("Length: %u\n", ControlData.DeviceRequest.wLength);

				if (ControlData.DeviceRequest.bmRequestType & (unsigned char)USB_ENDPOINT_DIRECTION_MASK) {
					/* If the most significant byte is non-zero this Setup Token is a 'Get Request' */
					AcknowledgeSETUP(); // Send ACK for setup packet (iff it is a setup packet). Clear buffer.
					printf("ACK\n");
				}
			}
			cntrl = usb_ReadEndpointWOClearBuffer(ENDP00_CONTROL_OUT, (unsigned char *)&ControlData.DeviceRequest, sizeof(ControlData.DeviceRequest));
			if(cntrl == sizeof(DEVICE_REQUEST)) {
				printf("RequestType: 0x%x\n", ControlData.DeviceRequest.bmRequestType);
				printf("Request: 0x%x\n", ControlData.DeviceRequest.bRequest);
				printf("Value: 0x%x\n", ControlData.DeviceRequest.wValue);
				printf("Index: 0x%x\n", ControlData.DeviceRequest.wIndex);
				printf("Length: %u\n", ControlData.DeviceRequest.wLength);

					if (ControlData.DeviceRequest.bmRequestType & (unsigned char)USB_ENDPOINT_DIRECTION_MASK) {
						/* If the most significant byte is non-zero this Setup Token is a 'Get Request' */
						AcknowledgeSETUP(); // Send ACK for setup packet (iff it is a setup packet). Clear buffer.
						printf("ACK\n");
					}
			}
			else if (flag == 0) {
				//printf("Cntrl Reg: %lx\n", int_reg);
				flag = 1;
			}
			else {

			}
			isr_Enable();

		}

		else if (state == Request) {

		}
		else if (state == Suspended) {

		}
#endif
/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/
