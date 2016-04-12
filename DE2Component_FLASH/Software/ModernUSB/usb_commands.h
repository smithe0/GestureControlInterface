#ifndef   USB_COMMANDS_H
#define   USB_COMMANDS_H

#define DMA_IDLE    0
#define DMA_RUNNING 1
#define DMA_PENDING 2
#define DMA_IN_DONE 3
#define DMA_BUFFER_SIZE     65536

//Packet sizes
#define NONISO_FIFO_PACKET_SIZE_0   0
#define NONISO_FIFO_PACKET_SIZE_2   2
#define NONISO_FIFO_PACKET_SIZE_4   4
#define NONISO_FIFO_PACKET_SIZE_8   8
#define NONISO_FIFO_PACKET_SIZE_16  16
#define NONISO_FIFO_PACKET_SIZE_32  32
#define NONISO_FIFO_PACKET_SIZE_64  64

//  Endpoint Index
#define ENDP00_CONTROL_OUT     0x0
#define ENDP00_CONTROL_IN      0x1
#define ENDP01             	  0x2
#define ENDP02                0x3
#define ENDP03                0x4
#define ENDP04                0x5
#define ENDP05                0x6
#define ENDP06                0x7
#define ENDP07                0x8
#define ENDP08                0x9
#define ENDP09                0xA
#define ENDP0A                0xB
#define ENDP0B                0xC
#define ENDP0C                0xD
#define ENDP0D                0xE
#define ENDP0E                0xF

//  Port Offset for ISP1362
#define HC_DATA_PORT      0
#define HC_COMMAND_PORT   1
#define D13_DATA_PORT     2 //Device Controller Port
#define D13_COMMAND_PORT  3 //Device Controller Port

// Command Set
// These are commands that are sent to the command register
#define D13CMD_DEV_RESET            0xF6
#define D13CMD_DEV_RD_CHIPID        0xB5
#define D13CMD_DEV_WR_ADDR          0xB6
#define D13CMD_DEV_RD_ADDR          0xB7
#define D13CMD_DEV_WR_MODE          0xB8
#define D13CMD_DEV_RD_MODE          0xB9
#define D13CMD_DEV_WR_CNFG          0xBA
#define D13CMD_DEV_RD_CNFG          0xBB
#define D13CMD_DEV_INT_SRC          0xC0
#define D13CMD_DEV_WR_INTEN         0xC2
#define D13CMD_DEV_RD_INTEN         0xC3
#define D13CMD_DEV_WR_RAM           0xB2
#define D13CMD_DEV_RD_RAM           0xB3
#define D13CMD_DEV_LOCK             0xB0

#define D13CMD_DMA_RD_COUNT         0xF3
#define D13CMD_DMA_WR_COUNT         0xF2
#define D13CMD_DMA_RD_CNFG          0xF1
#define D13CMD_DMA_WR_CNFG          0xF0

#define D13CMD_ACK_SETUP            0xF4
#define D13CMD_RD_FRMNUM            0xB4

// BMP for DevADDR
#define D13REG_DEVADDR_MASK             0x7F
#define D13REG_DEVADDR_EN               0x80

// Endpoint configuration flags
#define ENDPOINT_FIFO_ENABLE            0x80 // Enables an end-point buffer memory (disabled will not be allocated memory)
#define ENDPOINT_DIR_IN		            0x40
#define ENDPOINT_DIR_OUT	            0x00
#define ENDPOINT_DBLBUF_EN         		0x20
#define ENDPOINT_IS_ISO    	        	0x10 // Is either ISO (If set) or Bulk / Interrupt (If not set)
#define ENDPOINT_NOT_ISO				0x00

#define ENDPOINT_ISO_SIZE_MASK        	0x0F
#define ENDPOINT_NONISO_SIZE_MASK       0x07
#define ENDPOINT_RFB_EN            		0x08 //Don't know what this one is for

/*Bulk Endpoint Sizes*/
#define NONISO_BUFSIZE_8        0x00
#define NONISO_BUFSIZE_16       0x01
#define NONISO_BUFSIZE_32       0x02
#define NONISO_BUFSIZE_64       0x03

