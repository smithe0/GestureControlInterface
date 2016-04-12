#include <stdio.h>
#include "isr.h"
#include "sys/alt_irq.h"
#include "usb_commands.h"
#include "device_info.h"

// Global data
extern D13FLAGS bD13flags;
extern USBCHECK_DEVICE_STATES bUSBCheck_Device_State;
extern CONTROL_XFER ControlData;

//Function that determines source of interrupt and calls correct handler
void isr_usb(void * context, alt_u32 id) { // Either these arguments (void * context, alt_u32 id) or No arguments (void)
	unsigned long int_reg;

	printf("isr_main\n");

	isr_Disable();
	usb_UnlockDevice(0);
	int_reg = usb_ReadInterruptRegister();

	bD13flags.bits.At_IRQL1 = 1;

	if(int_reg != 0)
	{
		//printf("int_reg = 0x%lx\n", int_reg);
		if(int_reg & D13REG_INTSRC_BUSRESET)
			isr_BusReset();
		else if(int_reg & D13REG_INTSRC_SUSPEND)
			isr_Suspend();
		else if(int_reg & D13REG_INTSRC_EOT)
			isr_DmaEot();
		else if(int_reg & (D13REG_INTSRC_SOF|D13REG_INTSRC_PSEUDO_SOF))
			isr_SOF();
		else if(int_reg & D13REG_INTSRC_SHORT_PACKET)
			isr_SHORT_PACKET();
		else if(int_reg & D13REG_INTSRC_RESUME)
		{
			printf("RESUME\n");
			usb_UnlockDevice(0);
		}
		else
		{
			if(int_reg & D13REG_INTSRC_EP0OUT)
				isr_Ep00RxDone();
			if(int_reg & D13REG_INTSRC_EP0IN)
				isr_Ep00TxDone();
			if(int_reg & D13REG_INTSRC_EP01)
				isr_Ep01Done();
			if(int_reg & D13REG_INTSRC_EP02)
				isr_Ep02Done();
			if(int_reg & D13REG_INTSRC_EP03)
				isr_Ep03Done();

/*			if(int_reg & D13REG_INTSRC_EP04)
				Isr_Ep04Done();
			if(int_reg & D13REG_INTSRC_EP05)
				Isr_Ep05Done();
			if(int_reg & D13REG_INTSRC_EP06)
				Isr_Ep06Done();
*/
		}
	}
	isr_Enable();
}

void isr_BusReset(void)
{
    printf("BUS RESET\n");
    usb_DisconnectUSB();
    usb_ConnectUSB();
    bD13flags.bits.DCP_state = 0x00;
    bD13flags.bits.dma_disable=0x00;

    bUSBCheck_Device_State.State_bits.DEVICE_DEFAULT_STATE = 1;
    bUSBCheck_Device_State.State_bits.DEVICE_ADDRESS_STATE = 0;
    bUSBCheck_Device_State.State_bits.DEVICE_CONFIGURATION_STATE = 0;
}


static alt_irq_context status;
static unsigned char INT_Enabled = 1;

// Disables all IRQs
void isr_Disable(void)
{
  if(INT_Enabled)
  {
    status=alt_irq_disable_all();
    INT_Enabled=0;
  }
  //printf("INT_Enabled: %d\n", INT_Enabled);
}

// Enables all IRQs
void isr_Enable(void)
{
  if(!INT_Enabled)
  {
    alt_irq_enable_all(status);
    INT_Enabled=1;
  }
}

//Endpoint 0 IN interrupt
void isr_Ep00TxDone(void) {
	printf("Ep00 IN: DOES NOTHING\n");
	short i = ControlData.wLength - ControlData.wCount;
	unsigned char ep_last;
	usb_ReadInterruptRegister();
	ep_last = usb_GetEndpointStatusWInteruptClear(ENDP00_CONTROL_IN); /* Clear interrupt flag */

	switch (bD13flags.bits.DCP_state)
	{
		case USB_FSM_HANDSHAKE:
			break;
		case USB_FSM_DATAIN:
			if( i >= NONISO_FIFO_PACKET_SIZE_64)
	        {
	            usb_WriteEndpoint(ENDP00_CONTROL_IN,ControlData.Addr.pData + ControlData.wCount, NONISO_FIFO_PACKET_SIZE_64);
	            ControlData.wCount += NONISO_FIFO_PACKET_SIZE_64;
	            // State remains at USBFSM4DCP_DATAIN
	        }
	        else if( i != 0)
	        {
	            usb_WriteEndpoint(ENDP00_CONTROL_IN,ControlData.Addr.pData + ControlData.wCount, i);
	            ControlData.wCount += i;
	            bD13flags.bits.DCP_state = USB_FSM_HANDSHAKE;
	        }
	        else if (i == 0)
	        {
	            bD13flags.bits.DCP_state = USB_FSM_HANDSHAKE;
	            usb_SingleTransmitEP0(0, 0);
	        }
	        break;
		case USB_FSM_REQUESTPROC: break;
	    case USB_FSM_IDLE:       break;
	    case USB_FSM_SETUPPROC:  break;
	    case USB_FSM_STALL:      break;
	    case USB_FSM_DATAOUT:    break;
	    default:
	        printf("Fatal Error: Isr_Ep0TxDone Unexpected FSMState=%x\n",bD13flags.bits.DCP_state);
	        bD13flags.bits.DCP_state = USB_FSM_STALL;
	        usb_StallEP0InControlRead();
	        break;
	    }
}

