#ifndef   ISR_H
#define   ISR_H

#include "sys/alt_irq.h"

void isr_Disable(void);
void isr_Enable(void);

//Function that determines source of interrupt and calls correct handler
void isr_usb(void *, alt_u32);

int Get_Interrupt_number_HEX(int IRQ_NUMBER);
void Get_PIC_Mask(int* PIC_mask1 , int* PIC_mask2 ,int IRQ_NUMBER );

// Interrupt handlers
void isr_BusReset(void);
void isr_Suspend(void);
void isr_Resume(void);

void isr_DmaEot(void); 			//Don't know what this is for or if I need it yet. DMA, as a subject, is still a mystery to me
void isr_SOF(void);				//Not implemented
void isr_SHORT_PACKET(void); 	//Not implemented

// Endpoint interrupts handlers
void isr_Ep00TxDone(void);
void isr_Ep00RxDone(void);
void isr_Ep01Done(void);
void isr_Ep02Done(void);
void isr_Ep03Done(void);

// Endpoints disabled
//void isr_Ep04Done(void);
//void isr_Ep05Done(void);
//void isr_Ep06Done(void);
//void isr_Ep07Done(void);
//void isr_Ep08Done(void);
//void isr_Ep09Done(void);
//void isr_Ep0ADone(void);
//void isr_Ep0BDone(void);
//void isr_Ep0CDone(void);
//void isr_Ep0DDone(void);
//void isr_Ep0EDone(void);

#endif
