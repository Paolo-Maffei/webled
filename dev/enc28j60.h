#ifndef __ENC28J60_H__
#define __ENC28J60_H__

#include "spi.h"

//#define nop()	asm volatile ("nop")
#define BIT(x) (1L<<x)


// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address	(bits 0-4)
// - Bank number	(bits 5-6)
// - MAC/PHY indicator	(bit 7)
#define ADDR_MASK	0x1F
#define BANK_MASK	0x60
#define SPRD_MASK	0x80
// All-bank registers
#define EIE		0x1B
#define EIR		0x1C 
#define ESTAT		0x1D
#define ECON2		0x1E
#define ECON1		0x1F
// Bank 0 registers
#define ERDPTL		(0x00|0x00)
#define ERDPTH		(0x01|0x00)
#define EWRPTL		(0x02|0x00)
#define EWRPTH		(0x03|0x00)
#define ETXSTL		(0x04|0x00)
#define ETXSTH		(0x05|0x00)
#define ETXNDL		(0x06|0x00)
#define ETXNDH		(0x07|0x00)
#define ERXSTL		(0x08|0x00)
#define ERXSTH		(0x09|0x00)
#define ERXNDL		(0x0A|0x00)
#define ERXNDH		(0x0B|0x00)
#define ERXRDPTL	(0x0C|0x00)
#define ERXRDPTH	(0x0D|0x00)
#define ERXWRPTL	(0x0E|0x00)
#define ERXWRPTH	(0x0F|0x00)
#define EDMASTL		(0x10|0x00)
#define EDMASTH		(0x11|0x00)
#define EDMANDL		(0x12|0x00)
#define EDMANDH		(0x13|0x00)
#define EDMADSTL	(0x14|0x00)
#define EDMADSTH	(0x15|0x00)
#define EDMACSL		(0x16|0x00)
#define EDMACSH		(0x17|0x00)
// Bank 1 registers
#define EHT0		(0x00|0x20)
#define EHT1		(0x01|0x20)
#define EHT2		(0x02|0x20)
#define EHT3		(0x03|0x20)
#define EHT4		(0x04|0x20)
#define EHT5		(0x05|0x20)
#define EHT6		(0x06|0x20)
#define EHT7		(0x07|0x20)
#define EPMM0		(0x08|0x20)
#define EPMM1		(0x09|0x20)
#define EPMM2		(0x0A|0x20)
#define EPMM3		(0x0B|0x20)
#define EPMM4		(0x0C|0x20)
#define EPMM5		(0x0D|0x20)
#define EPMM6		(0x0E|0x20)
#define EPMM7		(0x0F|0x20)
#define EPMCSL		(0x10|0x20)
#define EPMCSH		(0x11|0x20)
#define EPMOL		(0x14|0x20)
#define EPMOH		(0x15|0x20)
#define EWOLIE		(0x16|0x20)
#define EWOLIR		(0x17|0x20)
#define ERXFCON		(0x18|0x20)
#define EPKTCNT		(0x19|0x20)
// Bank 2 registers
#define MACON1		(0x00|0x40|0x80)
#define MACON2		(0x01|0x40|0x80)
#define MACON3		(0x02|0x40|0x80)
#define MACON4		(0x03|0x40|0x80)
#define MABBIPG		(0x04|0x40|0x80)
#define MAIPGL		(0x06|0x40|0x80)
#define MAIPGH		(0x07|0x40|0x80)
#define MACLCON1	(0x08|0x40|0x80)
#define MACLCON2	(0x09|0x40|0x80)
#define MAMXFLL		(0x0A|0x40|0x80)
#define MAMXFLH		(0x0B|0x40|0x80)
#define MAPHSUP		(0x0D|0x40|0x80)
#define MICON		(0x11|0x40|0x80)
#define MICMD		(0x12|0x40|0x80)
#define MIREGADR	(0x14|0x40|0x80)
#define MIWRL		(0x16|0x40|0x80)
#define MIWRH		(0x17|0x40|0x80)
#define MIRDL		(0x18|0x40|0x80)
#define MIRDH		(0x19|0x40|0x80)
// Bank 3 registers
#define MAADR1		(0x00|0x60|0x80)
#define MAADR0		(0x01|0x60|0x80)
#define MAADR3		(0x02|0x60|0x80)
#define MAADR2		(0x03|0x60|0x80)
#define MAADR5		(0x04|0x60|0x80)
#define MAADR4		(0x05|0x60|0x80)
#define EBSTSD		(0x06|0x60)
#define EBSTCON		(0x07|0x60)
#define EBSTCSL		(0x08|0x60)
#define EBSTCSH		(0x09|0x60)
#define MISTAT		(0x0A|0x60|0x80)
#define EREVID		(0x12|0x60)
#define ECOCON		(0x15|0x60)
#define EFLOCON		(0x17|0x60)
#define EPAUSL		(0x18|0x60)
#define EPAUSH		(0x19|0x60)
// PHY registers
#define PHCON1		0x00
#define PHSTAT1		0x01
#define PHHID1		0x02
#define PHHID2		0x03
#define PHCON2		0x10
#define PHSTAT2		0x11
#define PHIE		0x12
#define PHIR		0x13
#define PHLCON		0x14


// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01

// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE	0x80
#define EIE_PKTIE	0x40
#define EIE_DMAIE	0x20
#define EIE_LINKIE	0x10
#define EIE_TXIE	0x08
#define EIE_WOLIE	0x04
#define EIE_TXERIE	0x02
#define EIE_RXERIE	0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF	0x40
#define EIR_DMAIF	0x20
#define EIR_LINKIF	0x10
#define EIR_TXIF	0x08
#define EIR_WOLIF	0x04
#define EIR_TXERIF	0x02
#define EIR_RXERIF	0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT	0x80
#define ESTAT_LATECOL	0x10
#define ESTAT_RXBUSY	0x04
#define ESTAT_TXABRT	0x02
#define ESTAT_CLKRDY	0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC	0x80
#define ECON2_PKTDEC	0x40
#define ECON2_PWRSV	0x20
#define ECON2_VRPS	0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST	0x80
#define	ECON1_RXRST	0x40
#define ECON1_DMAST	0x20
#define ECON1_CSUMEN	0x10
#define ECON1_TXRTS	0x08
#define	ECON1_RXEN	0x04
#define ECON1_BSEL1	0x02
#define ECON1_BSEL0	0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK	0x10
#define MACON1_TXPAUS	0x08
#define MACON1_RXPAUS	0x04
#define MACON1_PASSALL	0x02
#define MACON1_MARXEN	0x01
// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST	0x80
#define MACON2_RNDRST	0x40
#define MACON2_MARXRST	0x08
#define MACON2_RFUNRST	0x04
#define MACON2_MATXRST	0x02
#define MACON2_TFUNRST	0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2	0x80
#define MACON3_PADCFG1	0x40
#define MACON3_PADCFG0	0x20
#define MACON3_TXCRCEN	0x10
#define MACON3_PHDRLEN	0x08
#define MACON3_HFRMLEN	0x04
#define MACON3_FRMLNEN	0x02
#define MACON3_FULDPX	0x01

// MACON4 bits --------
#define	MACON4_DEFER	(1<<6)
#define	MACON4_BPEN		(1<<5)
#define	MACON4_NOBKOFF	(1<<4)

// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN	0x02
#define MICMD_MIIRD	0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID	0x04
#define MISTAT_SCAN	0x02
#define MISTAT_BUSY	0x01
// ENC28J60 PHY PHCON1 Register Bit Definitions
#define	PHCON1_PRST	0x8000
#define	PHCON1_PLOOPBK	0x4000
#define	PHCON1_PPWRSV	0x0800
#define	PHCON1_PDPXMD	0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define	PHSTAT1_PFDPX	0x1000
#define	PHSTAT1_PHDPX	0x0800
#define	PHSTAT1_LLSTAT	0x0004
#define	PHSTAT1_JBSTAT	0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK	0x4000
#define PHCON2_TXDIS	0x2000
#define PHCON2_JABBER	0x0400
#define PHCON2_HDLDIS	0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN		0x08
#define PKTCTRL_PPADEN		0x04
#define PKTCTRL_PCRCEN		0x02
#define PKTCTRL_POVERRIDE	0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG	0x00
#define ENC28J60_READ_BUF_MEM	0x3A
#define ENC28J60_WRITE_CTRL_REG	0x40
#define ENC28J60_WRITE_BUF_MEM	0x7A
#define ENC28J60_BIT_FIELD_SET	0x80
#define ENC28J60_BIT_FIELD_CLR	0xA0
#define ENC28J60_SOFT_RESET	0xFF