//Endpoint 0 OUT interrupt
void isr_Ep00RxDone(void) {
	printf("\nEp00 OUT: \n");
	unsigned char ep_last, i;
	printf("RX %d\n", bUSBCheck_Device_State.State_bits.FOUR_RESERVED_BITS++);
	ep_last = usb_GetEndpointStatusWInteruptClear(ENDP00_CONTROL_OUT); /* Clear interrupt flag */
	printf("ep_last: 0x%x\n", ep_last);
	if (ep_last & D13REG_EPSTS_SETUP)
	{
		if(bD13flags.bits.DCP_state == USB_FSM_REQUESTPROC)
		{
			ControlData.Abort = 1;
			bD13flags.bits.DCP_state = USB_FSM_SETUPPROC;
		}
		else
			bD13flags.bits.DCP_state = USB_FSM_SETUPPROC;
	}

	else /* not a setup packet, just a Data Out Packet */
	{
		switch (bD13flags.bits.DCP_state)
		{
		case USB_FSM_DATAOUT:

			i = usb_ReadEndpoint(ENDP00_CONTROL_OUT, ControlData.dataBuffer + ControlData.wCount,
					NONISO_FIFO_PACKET_SIZE_64);

			ControlData.wCount += i;
			if( i != NONISO_FIFO_PACKET_SIZE_64 || ControlData.wCount >= ControlData.wLength)
			{
				bD13flags.bits.DCP_state = USB_FSM_REQUESTPROC;
			}
			break;
		case USB_FSM_HANDSHAKE:

			bD13flags.bits.DCP_state = USB_FSM_IDLE;
			break;

		case USB_FSM_STALL:      break;

		case USB_FSM_SETUPPROC:  break;

		case USB_FSM_REQUESTPROC:	break;

		case USB_FSM_DATAIN:     break;

		default:
			printf("Fatal Error: Isr_Ep0RxDone ReadLastTranactionStatus=%x\n",ep_last);
			bD13flags.bits.DCP_state = USB_FSM_STALL;
			usb_StallEP0InControlWrite();
			break;
		}
	}
	return ;
}

// EP01 Interrupt IN. Custom Interface.
void isr_Ep01Done(void) {
	printf("Ep01 Interrupt: DOES NOTHING YET\n");
	usb_GetEndpointStatusWInteruptClear(ENDP01); // Clear interrupt flag
}

// EP01 Interrupt IN. HID Keyboard.
void isr_Ep02Done(void) {
	printf("Ep02 Interrupt: DOES NOTHING YET\n");
	usb_GetEndpointStatusWInteruptClear(ENDP02); // Clear interrupt flag
}

// EP01 Interrupt IN. HID Mouse.
void isr_Ep03Done(void) {
	printf("Ep03 Interrupt: DOES NOTHING YET\n");
	usb_GetEndpointStatusWInteruptClear(ENDP03); // Clear interrupt flag
}

void isr_ShortPacket(void)
{
    printf("SHORT PACKET INTERRUPT: DOES NOTHING\n");
}

void isr_DmaEot(void)
{
    bD13flags.bits.dma_disable =0;

    if(bD13flags.bits.dma_state == DMA_PENDING)
        bD13flags.bits.setup_dma = 1;
    else
        bD13flags.bits.dma_state = DMA_IDLE;
    printf("EOT reached!!!\n");
    //if(bD13flags.bits.verbose==1)
    //    printf("dma_counter = %x\n", usb_GetDMACounter());
    printf("\n");
}

void isr_Suspend(void) {
    bD13flags.bits.suspend=1;
}

void isr_SOF(void) {
	// Does nothing
}

void isr_SHORT_PACKET(void) {
	// Does nothing
}
