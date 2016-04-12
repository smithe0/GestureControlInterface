#include <stdio.h>
#include <altera_up_avalon_usb_regs.h>
#include <sys/alt_irq.h>
#include "usb_commands.h"
#include "system.h"
#include "isr.h"
#include "device_info.h"
//#include "includes.h"

//-------------------------------------------------------------------------//
//  Global Variable
extern D13FLAGS bD13flags;
//-------------------------------------------------------------------------//

//USB_0_BASE below should be replaced with the base address of the USB controller component used in your project
#define USB_CORE_BASE USB_0_BASE

//****************
// Setup Commands
//****************

void usb_AcquireD13(unsigned char g_host_IRQ_num, void * usb_isr)
{
  alt_irq_register(g_host_IRQ_num,NULL,usb_isr);
}

void usb_UnlockDevice(unsigned char bTrue)
{
    unsigned short c;
    if(bTrue)
        c = 0;
    else
        c = 0xAA37; //Unlock code
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_LOCK );
    IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, c);
}

unsigned short usb_ReadChipID() {
	unsigned short ID;
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_RD_CHIPID);
	ID = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
	return ID;
}

void usb_DisconnectUSB(void) {
    usb_SetDevConfig( D13REG_DEVCNFG_NOLAZYCLOCK
                  |D13REG_DEVCNFG_PWROFF
                  |D13REG_DEVCNFG_CLOCKRUNNING );
    usb_SetMode( D13REG_MODE_INT_EN );
    //printf("disconnect\n");
}

void usb_ConnectUSB(void) {
    isr_Disable(); //Disable interrupts Critical Section
    //printf("connect_USB\n");
    bD13flags.value = 0; /* reset event flags */
    bD13flags.bits.DCP_state = USB_FSM_IDLE;
    usb_ConfigureEndpoints();
    isr_Enable(); //ReEnable interrupts

	usb_SetMode( D13REG_MODE_SOFTCONNECT
           | D13REG_MODE_DMA16
//           | D13REG_MODE_OFFGOODLNK
           | D13REG_MODE_DBG
           | D13REG_MODE_INT_EN
//         | D13REG_MODE_SUSPND
    	);
}

unsigned char usb_GetAddress(void)
{
    unsigned char c;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_RD_ADDR);
    c = (unsigned char)(IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE) & 0x0ff);
    return c;
}

void usb_SetAddressEnable(unsigned char bAddress, unsigned char bEnable)
{
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_WR_ADDR);
	if(bEnable)
		bAddress |= D13REG_DEVADDR_EN;
	else
		bAddress &= D13REG_DEVADDR_MASK;
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, bAddress);
}

void usb_SoftConnect(unsigned char bEnable)
{
	unsigned char c;
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_RD_MODE);
	c = (unsigned char)(IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE) & 0x0ff);
	if(bEnable)
		c |= D13REG_MODE_SOFTCONNECT;
	else
		c &= ~D13REG_MODE_SOFTCONNECT;
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_WR_MODE);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, c);
}

void usb_AcknowledgeSETUP(void)
{
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_ACK_SETUP);
}

