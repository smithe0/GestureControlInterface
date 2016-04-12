#ifndef ISP1362_INCLUDES_H_
#define ISP1362_INCLUDES_H_

#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <unistd.h>  // usleep
#include <io.h>
//#include "my_includes.h"
#include "alt_types.h"  // alt_u32
#include "altera_avalon_pio_regs.h" //IOWR_ALTERA_AVALON_PIO_DATA
//#include "altera_avalon_spi.h"  // spi
#include "sys/alt_irq.h"  // interrupt
#include "sys/alt_alarm.h" // time tick function (alt_nticks(), alt_ticks_per_second())
#include "sys/alt_timestamp.h" 
#include "sys/alt_stdio.h"
#include "system.h"

#include <stddef.h>
#include <fcntl.h>

#include "BasicTyp.h"
#include "HAL4D13.h"
#include "usb_irq.h"
#include "ISP1362_HAL.h"
#include "ISR.h"
#include "MAINLOOP.h"

#endif /*ISP1362_INCLUDES_H_*/