// buffer boundaries applied to internal 8K ram
//	entire available packet buffer space is allocated
/*
#define TXSTART_INIT   	0x0000	// start TX buffer at 0
#define RXSTART_INIT   	0x0600	// give TX buffer space for one full ethernet frame (~1500 bytes)
#define RXSTOP_INIT    	0x1FFF	// receive buffer gets the rest
#define	MAX_FRAMELEN	1518	// maximum ethernet frame length
*/
// Ethernet constants
#define ETHERNET_MIN_PACKET_LENGTH	0x3C
//#define ETHERNET_HEADER_LENGTH		0x0E

        // filter shitt einstellen
        // bit7 UCEN: Unicast Filter Enable bit 1 = Packets not having a destination address matching the local MAC address will be discarded
        // bit6 ANDOR: AND/OR Filter Select bit 1 = AND: Packets will be rejected unless all enabled filters accept the packet
        // bit5 CRCEN: Post-Filter CRC Check Enable bit 1 = All packets with an invalid CRC will be discarded
        // bit4 PMEN: Pattern Match Filter Enable bit 1 = Packets must meet the pattern match criteria or they will be discarded
        // bit3 MPEN: Magic Packet Filter Enable bit 1 = Packets must be Magic Packets for the local MAC address or they will be discarded
        // bit2 HTEN: Hash Table Filter Enable bit 1 = Packets must meet the hash table criteria or they will be discarded
        // bit1 MCEN: Multicast Filter Enable bit 1 = Packets must have the Least Significant bit set in the destination address or they will be discarded
        // bit0 BCEN: Broadcast Filter Enable bit 1 = Packets must have a destination address of FF-FF-FF-FF-FF-FF or they will be discarded
#define UCEN      BIT(7)
#define ANDOR     BIT(6)
#define CRCEN     BIT(5)
#define PMEN      BIT(4)
#define MPEN      BIT(3)
#define HTEN      BIT(2)
#define MCEN      BIT(1)
#define BCEN      BIT(0)

// The RXSTART_INIT should be zero. See Rev. B4 Silicon Errata
// buffer boundaries applied to internal 8K ram
// the entire available packet buffer space is allocated
//
// start with recbuf at 0/
#define RXSTART_INIT     0x0
// receive buffer end
#define RXSTOP_INIT      (0x1FFF-0x0C00-1)
// start TX buffer at 0x1FFF-0x0600, pace for one full ethernet frame (~1500 bytes)
#define TXSTART_INIT     (0x1FFF-0x0C00)
// stp TX buffer at end of mem
#define TXSTOP_INIT      0x1FFF
//
// max frame length which the conroller will accept:
#define        MAX_FRAMELEN        1518        // (note: maximum ethernet frame length would be 1518)
//#define MAX_FRAMELEN     600

typedef union _WORD_VAL
{
    uint16_t Val;
    uint8_t v[2];
    struct
    {
        uint8_t LB;
        uint8_t HB;
    } byte;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
    } bits;
} WORD_VAL, WORD_BITS;

typedef union {
	uint8_t v[7];
	struct {
		uint16_t	 		ByteCount;
		unsigned char	CollisionCount:4;
		unsigned char	CRCError:1;
		unsigned char	LengthCheckError:1;
		unsigned char	LengthOutOfRange:1;
		unsigned char	Done:1;
		unsigned char	Multicast:1;
		unsigned char	Broadcast:1;
		unsigned char	PacketDefer:1;
		unsigned char	ExcessiveDefer:1;
		unsigned char	MaximumCollisions:1;
		unsigned char	LateCollision:1;
		unsigned char	Giant:1;
		unsigned char	Underrun:1;
		uint16_t 	 		BytesTransmittedOnWire;
		unsigned char	ControlFrame:1;
		unsigned char	PAUSEControlFrame:1;
		unsigned char	BackpressureApplied:1;
		unsigned char	VLANTaggedFrame:1;
		unsigned char	Zeros:4;
	} bits;
} TXSTATUS;


// functions

static void GetMacAddress(uint8_t *macaddr);
// setup ports for I/O
static void SetMacAddress(const uint8_t *macaddr);
//! do a ENC28J60 read operation
static uint8_t enc28j60ReadOp(uint8_t op, uint8_t address);
//! do a ENC28J60 write operation
static void enc28j60WriteOp(uint8_t op, uint8_t address, uint8_t data);
//! read the packet buffer memory
static void enc28j60ReadBuffer(uint16_t len, uint8_t* data);
//! write the packet buffer memory
static void enc28j60WriteBuffer(uint16_t len, uint8_t* data);
//! set the register bank for register at address
static void enc28j60SetBank(uint8_t address);
//! read ax88796 register
static uint8_t enc28j60Read(uint8_t address);
//! write ax88796 register
static void enc28j60Write(uint8_t address, uint8_t data);
//! read a PHY register
static uint16_t enc28j60PhyRead(uint8_t address);
//! write a PHY register
static void enc28j60PhyWrite(uint8_t address, uint16_t data);

//! initialize the ethernet interface for transmit/receive
void enc28j60Init(uint8_t *mymac);

//! Packet transmit function.
/// Sends a packet on the network.  It is assumed that the packet is headed by a valid ethernet header.
/// \uip_len		Length of packet in bytes.
/// \uip_buf    	Pointer to packet data.
void enc28j60PacketSend(void);

// Packet receive function.
// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.

uint16_t enc28j60PacketReceive(void);

void enc28j60clkout(unsigned char clk);
char enc28j60_powersave(uint8_t mode);

#endif