void usb_ConfigureEndpoints(void) {
    // Control End-point
	usb_SetEndpointConfig( ENDPOINT_FIFO_ENABLE | ENDPOINT_DIR_OUT | NONISO_BUFSIZE_64, ENDP00_CONTROL_OUT);
    usb_SetEndpointConfig( ENDPOINT_FIFO_ENABLE | ENDPOINT_DIR_IN | NONISO_BUFSIZE_64, ENDP00_CONTROL_IN);

    // ENDP01 : Custom Endpoint. Does nothing for now.
    usb_SetEndpointConfig( /* ENDPOINT_FIFO_ENABLE |*/ ENDPOINT_DIR_IN | ENDPOINT_DBLBUF_EN | ENDPOINT_NOT_ISO | NONISO_BUFSIZE_8, ENDP01);

    // ENDP02 : HID keyboard IN endpoint. Not ISO (will be interrupt)
    usb_SetEndpointConfig( /* ENDPOINT_FIFO_ENABLE |*/ ENDPOINT_DIR_IN | ENDPOINT_DBLBUF_EN | ENDPOINT_NOT_ISO | NONISO_BUFSIZE_8, ENDP02);

    // ENDP03 : HID mouse IN endpoint. Not ISO (will be interrupt)
    usb_SetEndpointConfig( /* ENDPOINT_FIFO_ENABLE |*/ ENDPOINT_DIR_IN | ENDPOINT_DBLBUF_EN | ENDPOINT_NOT_ISO | NONISO_BUFSIZE_8, ENDP03);

    // End-point 4 - A *DISABLED*
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP04);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP05);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP06);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP07);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP08);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP09);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP0A);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP0B);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP0C);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP0D);
    usb_SetEndpointConfig(/* ENDPOINT_FIFO_ENABLE | */ ENDPOINT_DIR_OUT | ENDPOINT_DBLBUF_EN | NONISO_BUFSIZE_16, ENDP0E);

    /*Set interrupt configuration*/
    usb_SetIntEnable(
    		ENABLE_IRQ_EP0OUT
    		|ENABLE_IRQ_EP0IN
//    		|ENABLE_IRQ_EP01
//    		|ENABLE_IRQ_EP02
//    		|ENABLE_IRQ_EP03
//			|ENABLE_IRQ_EP04
//			|ENABLE_IRQ_EP05
//			|ENABLE_IRQ_EP06
    		|ENABLE_IRQ_SUSPEND
    		|ENABLE_IRQ_RESUME
    		|ENABLE_IRQ_BUSRESET
    );

    /*Set Hardware Configuration*/
    //These settings are actually all the default upon reset and so this call might be unnecessary
/*    usb_SetDevConfig(D13REG_DEVCNFG_NOLAZYCLOCK
    		| D13REG_DEVCNFG_CLOCKDIV_120M
    		| D13REG_DEVCNFG_DMARQPOL
//          | D13REG_DEVCNFG_EXPULLUP
//    		| D13REG_DEVCNFG_INTPOL
            | D13REG_DEVCNFG_INTEDGE
    );*/


}

unsigned char usb_GetEndpointConfig(unsigned char ENPT_INDEX)
{
	unsigned char c;
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_RD_CNFG + ENPT_INDEX);
	c = (unsigned char)( IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE) & 0x0ff);
	return c;
}

void usb_SetEndpointConfig(unsigned char bEPConfig, unsigned char bEPIndex) {
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_WR_CNFG + bEPIndex );
    IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, bEPConfig);
}

void usb_SetIntEnable(unsigned long enable_bits){
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_WR_INTEN);
	enable_bits = (enable_bits & 0x0FFFFFF);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE,(unsigned short)enable_bits);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE,(unsigned short)(enable_bits >> 16));
}

unsigned long usb_GetIntEnable() {
	unsigned long i;
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_RD_INTEN);
	i = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
	i += ( ((unsigned long) IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE)) << 16 );
	return i;
}

void usb_ResetDevice(void) {
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_RESET);
}

unsigned short usb_GetDevConfig(void) {
	unsigned short value;
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_RD_CNFG);
	value = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
	return value;
}

void usb_SetDevConfig(unsigned short wDevCnfg) {
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_WR_CNFG);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, wDevCnfg);
}

unsigned char usb_GetMode(){
	unsigned char mode;
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_RD_MODE);
	mode = (unsigned char)( IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE) & 0x0ff);
    return mode;
}

void usb_SetMode(unsigned char mode) {
	//printf("set mode at: %x\n", mode);

	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_WR_MODE);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, mode);
}

void usb_ModeDisableInterrupts() {
	unsigned char mode = usb_GetMode();
	mode = mode & ~(D13REG_MODE_INT_EN);

	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_WR_MODE);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, mode);
}

void usb_ModeEnableInterrupts() {
	unsigned char mode = usb_GetMode();
	mode = mode | D13REG_MODE_INT_EN;

	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_WR_MODE);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, mode);
}

//******************
// Communication Operations
//******************

// This reads both 16-bit words but only returns the least significant. (contains interrupt info up to ENDP06)
unsigned long usb_ReadInterruptRegister(void)
{
    unsigned long j,i = 0;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_INT_SRC);
    i = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    j = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    j = ((j<<16) & 0xffff0000 ) + (i & 0xffff);
    return i;
}

void usb_ClearBuffer(unsigned char ENDPIndex)
{
	/* Select Endpoint */
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_CLEAR_BUF + ENDPIndex);
}

void usb_ValidBuffer(unsigned char ENDPIndex)
{
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_VALID_BUF + ENDPIndex);
}

