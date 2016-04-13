#include "usb_irq.h"
#include "system.h"
#include "sys/alt_irq.h"

/*
 *	This file is only for enabling and disabling interrupts
 *
 *	With the exception of changing function names this file
 *	is unmodified code from the demonstration project.
 *
 */

alt_irq_context status;
unsigned char INT_Enabled=1;

void disable_irq(void)
{
  if(INT_Enabled)
  {
    status=alt_irq_disable_all();
    INT_Enabled=0;
  }
}

void enable_irq(void)
{
  if(!INT_Enabled)
  {
    alt_irq_enable_all(status);
    INT_Enabled=1;    
  }
}