/*Isochronous Endpoint Sizes*/
#define ISO_BUFSIZE_16          0x00
#define ISO_BUFSIZE_32          0x01
#define ISO_BUFSIZE_48          0x02
#define ISO_BUFSIZE_64          0x03
#define ISO_BUFSIZE_96          0x04
#define ISO_BUFSIZE_128         0x05
#define ISO_BUFSIZE_160         0x06
#define ISO_BUFSIZE_192         0x07
#define ISO_BUFSIZE_256         0x08
#define ISO_BUFSIZE_320         0x09
#define ISO_BUFSIZE_384         0x0A
#define ISO_BUFSIZE_512         0x0B
#define ISO_BUFSIZE_640         0x0C
#define ISO_BUFSIZE_768         0x0D
#define ISO_BUFSIZE_896         0x0E
#define ISO_BUFSIZE_1023        0x0F

// Add ENDP index to access correct Endpoint
#define D13CMD_EP_WR_FIFO           0x00
#define D13CMD_EP_RD_FIFO           0x10
#define D13CMD_EP_WR_CNFG           0x20
#define D13CMD_EP_RD_CNFG           0x30
#define D13CMD_EP_WR_STS            0x40
#define D13CMD_EP_CLR_STALL         0x80
#define D13CMD_EP_RDSTS_CLRINT      0x50
#define D13CMD_EP_VALID_BUF         0x60
#define D13CMD_EP_CLEAR_BUF         0x70
#define D13CMD_EP_RDSTS             0xD0
#define D13CMD_EP_RD_ERR            0xA0

// BMP for EP_STATUS
#define D13REG_EPSTS_STALL              0x80
#define D13REG_EPSTS_DBF1               0x40
#define D13REG_EPSTS_DBF0               0x20
#define D13REG_EPSTS_OVWR               0x08
#define D13REG_EPSTS_SETUP              0x04
#define D13REG_EPSTS_DBFIDX             0x02

// BMP for Error_Code
#define D13REG_ERRCODE_RXTX             0x01
#define D13REG_ERRCODE_DPID             0x40
#define D13REG_ERRCODE_OVRLP            0x80
#define D13REG_ERRCODE_MASK             0x1E
#define D13REG_ERRCODE_OK               0x00
#define D13REG_ERRCODE_PIDERR           0x02
#define D13REG_ERRCODE_PIDNEW           0x04
#define D13REG_ERRCODE_PKTTYP           0x06
#define D13REG_ERRCODE_TKCRC            0x08
#define D13REG_ERRCODE_DATACRC          0x0A
#define D13REG_ERRCODE_TIMEOUT          0x0C
#define D13REG_ERRCODE_RSRV0            0x0E
#define D13REG_ERRCODE_EOP              0x10
#define D13REG_ERRCODE_NAK              0x12
#define D13REG_ERRCODE_STALL            0x14
#define D13REG_ERRCODE_OVRFL            0x16
#define D13REG_ERRCODE_RSRV1            0x18
#define D13REG_ERRCODE_BITSTUFF         0x1A
#define D13REG_ERRCODE_RSRV2            0x1C
#define D13REG_ERRCODE_DATAPID          0x1E

// FSM for Program Flow Control

#define USB_FSM_IDLE            	0x0
#define USB_FSM_DATAIN        		0x1
#define USB_FSM_DATAOUT      	    0x2
#define USB_FSM_REQUESTPROC    		0x3
#define USB_FSM_HANDSHAKE        	0x4
#define USB_FSM_SETUPPROC      		0x5
#define USB_FSM_REQUESTPROC1  	    0x6
#define USB_FSM_STALL         		0x7
#define USB_FSM_REQUESTPROC2     	0x8
#define USB_FSM_REQUESTPROC3     	0x9
#define USB_FSM_CONTROLOUTDONE   	0x0A

// BMP for LOCK_CMD
#define D13REG_LOCK_IOEN                0x01