unsigned short usb_ReadEndpoint(unsigned char ENDPIndex, unsigned char *buf, unsigned short len)
{
    unsigned short i, j, c;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_RD_FIFO + ENDPIndex);
    /* read Buffer */
    j = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    if(j > len)
        j = len;
    i=0;
    while (i<j)//<<
    //for(i=0; i<j; i=i+2, buf++ )
    {
        c = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
        *buf = (unsigned char)c;
        i++;//<<
        if (i >= j) break; //<<
        buf++;
        *buf = (unsigned char)(c>>8);
        i++;//<<
         buf++;
    }
    /* Clear Buffer */
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_CLEAR_BUF+ENDPIndex);
    return j;

}

unsigned short usb_ReadBulkEndpoint(unsigned char ENDPIndex, unsigned char * buf, unsigned short len)
{
    unsigned short i, j, c;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_RD_FIFO + ENDPIndex);
    j = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    if(j > len)
        j = len;
    i=0;
    while (i<j) //<<
//  for(i=0; i<j; i=i+2, buf++ )
    {
        c = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
        *buf = (unsigned char)c;
        i++;//<<
        if (i >= j) break; //<<
        buf++;
        *buf = (unsigned char)(c>>8);

        i++;//<<
    buf++;
    }
    /* Clear Buffer */
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_CLEAR_BUF+ENDPIndex);

    return j;
}

// I think that the strange incrementing within the while loop is trying to handle a char buffer with a short len. Though I'm not convinced they are different.
unsigned short usb_ReadEndpointWOClearBuffer(unsigned char ENDPIndex, unsigned char * buf, unsigned short len)
{
    unsigned short i, j,c;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_RD_FIFO + ENDPIndex);
    /* read Buffer */
    j = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    if(j > len)
        j = len;
    i=0;
    while (i<j) //<<
   //for(i<j; i=i+2, buf++ )
    {
         c = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
         *buf = (unsigned char)c;//printf("WOC= %02X ",*buf);//<<
         i++;//<<
         if (i >= j) break; //<<
         buf++;
         *buf = (unsigned char)(c>>8); //printf("WOC= %02X ",*buf);//<<
         i++;//<<
     buf++;
    }
   // printf("\n",c);
    /* Clear Buffer */
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_CLEAR_BUF+ENDPIndex);

    return j;
}

// Be careful with this one, I'm not sure I entirely get what's going on in the while loop.
unsigned short usb_WriteEndpoint(unsigned char ENDPIndex, unsigned char * buf, unsigned short len)
{
    unsigned short i,c;

    /* Select endpoint and send write command */
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_WR_FIFO + ENDPIndex);
    IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, len);

    if (len==0) IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, 0);
    /* Write Buffer */

    i=0;
    while (i < len)
    {
    	c = *buf;
        buf++;i++;
        if (i >=len)
        	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, c);

        c =(c & 0xff)+ (*buf<<8  & 0xff00);
        buf++;i++;
        IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, c);
    }

  /* Validate Buffer */
  IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_VALID_BUF+ENDPIndex);
  return i;
}

unsigned short usb_WriteBulkEndpoint(unsigned char ENDPIndex, unsigned char * buf, unsigned short len)
{
    unsigned short i, c;

    //Write command (Tells endpoint you will write 'len' bytes)
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_WR_FIFO + ENDPIndex);
    IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, len);

    if (len==0)
    	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, 0);

    /* Write Buffer */
    for(i=0; i<len; i=i+2, buf++ )
    {
        c = *buf;
        buf++;
        c += (unsigned short)(*buf << 8);
        IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, c);
    }
    /* Validate Buffer */
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_VALID_BUF+ENDPIndex);
    return len;
}

unsigned char usb_GetEndpointStatusWInteruptClear(unsigned char bEPIndex)
{
    unsigned char c;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_RDSTS_CLRINT + bEPIndex);
    c = (unsigned char)(IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE) & 0x0ff);
    return c;
}

unsigned char usb_GetEndpointStatusWOInteruptClear(unsigned char bEPIndex)
{
    unsigned char c;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_RDSTS + bEPIndex);
    c = (unsigned char)(IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE) &0x0ff);
    return c;
}