// BMP for system SetMode
#define D13REG_MODE_SOFTCONNECT         0x01
#define D13REG_MODE_OFFGOODLNK          0x02
#define D13REG_MODE_DBG                 0x04
#define D13REG_MODE_INT_EN              0x08
#define D13REG_MODE_SUSPND              0x20
#define D13REG_MODE_DMA16               0x80 // This is supposedly a reserved bit in the mode register

// BMP for system DcHardwareConfiguration register  (CNFG command)
#define D13REG_DEVCNFG_INTPOL          	0x0001
#define D13REG_DEVCNFG_INTEDGE        	0x0002
#define D13REG_DEVCNFG_PWROFF        	0x0004
#define D13REG_DEVCNFG_WAKEUPBY_CS    	0x0008
#define D13REG_DEVCNFG_EOTPOL        	0x0010
#define D13REG_DEVCNFG_DMAACKPOL      	0x0020
#define D13REG_DEVCNFG_DMARQPOL       	0x0040
#define D13REG_DEVCNFG_DMAACKONLY    	0x0080
#define D13REG_DEVCNFG_CLOCKDIV_MASK   	0x0F00
#define D13REG_DEVCNFG_CLOCKRUNNING   	0x1000
#define D13REG_DEVCNFG_NOLAZYCLOCK    	0x2000
#define D13REG_DEVCNFG_EXPULLUP      	0x4000


#define D13REG_DEVCNFG_CLOCKDIV_480M    0x0000
#define D13REG_DEVCNFG_CLOCKDIV_240M    0x0100
#define D13REG_DEVCNFG_CLOCKDIV_160M    0x0200
#define D13REG_DEVCNFG_CLOCKDIV_120M    0x0300
#define D13REG_DEVCNFG_CLOCKDIV_96M     0x0400
#define D13REG_DEVCNFG_CLOCKDIV_80M     0x0500
#define D13REG_DEVCNFG_CLOCKDIV_69M     0x0600
#define D13REG_DEVCNFG_CLOCKDIV_60M     0x0700
#define D13REG_DEVCNFG_CLOCKDIV_54M     0x0800
#define D13REG_DEVCNFG_CLOCKDIV_48M     0x0900
#define D13REG_DEVCNFG_CLOCKDIV_44M     0x0A00
#define D13REG_DEVCNFG_CLOCKDIV_40M     0x0B00
#define D13REG_DEVCNFG_CLOCKDIV_37M     0x0C00
#define D13REG_DEVCNFG_CLOCKDIV_34M     0x0D00
#define D13REG_DEVCNFG_CLOCKDIV_32M     0x0E00
#define D13REG_DEVCNFG_CLOCKDIV_30M     0x0F00

// Flags for Interrupt Enable Register
#define ENABLE_IRQ_BUSRESET          0x000001
#define ENABLE_IRQ_RESUME            0x000002
#define ENABLE_IRQ_SUSPEND           0x000004

#define ENABLE_IRQ_EOT               0x000008
#define ENABLE_IRQ_SOF               0x000010
#define ENABLE_IRQ_PSEUDO_SOF        0x000020
#define ENABLE_IRQ_SHORT_PACKET      0x000040

#define ENABLE_IRQ_EP0OUT            0x00000100
#define ENABLE_IRQ_EP0IN             0x00000200
#define ENABLE_IRQ_EP01              0x00000400
#define ENABLE_IRQ_EP02              0x00000800
#define ENABLE_IRQ_EP03              0x00001000
#define ENABLE_IRQ_EP04              0x00002000
#define ENABLE_IRQ_EP05              0x00004000
#define ENABLE_IRQ_EP06              0x00008000
#define ENABLE_IRQ_EP07              0x00010000
#define ENABLE_IRQ_EP08              0x00020000
#define ENABLE_IRQ_EP09              0x00040000
#define ENABLE_IRQ_EP0A              0x00080000
#define ENABLE_IRQ_EP0B              0x00100000
#define ENABLE_IRQ_EP0C              0x00200000
#define ENABLE_IRQ_EP0D              0x400000
#define ENABLE_IRQ_EP0E              0x800000

// BMP for INTERRUPT SRC
#define D13REG_INTSRC_BUSRESET          0x000001
#define D13REG_INTSRC_RESUME            0x000002
#define D13REG_INTSRC_SUSPEND           0x000004

#define D13REG_INTSRC_EOT               0x000008
#define D13REG_INTSRC_SOF               0x000010
#define D13REG_INTSRC_PSEUDO_SOF        0x000020
#define D13REG_INTSRC_SHORT_PACKET      0x000040

#define D13REG_INTSRC_EP0OUT            0x000100
#define D13REG_INTSRC_EP0IN             0x000200
#define D13REG_INTSRC_EP01              0x000400
#define D13REG_INTSRC_EP02              0x000800
#define D13REG_INTSRC_EP03              0x001000
#define D13REG_INTSRC_EP04              0x002000
#define D13REG_INTSRC_EP05              0x004000
#define D13REG_INTSRC_EP06              0x008000
#define D13REG_INTSRC_EP07              0x010000
#define D13REG_INTSRC_EP08              0x020000
#define D13REG_INTSRC_EP09              0x040000
#define D13REG_INTSRC_EP0A              0x080000
#define D13REG_INTSRC_EP0B              0x100000
#define D13REG_INTSRC_EP0C              0x200000
#define D13REG_INTSRC_EP0D              0x400000
#define D13REG_INTSRC_EP0E              0x800000

// 1362 Commands. Probably wont need these
#define HCCMD_RD_CNFG_REG 0x20
#define HCCMD_WR_CNFG_REG 0xA0

//******************************
// Setup Functions
//******************************

/* modified functions from HAL4D13.h in example project */
void usb_AcquireD13(unsigned char,void *);
void usb_UnlockDevice(unsigned char);
unsigned short usb_ReadChipID(void);
void usb_ResetDevice(void);

unsigned char usb_GetAddress(void);
void usb_SetAddressEnable(unsigned char, unsigned char);
void usb_SoftConnect(unsigned char);
void usb_AcknowledgeSETUP(void);

unsigned short usb_GetDevConfig(void);
void usb_SetDevConfig(unsigned short);
unsigned char usb_GetMode(void);
void usb_SetMode(unsigned char);

void usb_ConfigureEndpoints(void);
unsigned char usb_GetEndpointConfig(unsigned char);
void usb_SetEndpointConfig(unsigned char, unsigned char);
void usb_SetIntEnable(unsigned long); //Not written yet.

/* modified functions from MAINLOOP.h in example project */
void usb_DisconnectUSB(void);
void usb_ConnectUSB(void);

//******************************
// Communication Functions
//******************************

/* modified functions from HAL4D13.h in example project */
unsigned long usb_ReadInterruptRegister(void);

void usb_ClearBuffer(unsigned char);
void usb_ValidBuffer(unsigned char);

unsigned short usb_ReadEndpoint(unsigned char, unsigned char *, unsigned short);
unsigned short usb_ReadBulkEndpoint(unsigned char, unsigned char *, unsigned short);
unsigned short usb_ReadEndpointWOClearBuffer(unsigned char, unsigned char *, unsigned short);
unsigned short usb_WriteEndpoint(unsigned char, unsigned char *, unsigned short);
unsigned short usb_WriteBulkEndpoint(unsigned char, unsigned char *, unsigned short);
unsigned char usb_GetEndpointStatusWInteruptClear(unsigned char);
unsigned char usb_GetEndpointStatusWOInteruptClear(unsigned char);
void usb_SetEndpointStatus(unsigned char, unsigned char);

void usb_SetDMAConfig(unsigned short);
unsigned short usb_GetDMAConfig(void);
void usb_SetDMACounter(unsigned short);
unsigned short usb_GetDMACounter(void);

unsigned short usb_GetDataFromChipRam(void);
void usb_SetDataToChipRam(unsigned short);
unsigned short usb_ReadCurrentFrameNumber(void);
unsigned char usb_GetErrorCode(unsigned char);
void usb_SingleTransmitEP0(unsigned char *, unsigned short);
void usb_RegAccess(void);
void usb_StallEP0InControlRead(void);
void usb_StallEP0InControlWrite(void);
unsigned char usb_IsSetupPktInvalid(void);

#endif