void usb_SetEndpointStatus(unsigned char bEPIndex, unsigned char bStalled)
{
    if(bStalled & 0x80)
    {
    	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_WR_STS + bEPIndex);
    }
    else
    {
    // clear endpoint stall.
    	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_CLR_STALL + bEPIndex);
    }
}

void usb_SetDMAConfig(unsigned short wDMACnfg)
{
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DMA_WR_CNFG);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, wDMACnfg);
}

unsigned short usb_GetDMAConfig(void)
{
    unsigned short i;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DMA_RD_CNFG);
    i = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    return i;
}

void usb_SetDMACounter(unsigned short wDMACounter)
{
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DMA_WR_COUNT);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, wDMACounter);
}

unsigned short usb_GetDMACounter(void)
{
    unsigned short i;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DMA_RD_COUNT);
    i = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    return i;
}

unsigned short usb_GetDataFromChipRam(void)
{
    unsigned short i;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE,D13CMD_DEV_RD_RAM);
    i = IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    return i;
}

void usb_SetDataToChipRam(unsigned short wData)
{
	IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_DEV_WR_RAM);
	IOWR_ALT_UP_USB_DC_DATA(USB_CORE_BASE, wData);
}

unsigned short usb_ReadCurrentFrameNumber(void)
{
    unsigned short i;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_RD_FRMNUM);
    i= IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE);
    return i;
}

unsigned char usb_GetErrorCode(unsigned char bEPIndex)
{
    unsigned char   c;
    IOWR_ALT_UP_USB_DC_COMMAND(USB_CORE_BASE, D13CMD_EP_RD_ERR+bEPIndex);
    c = (unsigned char)(IORD_ALT_UP_USB_DC_DATA(USB_CORE_BASE)&0x0ff);
    return c;
}

void usb_SingleTransmitEP0(unsigned char * buf, unsigned short len)
{
    if( len <= NONISO_FIFO_PACKET_SIZE_64) {
        usb_WriteEndpoint(ENDP00_CONTROL_IN, buf, len);
    }
}

void usb_RegAccess(void)
{
    unsigned short   c;
    unsigned short  i;
    unsigned long   l;

    usb_ResetDevice();
    printf("Reseting\n");

    i = usb_ReadChipID();
    printf(" ChipId = %hx\n",i);

    i = usb_GetDataFromChipRam();
    printf(" Chip RAM = %hx\n",i);

    printf("Writing 0x55AA,");
    usb_SetDataToChipRam(0x55AA);
    i = usb_GetDataFromChipRam();
    printf(" Chip RAM = %hx\n",i);

    l = usb_GetIntEnable();
    printf("usb_GetIntEnable = %lx\n",l);

    l = usb_ReadInterruptRegister();
    printf("usb_ReadInterruptRegister = %lx\n",l);

    i = usb_GetDevConfig();
    printf("usb_GetDevConfig = %hx\n",i);

    i = usb_GetDMAConfig();
    printf("usb_GetDMAConfig = %hx\n",i);

    i = usb_GetDMACounter();
    printf("usb_GetDMACounter = %hx\n",i);

    i = usb_ReadCurrentFrameNumber();
    printf("usb_ReadCurrentFrameNumber = %hx\n",i);

    c = usb_GetMode();
    printf("usb_GetMode = %x\n",c);

    c = usb_GetEndpointStatusWOInteruptClear(ENDP00_CONTROL_OUT);
    printf("usb_GetEndpointStatus EP0 = %x\n",c);

 //   c = usb_GetAddress();
//  printf("usb_GetAddress = %x\n",c);
}

void usb_StallEP0InControlWrite(void)
{
    usb_SetEndpointStatus(ENDP00_CONTROL_IN, D13REG_EPSTS_STALL);
    usb_SetEndpointStatus(ENDP00_CONTROL_OUT, D13REG_EPSTS_STALL);
}

void usb_StallEP0InControlRead(void)
{
    usb_SetEndpointStatus(ENDP00_CONTROL_OUT, D13REG_EPSTS_STALL);
    usb_SetEndpointStatus(ENDP00_CONTROL_IN, D13REG_EPSTS_STALL);
}

unsigned char usb_IsSetupPktInvalid(void)
{
    unsigned short SetupPacketInvalid;
    SetupPacketInvalid = usb_GetEndpointStatusWOInteruptClear(ENDP00_CONTROL_OUT)&D13REG_EPSTS_OVWR;
    return SetupPacketInvalid;
}


